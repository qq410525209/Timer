#include "surf_misc.h"
#include <surf/surf_player.h>
#include <core/interfaces.h>
#include <core/logger.h>
#include <core/patch.h>
#include <utils/utils.h>

CSurfMisc g_SurfMisc;

extern void HookEvents();
extern void RegisterCommands();

void CSurfMisc::OnPluginStart() {
	HookEvents();
	RegisterCommands();

	PATCH::DoMovementUnlocker();
}

void CSurfMisc::OnActivateServer(CNetworkGameServerBase* pGameServer) {
	static bool cvTweaked {};
	if (!cvTweaked) {
		cvTweaked = true;
		auto cvarHandle = g_pCVar->FindConVar("sv_infinite_ammo");
		if (cvarHandle.IsValid()) {
			g_pCVar->GetConVar(cvarHandle)->flags &= ~FCVAR_CHEAT;
		} else {
			LOG::PrintWarning("Warning: sv_infinite_ammo is not found!\n");
		}
		cvarHandle = g_pCVar->FindConVar("bot_stop");
		if (cvarHandle.IsValid()) {
			g_pCVar->GetConVar(cvarHandle)->flags &= ~FCVAR_CHEAT;
		} else {
			LOG::PrintWarning("Warning: bot_stop is not found!\n");
		}
	}

	IFACE::pEngine->ServerCommand("exec cs2surf.cfg");

	// Restart round to ensure settings (e.g. mp_weapons_allow_map_placed) are applied
	IFACE::pEngine->ServerCommand("mp_restartgame 1");
}

void CSurfMisc::OnWeaponDropPost(CCSPlayer_WeaponServices* pService, CBasePlayerWeapon* pWeapon, const int& iDropType, const Vector* targetPos) {
	pWeapon->AcceptInput("kill");
}

bool CSurfMisc::OnProcessMovement(CCSPlayer_MovementServices* ms, CMoveData* mv) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(ms);
	if (!player) {
		return true;
	}

	player->EnableGodMode();
	player->HideLegs();

	return true;
}
