#include <surf/timer/surf_timer.h>
#include <surf/zones/surf_zones.h>

CSurfPlayerManager g_SurfPlayerManager;

template<>
CSurfForward* CBaseForward<CSurfForward>::m_pFirst = nullptr;

template<>
CSurfBaseService* CBaseForward<CSurfBaseService>::m_pFirst = nullptr;

CSurfPlayerManager* SURF::GetPlayerManager() {
	return &g_SurfPlayerManager;
}

CMovementPlayerManager* MOVEMENT::GetPlayerManager() {
	return static_cast<CMovementPlayerManager*>(&g_SurfPlayerManager);
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CServerSideClientBase* pClient) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(pClient));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CPlayerPawnComponent* component) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(component));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CBasePlayerController* controller) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(controller));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CBasePlayerPawn* pawn) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(pawn));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CPlayerSlot slot) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(slot));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CEntityIndex entIndex) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(entIndex));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CPlayerUserId userID) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(userID));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CSteamID steamid, bool validate) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(steamid, validate));
}

void CSurfPlayer::Init(int iSlot) {
	CMovementPlayer::Init(iSlot);

	InitService(m_pTimerService);
	InitService(m_pZoneService);
}

void CSurfPlayer::EnableGodMode() {
	CCSPlayerPawn* pawn = this->GetPlayerPawn();
	if (!pawn) {
		return;
	}
	if (pawn->m_bTakesDamage()) {
		pawn->m_bTakesDamage(false);
	}
}

void CSurfPlayer::HideLegs() {
	CCSPlayerPawn* pawn = this->GetPlayerPawn();
	if (!pawn) {
		return;
	}

	Color& ogColor = pawn->m_clrRender();
	if (this->m_bHideLegs && ogColor.a() == 255) {
		pawn->m_clrRender(Color(255, 255, 255, 254));
	} else if (!this->m_bHideLegs && ogColor.a() != 255) {
		pawn->m_clrRender(Color(255, 255, 255, 255));
	}
}
