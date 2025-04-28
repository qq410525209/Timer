#include "menu.h"
#include <core/memory.h>
#include <core/concmdmanager.h>
#include <utils/utils.h>
#include <sdk/entity/cbaseentity.h>

#include <fmt/format.h>

constexpr float g_fMenuDefaultOffsetX_Alive = -8.9f;
constexpr float g_fMenuDefaultOffsetY_Alive = 2.4f;

CScreenTextMenu::CScreenTextMenu(CBasePlayerController* pController, MenuHandler fnHandler, std::string sTitle) : CBaseMenu(pController, fnHandler, sTitle) {
	ScreenTextManifest_t manifest;
	manifest.m_iUnits = 1000;
	manifest.m_sFont = "Arial Bold";
	manifest.m_Color = Color(255, 165, 0, 255);
	manifest.m_fFontSize = 40;
	manifest.m_bEnable = false;
	manifest.m_vecPos.x = g_fMenuDefaultOffsetX_Alive;
	manifest.m_vecPos.y = g_fMenuDefaultOffsetY_Alive;
	manifest.m_bBackground = true;

	m_wpScreenText = VGUI::CreateScreenText(pController, manifest);
}

CScreenTextMenu::~CScreenTextMenu() {
	CBaseMenu::~CBaseMenu();
	this->Close();
}

void CScreenTextMenu::Display(int iPageIndex) {
	if (m_wpScreenText.expired()) {
		SDK_ASSERT(false);
		return;
	}

	auto pMenu = m_wpScreenText.lock();
	auto pController = pMenu->GetOriginalController();
	if (!pController) {
		return;
	}

	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return;
	}

	pMenuPlayer->m_nCurrentPage = iPageIndex;
	pMenuPlayer->ClampItem();
	auto iCurrentItem = pMenuPlayer->m_nCurrentItem;
	auto bWSAD = pMenuPlayer->m_bWSADMenu;

	auto formatItem = [iCurrentItem, bWSAD](int iItemIndex, const std::string& sItem) -> std::string {
		if (sItem.empty()) {
			return "";
		}

		if (bWSAD) {
			return fmt::format("{}{}", iItemIndex == iCurrentItem ? "> " : "", sItem);
		}

		return fmt::format("{}.{}", iItemIndex + 1, sItem);
	};
	bool bDrawPreview = (iPageIndex != 0);
	bool bDrawNext = ((this->m_vPage.size() > 0) && (iPageIndex < (this->m_vPage.size() - 1)));

	// clang-format off
	std::string sMenuText = fmt::format("{}\n\n"
										"{}\n"
										"{}\n"
										"{}\n"
										"{}\n"
										"{}\n"
										"{}\n\n"
										"{}",
										this->m_sTitle, 
										formatItem(0, GetItem(iPageIndex, 0).first), 
										formatItem(1, GetItem(iPageIndex, 1).first), 
										formatItem(2, GetItem(iPageIndex, 2).first), 
										formatItem(3, GetItem(iPageIndex, 3).first), 
										formatItem(4, GetItem(iPageIndex, 4).first), 
										formatItem(5, GetItem(iPageIndex, 5).first), 
										!bWSAD ? fmt::format("{}\n"
															"{}\n"
															"{}",
															formatItem(6, bDrawPreview ? "上一页" : ""), 
															formatItem(7, bDrawNext ? "下一页" : ""),
															formatItem(8, "退出"))
											 : fmt::format("A/D: 翻页, W/S: 滚动\n"
														   "E/F: 选择, Shift: 退出"));
	// clang-format on

	pMenu->SetText(sMenuText.c_str());
	VGUI::Render(m_wpScreenText);
}

bool CScreenTextMenu::Close() {
	if (m_wpScreenText.expired()) {
		return false;
	}

	VGUI::Unrender(m_wpScreenText);

	return true;
}

