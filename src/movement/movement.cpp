#include "movement.h"

template<>
CMovementForward* CForwardBase<CMovementForward>::m_pFirst = nullptr;

CMovementPlayerManager* MOVEMENT::GetPlayerManager() {
	return static_cast<CMovementPlayerManager*>(::GetPlayerManager());
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CServerSideClientBase* pClient) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(pClient));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CPlayerPawnComponent* component) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(component));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CBasePlayerController* controller) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(controller));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CBasePlayerPawn* pawn) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(pawn));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CPlayerSlot slot) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(slot));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CEntityIndex entIndex) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(entIndex));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CPlayerUserId userID) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(userID));
}

CMovementPlayer* CMovementPlayerManager::ToPlayer(CSteamID steamid, bool validate) const {
	return static_cast<CMovementPlayer*>(CPlayerManager::ToPlayer(steamid, validate));
}

void CMovementPlayer::Reset() {
	CPlayer::Reset();
	this->landingVelocity.Init();
	this->jumped = false;
}

CCSPlayer_MovementServices* CMovementPlayer::GetMoveServices() {
	if (!this->GetPlayerPawn()) {
		return nullptr;
	}
	return static_cast<CCSPlayer_MovementServices*>(this->GetPlayerPawn()->m_pMovementServices());
}

void CMovementPlayer::GetBBoxBounds(bbox_t* bounds) {
	bounds->mins = {-16.0f, -16.0f, 0.0f};
	bounds->maxs = {16.0f, 16.0f, 72.0f};
	if (this->GetMoveServices() && this->GetMoveServices()->m_bDucked()) {
		bounds->maxs.z = 54.0f;
	}
}

void CMovementPlayer::RegisterTakeoff(bool jumped) {
	this->jumped = jumped;
}

void CMovementPlayer::RegisterLanding(const Vector& landingVelocity, bool distbugFix) {
	this->landingVelocity = landingVelocity;
}
