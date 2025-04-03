#pragma once

#include <pch.h>
#include <core/playermanager.h>
#include <core/screentext.h>

constexpr auto MENU_SND_SELECT = "UIPanorama.submenu_select";
constexpr auto MENU_SND_EXIT = "UIPanorama.submenu_slidein";

enum class EMenuType {
	Unknown = 0,
	ScreenText = 1
};

using CMenuHandle = CWeakHandle<class CBaseMenu>;

#define MENU_CALLBACK_ARGS   CMenuHandle &hMenu, CBasePlayerController *pController, int iItem
#define MENU_CALLBACK(fn)    static void fn(MENU_CALLBACK_ARGS)
#define MENU_CALLBACK_L(...) [__VA_ARGS__](MENU_CALLBACK_ARGS) -> void
using MenuHandler = std::function<void(MENU_CALLBACK_ARGS)>;

class CBaseMenu {
public:
	CBaseMenu(MenuHandler pFnHandler, std::string sTitle = "") : m_pFnMenuHandler(pFnHandler), m_sTitle(sTitle) {
		AllocatePage();
	}

	virtual ~CBaseMenu() {
		m_pFnMenuHandler = nullptr;
	}

	virtual void SetTitle(const std::string_view& sTitle) {
		m_sTitle = sTitle;
	}

	virtual void AddItem(std::string sItem) {
		if (m_vItems.empty() || m_vItems.back().size() >= PAGE_SIZE) {
			AllocatePage();
		}

		m_vItems.back().emplace_back(sItem);
	}

	virtual EMenuType GetType() {
		return EMenuType::Unknown;
	}

	virtual std::string GetItem(int iPageIndex, int iItemIndex);

	virtual void Display(int iPageIndex = 0) = 0;
	virtual bool Close() = 0;

public:
	size_t GetPageLength() const {
		return m_vItems.size();
	}

private:
	void AllocatePage() {
		std::vector<std::string> nullItems;
		nullItems.reserve(PAGE_SIZE);
		m_vItems.emplace_back(nullItems);
	}

public:
	static const size_t PAGE_SIZE = 6;
	std::string m_sTitle;
	std::vector<std::vector<std::string>> m_vItems;
	MenuHandler m_pFnMenuHandler;
};

class CScreenTextMenu : public CBaseMenu {
public:
	CScreenTextMenu(CBasePlayerController* pController, MenuHandler pFnHandler, std::string sTitle = "");

	virtual EMenuType GetType() override {
		return EMenuType::ScreenText;
	}

public:
	virtual ~CScreenTextMenu() override;
	virtual void Display(int iPageIndex = 0) override;
	virtual bool Close() override;

public:
	std::weak_ptr<CScreenText> m_wpScreenText;
};

class CScreenTextMenuHandle : public CMenuHandle {
public:
	using CMenuHandle::CMenuHandle;

	virtual bool Close() override;
};

class CMenuPlayer : public CPlayer {
private:
	virtual void Reset() override {
		CPlayer::Reset();

		ResetMenu();
	}

public:
	using CPlayer::CPlayer;

	void ResetMenu();
	void SelectMenu(int iMenuItem);

public:
	std::shared_ptr<CBaseMenu> m_pCurrentMenu;
	int m_iCurrentPage;
};

class CMenuManager : CPlayerManager {
public:
	CMenuManager() {
		for (int i = 0; i < MAXPLAYERS; i++) {
			m_pPlayers[i] = std::make_unique<CMenuPlayer>(i);
		}
	}

	virtual CMenuPlayer* ToPlayer(CBasePlayerController* controller) const override {
		return static_cast<CMenuPlayer*>(CPlayerManager::ToPlayer(controller));
	}

	virtual CMenuPlayer* ToPlayer(CBasePlayerPawn* pawn) const override {
		return static_cast<CMenuPlayer*>(CPlayerManager::ToPlayer(pawn));
	}

private:
	virtual void OnPluginStart() override;
};

namespace MENU {
	extern CMenuManager* GetManager();

	[[nodiscard]] std::weak_ptr<CBaseMenu> Create(CBasePlayerController* pController, MenuHandler pFnMenuHandler, EMenuType eMenuType = EMenuType::ScreenText);
	void Close(CBasePlayerController* pController);
} // namespace MENU