void CBaseMenu::AddItem(std::string sItem, std::optional<std::any> data) {
	if (m_vPage.empty() || m_vPage.back().size() >= PAGE_SIZE) {
		AllocatePage();
	}

	MenuItemType item {sItem, data.has_value() ? data.value() : std::any()};
	m_vPage.back().emplace_back(item);
}

const CBaseMenu::MenuItemType& CBaseMenu::GetItem(int iPageIndex, int iItemIndex) const {
	if (iPageIndex < 0 || iPageIndex >= this->m_vPage.size()) {
		SDK_ASSERT(false);
		return CBaseMenu::NULL_ITEM;
	}

	if (iItemIndex < 0) {
		SDK_ASSERT(false);
		return CBaseMenu::NULL_ITEM;
	}

	auto& vItems = m_vPage[iPageIndex];
	auto iActualItemIdx = iItemIndex % CBaseMenu::PAGE_SIZE;
	if (iActualItemIdx < vItems.size()) {
		return vItems.at(iActualItemIdx);
	}

	return CBaseMenu::NULL_ITEM;
}

const CBaseMenu::MenuItemType& CBaseMenu::GetItem(int iItemIndex) const {
	auto pController = m_hController.Get();
	if (!pController) {
		SDK_ASSERT(false);
		return CBaseMenu::NULL_ITEM;
	}

	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return CBaseMenu::NULL_ITEM;
	}

	return GetItem(pMenuPlayer->m_nCurrentPage, iItemIndex);
}

void CMenuPlayer::ResetMenu(bool bResetMode) {
	if (m_pCurrentMenu) {
		m_pCurrentMenu.reset();
	}

	m_nCurrentPage = 0;
	m_nCurrentItem = 0;

	if (bResetMode) {
		m_bWSADMenu = false;
	}
}

CMenuManager g_MenuManager;

CMenuManager* MENU::GetManager() {
	return &g_MenuManager;
}

void CMenuPlayer::SelectMenu() {
	auto pController = GetController();
	if (!pController) {
		return;
	}

	const auto& pMenu = m_pCurrentMenu;
	if (!pMenu) {
		return;
	}

	auto iMenuType = pMenu->GetType();
	if (iMenuType == EMenuType::Unknown) {
		SDK_ASSERT(false);
		return;
	}

	this->ClampItem();

	switch (m_nCurrentItem) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: {
			if (pMenu->m_pFnMenuHandler) {
				int iItemIndex = (m_nCurrentPage * CBaseMenu::PAGE_SIZE) + m_nCurrentItem;
				CMenuHandle hMenu(pMenu);
				pMenu->m_pFnMenuHandler(hMenu, pController, iItemIndex);
				UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_SELECT);
			}

			break;
		}
		case 6: {
			this->DisplayPagePrev();
			break;
		}
		case 7: {
			this->DisplayPageNext();
			break;
		}
		case 8: {
			this->CloseMenu();
			break;
		}
	}
}

void CMenuPlayer::SwitchMode(bool bRedraw) {
	m_bWSADMenu = !m_bWSADMenu;

	if (m_pCurrentMenu && bRedraw) {
		m_pCurrentMenu->Display(m_nCurrentPage);
	}
}

void CMenuPlayer::DisplayPagePrev() {
	int iPrevPageIndex = m_nCurrentPage - 1;
	if (iPrevPageIndex >= 0 && iPrevPageIndex < m_pCurrentMenu->GetPageLength()) {
		m_pCurrentMenu->Display(iPrevPageIndex);
		UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_SELECT);
	}
}

void CMenuPlayer::DisplayPageNext() {
	int iNextPageIndex = m_nCurrentPage + 1;
	if (iNextPageIndex >= 0 && iNextPageIndex < m_pCurrentMenu->GetPageLength()) {
		m_pCurrentMenu->Display(iNextPageIndex);
		UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_SELECT);
	}
}

void CMenuPlayer::Refresh() const {
	m_pCurrentMenu->Display(m_nCurrentPage);
}

