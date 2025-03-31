#include "menu.h"
#include <core/memory.h>
#include <core/concmdmanager.h>
#include <utils/utils.h>
#include <sdk/entity/cbaseentity.h>

#include <fmt/format.h>

constexpr float g_fMenuOffsetX = -9.1f;
constexpr float g_fMenuOffsetY = 6.45f;

CScreenTextMenu::CScreenTextMenu(CBasePlayerController* pController, MenuHandler fnHandler, std::string sTitle) : CBaseMenu(fnHandler, sTitle) {
	ScreenTextManifest_t manifest;
	manifest.m_iUnits = 1000;
	manifest.m_sFont = "Consolas";
	manifest.m_Color = Color(255, 100, 255, 255);
	manifest.m_fFontSize = 40;
	manifest.m_bEnable = false;
	manifest.m_vecPos.x = g_fMenuOffsetX;
	manifest.m_vecPos.y = g_fMenuOffsetY;

	m_wpScreenText = VGUI::CreateScreenText(pController, manifest);
}

CScreenTextMenu::~CScreenTextMenu() {
	CBaseMenu::~CBaseMenu();
}

void CScreenTextMenu::Display(int iPageIndex) {
	if (m_wpScreenText.expired()) {
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
	bool bDrawNext = ((this->m_vItems.size() > 0) && (iPageIndex < (this->m_vItems.size() - 1)));

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
										formatItem(1, GetItem(iPageIndex, 0)), 
										formatItem(2, GetItem(iPageIndex, 1)), 
										formatItem(3, GetItem(iPageIndex, 2)), 
										formatItem(4, GetItem(iPageIndex, 3)), 
										formatItem(5, GetItem(iPageIndex, 4)), 
										formatItem(6, GetItem(iPageIndex, 5)), 
										formatItem(7, bDrawPreview ? "上一页" : ""), 
										formatItem(8, bDrawNext ? "下一页" : ""));
	// clang-format on

	pMenu->SetText(sMenuText.c_str());
	VGUI::Render(m_wpScreenText);
}

bool CScreenTextMenu::Close() {
	if (this->m_wpScreenText.expired()) {
		SDK_ASSERT(false);
		return false;
	}

	VGUI::Unrender(this->m_wpScreenText);

	if (auto pText = this->m_wpScreenText.lock()) {
		auto pController = pText->GetOriginalController();
		if (!pController) {
			SDK_ASSERT(false);
			return false;
		}

		auto pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
		if (!pMenuPlayer) {
			SDK_ASSERT(false);
			return false;
		}

		pMenuPlayer->ResetMenu();
	}

	return true;
}

std::string CBaseMenu::GetItem(int iPageIndex, int iItemIndex) {
	if (iPageIndex < 0 || iPageIndex >= this->m_vItems.size()) {
		SDK_ASSERT(false);
		return "";
	}

	if (iItemIndex < 0) {
		SDK_ASSERT(false);
		return "";
	}

	auto& vItems = m_vItems[iPageIndex];
	if (iItemIndex < vItems.size()) {
		return vItems[iItemIndex];
	}

	return "";
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

void CMenuManager::OnMenuItemSelect(CCSPlayerController* pController, const std::vector<std::string>& vArgs) {
	CMenuPlayer* pMenuPlayer = MENU::GetManager()->ToPlayer(pController);
	if (!pMenuPlayer) {
		SDK_ASSERT(false);
		return;
	}

	if (!pMenuPlayer->m_pCurrentMenu) {
		return;
	}

	const auto& pMenu = pMenuPlayer->m_pCurrentMenu;
	auto iMenuType = pMenu->GetType();
	if (iMenuType == EMenuType::Unknown) {
		SDK_ASSERT(false);
		return;
	}

	int num = -1;
	if (vArgs.size() > 0) {
		num = V_StringToInt32(vArgs[0].c_str(), -1);
	}

	switch (num) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6: {
			if (pMenu->m_pFnMenuHandler) {
				int iItemIndex = (pMenuPlayer->m_iCurrentPage * CBaseMenu::PAGE_SIZE) + num - 1;
				switch (iMenuType) {
					case EMenuType::ScreenText: {
						CScreenTextMenuHandle hMenu(pMenu);
						pMenu->m_pFnMenuHandler(hMenu, pController, iItemIndex);
						break;
					}
					default: {
						SDK_ASSERT(false);
						break;
					}
				}
			}

			break;
		}
		case 7: {
			int iPrevPageIndex = pMenuPlayer->m_iCurrentPage - 1;
			if (iPrevPageIndex >= 0 && iPrevPageIndex < pMenu->GetPageLength()) {
				pMenu->Display(iPrevPageIndex);
			}
			break;
		}
		case 8: {
			int iNextPageIndex = pMenuPlayer->m_iCurrentPage + 1;
			if (iNextPageIndex >= 0 && iNextPageIndex < pMenu->GetPageLength()) {
				pMenu->Display(iNextPageIndex);
			}
			break;
		}
		case 9: {
			// prev, next menu not impl yet.
			pMenuPlayer->m_pCurrentMenu->Close();
			break;
		}
	}
}

void CMenuManager::OnPluginStart() {
	CONCMD::RegConsoleCmd("sm_menu_select", OnMenuItemSelect);
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

bool CScreenTextMenuHandle::Close() {
	if (auto pMenu = this->m_wpData.lock()) {
		auto pScreenTextMenu = std::dynamic_pointer_cast<CScreenTextMenu>(pMenu);
		if (!pScreenTextMenu) {
			SDK_ASSERT(false);
			return false;
		}

		return pScreenTextMenu->Close();
	} else {
		return false;
	}
}
