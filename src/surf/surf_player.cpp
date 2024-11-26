#include "surf_player.h"

CSurfPlayerManager g_SurfPlayerManager;

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

void CSurfPlayer::EnableGodMode() {
	CCSPlayerPawn* pawn = this->GetPlayerPawn();
	if (!pawn) {
		return;
	}
	if (pawn->m_bTakesDamage()) {
		pawn->m_bTakesDamage(false);
	}
}