void CMenuPlayer::CloseMenu() {
	ResetMenu();
	UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_EXIT);
}

void CMenuPlayer::ClampItem() {
	if (!m_bWSADMenu) {
		return;
	}

	uint nItemSize = (uint)m_pCurrentMenu->m_vPage[m_nCurrentPage].size();
	if (m_nCurrentItem == UINT_MAX) {
		m_nCurrentItem = nItemSize - 1;
	} else if (m_nCurrentItem >= nItemSize) {
		m_nCurrentItem = 0;
	}
}

CCMD_CALLBACK(OnMenuItemSelect) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return;
	}

	if (!pMenuPlayer->m_pCurrentMenu) {
		return;
	}

	int num = -1;
	if (vArgs.size() > 0) {
		num = V_StringToInt32(vArgs[0].c_str(), -1) - 1;
	}

	pMenuPlayer->m_nCurrentItem = num;
	pMenuPlayer->SelectMenu();
}

CCMD_CALLBACK(OnNumberSelect) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return;
	}

	if (!pMenuPlayer->m_pCurrentMenu) {
		return;
	}

	pMenuPlayer->m_nCurrentItem = wCommand[wCommand.length() - 2] - L'0';
	pMenuPlayer->m_nCurrentItem--;
	pMenuPlayer->SelectMenu();
}

CCMD_CALLBACK(OnMenuModeChange) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return;
	}

	pMenuPlayer->SwitchMode(true);
}

void CMenuManager::OnPluginStart() {
	CONCMD::RegConsoleCmd("sm_menu_select", OnMenuItemSelect);

	for (int i = 1; i <= 9; i++) {
		std::string sMenuCmd = fmt::format("sm_{}", i);
		CONCMD::RegConsoleCmd(sMenuCmd, OnNumberSelect);
	}

	CONCMD::RegConsoleCmd("sm_mma", OnMenuModeChange);
}

void CMenuManager::OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pPawn);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return;
	}

	if (!pMenuPlayer->m_pCurrentMenu || !pMenuPlayer->m_bWSADMenu) {
		return;
	}

	if (buttons.Pressed(IN_FORWARD)) {
		pMenuPlayer->m_nCurrentItem--;
		pMenuPlayer->Refresh();
	} else if (buttons.Pressed(IN_BACK)) {
		pMenuPlayer->m_nCurrentItem++;
		pMenuPlayer->Refresh();
	} else if (buttons.Pressed(IN_MOVELEFT)) {
		pMenuPlayer->DisplayPagePrev();
	} else if (buttons.Pressed(IN_MOVERIGHT)) {
		pMenuPlayer->DisplayPageNext();
	} else if (buttons.Pressed(IN_USE) || buttons.Pressed(IN_LOOK_AT_WEAPON)) {
		pMenuPlayer->SelectMenu();
	} else if (buttons.Pressed(IN_SPEED)) {
		pMenuPlayer->CloseMenu();
	}
}

std::weak_ptr<CBaseMenu> MENU::Create(CBasePlayerController* pController, MenuHandler pFnMenuHandler, EMenuType eMenuType) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return {};
	}

	switch (eMenuType) {
		case EMenuType::ScreenText: {
			pMenuPlayer->ResetMenu();

			auto pMenu = std::make_shared<CScreenTextMenu>(pController, pFnMenuHandler);
			pMenuPlayer->m_pCurrentMenu = pMenu;
			return pMenu;
			break;
		}
	}

	return {};
}

bool MENU::Close(CBasePlayerController* pController) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return false;
	}

	pMenuPlayer->ResetMenu();
	return true;
}

bool CMenuHandle::Close() {
	if (!IsValid()) {
		return false;
	}

	auto pController = Data()->m_hController.Get();
	if (!pController) {
		SDK_ASSERT(false);
		return false;
	}

	return MENU::Close(pController);
}
