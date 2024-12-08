#pragma once

#include <pch.h>
#include <surf/surf_player.h>

class CSurfMiscPlugin : CCoreForward, CMovementForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnActivateServer(CNetworkGameServerBase* pGameServer) override;
	virtual void OnWeaponDropPost(CCSPlayer_WeaponServices* pService, CBasePlayerWeapon* pWeapon, const int& iDropType,
								  const Vector* targetPos) override;
	virtual bool OnProcessMovement(CCSPlayer_MovementServices* ms, CMoveData* mv) override;
	virtual bool OnTakeDamage(CCSPlayerPawn* pVictim, CTakeDamageInfo* info) override;
};

class CSurfMiscService : CSurfBaseService {
	using CSurfBaseService::CSurfBaseService;

	virtual void Reset() override;

public: // rampfix
	bool didTPM {};
	bool overrideTPM {};
	Vector tpmVelocity = vec3_invalid;
	Vector tpmOrigin = vec3_invalid;
	Vector lastValidPlane = vec3_origin;

public:
	bool m_bHideLegs = true;

public:
	void HideLegs();
};
