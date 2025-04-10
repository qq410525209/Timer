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

	pMenuPlayer->m_iCurrentPage = iPageIndex;

	auto formatItem = [](int iItemIndex, const std::string& sItem) { return !sItem.empty() ? fmt::format("{}.{}", iItemIndex, sItem) : ""; };
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
										"{}\n"
										"{}\n"
										"9.退出",
										this->m_sTitle, 
										formatItem(1, GetItem(iPageIndex, 0).first), 
										formatItem(2, GetItem(iPageIndex, 1).first), 
										formatItem(3, GetItem(iPageIndex, 2).first), 
										formatItem(4, GetItem(iPageIndex, 3).first), 
										formatItem(5, GetItem(iPageIndex, 4).first), 
										formatItem(6, GetItem(iPageIndex, 5).first), 
										formatItem(7, bDrawPreview ? "上一页" : ""), 
										formatItem(8, bDrawNext ? "下一页" : ""));
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
	if (iItemIndex < vItems.size()) {
		return vItems.at(iItemIndex);
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

	return GetItem(pMenuPlayer->m_iCurrentPage, iItemIndex);
}

void CMenuPlayer::ResetMenu() {
	if (m_pCurrentMenu) {
		m_pCurrentMenu.reset();
	}

	m_iCurrentPage = 0;
}

CMenuManager g_MenuManager;

CMenuManager* MENU::GetManager() {
	return &g_MenuManager;
}

void CMenuPlayer::SelectMenu(int iMenuItem) {
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

	switch (iMenuItem) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6: {
			if (pMenu->m_pFnMenuHandler) {
				int iItemIndex = (m_iCurrentPage * CBaseMenu::PAGE_SIZE) + iMenuItem - 1;
				CMenuHandle hMenu(pMenu);
				pMenu->m_pFnMenuHandler(hMenu, pController, iItemIndex);
				UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_SELECT);
			}

			break;
		}
		case 7: {
			int iPrevPageIndex = m_iCurrentPage - 1;
			if (iPrevPageIndex >= 0 && iPrevPageIndex < pMenu->GetPageLength()) {
				pMenu->Display(iPrevPageIndex);
				UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_SELECT);
			}
			break;
		}
		case 8: {
			int iNextPageIndex = m_iCurrentPage + 1;
			if (iNextPageIndex >= 0 && iNextPageIndex < pMenu->GetPageLength()) {
				pMenu->Display(iNextPageIndex);
				UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_SELECT);
			}
			break;
		}
		case 9: {
			ResetMenu();
			UTIL::PlaySoundToClient(GetPlayerSlot(), MENU_SND_EXIT);
			break;
		}
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
		num = V_StringToInt32(vArgs[0].c_str(), -1);
	}

	pMenuPlayer->SelectMenu(num);
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

	int num = wCommand[wCommand.length() - 2] - L'0';
	pMenuPlayer->SelectMenu(num);
}

void CMenuManager::OnPluginStart() {
	CONCMD::RegConsoleCmd("sm_menu_select", OnMenuItemSelect);

	for (int i = 1; i <= 9; i++) {
		std::string sMenuCmd = fmt::format("sm_{}", i);
		CONCMD::RegConsoleCmd(sMenuCmd, OnNumberSelect);
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
