#pragma once

#include <pch.h>

class CSurfMisc : CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnActivateServer(CNetworkGameServerBase* pGameServer) override;
	virtual void OnWeaponDropPost(CCSPlayer_WeaponServices* pService, CBasePlayerWeapon* pWeapon, const int& iDropType,
								  const Vector* targetPos) override;
};
