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
	using MenuItemType = std::pair<std::string, std::any>;
	static inline const MenuItemType NULL_ITEM = {};
	static const size_t PAGE_SIZE = 6;

public:
	CBaseMenu(CBasePlayerController* pController, MenuHandler pFnHandler, std::string sTitle = "") : m_hController(pController->GetRefEHandle()), m_pFnMenuHandler(pFnHandler), m_sTitle(sTitle) {
		AllocatePage();
	}

	virtual ~CBaseMenu() {}

	virtual EMenuType GetType() {
		return EMenuType::Unknown;
	}

	virtual void Display(int iPageIndex = 0) = 0;
	virtual bool Close() = 0;

public:
	void SetTitle(const std::string_view& sTitle) {
		m_sTitle = sTitle;
	}

	void AddItem(std::string sItem, std::optional<std::any> data = std::nullopt);

	size_t GetPageLength() const {
		return m_vPage.size();
	}

	size_t GetItemLength() const {
		if (m_vPage.size() == 0) {
			return 0;
		}

		return ((m_vPage.size() - 1) * PAGE_SIZE) + m_vPage.back().size();
	}

	const MenuItemType& GetItem(int iPageIndex, int iItemIndex) const;
	const MenuItemType& GetItem(int iItemIndex) const;

private:
	void AllocatePage() {
		std::vector<MenuItemType> nullItems;
		nullItems.reserve(PAGE_SIZE);
		m_vPage.emplace_back(nullItems);
	}

public:
	std::string m_sTitle;
	std::vector<std::vector<MenuItemType>> m_vPage;
	MenuHandler m_pFnMenuHandler;
	CHandle<CBasePlayerController> m_hController;
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
	bool Close(CBasePlayerController* pController);
} // namespace MENU
