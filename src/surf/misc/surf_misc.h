#pragma once

#include <pch.h>
#include <movement/movement.h>

class CSurfMisc : CCoreForward, CMovementForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnActivateServer(CNetworkGameServerBase* pGameServer) override;
	virtual void OnWeaponDropPost(CCSPlayer_WeaponServices* pService, CBasePlayerWeapon* pWeapon, const int& iDropType,
								  const Vector* targetPos) override;
	virtual bool OnProcessMovement(CCSPlayer_MovementServices* ms, CMoveData* mv) override;
};
