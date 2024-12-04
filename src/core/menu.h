#pragma once

#include <pch.h>
#include <core/playermanager.h>

enum class EMenuType {
	POINT_WORLDTEXT = 0
};

class CBaseMenu;
using MenuHandler = std::function<void(CBaseMenu* pMenu, CBasePlayerController* pController, int iSelectedItem)>;

class CBaseMenu {
	static const size_t PAGE_SIZE = 6;

public:
	CBaseMenu(MenuHandler pFnHandler, std::string sTitle = "") : m_pFnMenuHandler(pFnHandler), m_sTitle(sTitle) {
		AllocatePage();
	}

	virtual ~CBaseMenu() {}

	virtual void SetTitle(const std::string_view& sTitle) {
		m_sTitle = sTitle;
	}

	virtual void SetPrevious(CBaseMenu* pPrevious) {
		m_pLast = pPrevious;
	}

	virtual void SetNext(CBaseMenu* pNext) {
		m_pNext = pNext;
	}

	virtual void AddItem(std::string sItem) {
		if (m_vItems.empty() || m_vItems.back().size() >= PAGE_SIZE) {
			AllocatePage();
		}

		m_vItems.back().emplace_back(sItem);
	}

	virtual void OnItemSelect(CBasePlayerController* pController, int iSelectedItem) {
		if (m_pFnMenuHandler) {
			m_pFnMenuHandler(this, pController, iSelectedItem);
		}
	};

	virtual std::string GetItem(int iPageIndex, int iItemIndex);

public:
	virtual void Display(CCSPlayerPawnBase* pPawn, int iPageIndex = 0) = 0;

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
	std::string m_sTitle;
	std::vector<std::vector<std::string>> m_vItems;

protected:
	MenuHandler m_pFnMenuHandler;
	CBaseMenu* m_pLast;
	CBaseMenu* m_pNext;
};

class CWorldTextMenu : public CBaseMenu {
public:
	CWorldTextMenu(MenuHandler pFnHandler, std::string sTitle = "");

	virtual ~CWorldTextMenu() override;
	virtual void Display(CCSPlayerPawnBase* pPawn, int iPageIndex = 0) override;

public:
	static Vector GetAimPoint(const Vector& eyePosition, const QAngle& eyeAngles, float distanceToTarget = 100.0);

private:
	CHandle<CPointWorldText> m_hWorldText;
	CHandle<CPointWorldText> m_hBackground;
};

class CMenuPlayer : public CPlayer {
public:
	using CPlayer::CPlayer;

	virtual void Init(int iSlot) override {
		CPlayer::Init(iSlot);

		ResetMenu();
	}

	virtual void Reset() override {
		CPlayer::Reset();

		ResetMenu();
	}

private:
	void ResetMenu() {
		if (m_pCurrentMenu) {
			delete m_pCurrentMenu;
			m_pCurrentMenu = nullptr;
		}

		m_iCurrentPage = 0;
	}

public:
	CBaseMenu* m_pCurrentMenu = nullptr;
	int m_iCurrentPage;
};

class CMenuManager : CPlayerManager {
public:
	CMenuManager() {
		for (int i = 0; i < MAXPLAYERS; i++) {
			m_pPlayers[i] = std::make_unique<CMenuPlayer>(i);
		}
	}

	CMenuPlayer* ToMenuPlayer(CPlayer* pPlayer) {
		return static_cast<CMenuPlayer*>(pPlayer);
	}

private:
	static void OnMenuItemSelect(CCSPlayerController* pController, const std::vector<std::string>& vArgs);

private:
	virtual void OnPluginStart() override;
};

namespace MENU {
	extern CMenuManager* GetManager();

	[[nodiscard]] CBaseMenu* Create(MenuHandler pMenuHandler, EMenuType eMenuType = EMenuType::POINT_WORLDTEXT);
} // namespace MENU
