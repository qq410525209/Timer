#include "surf_misc.h"
#include <surf/surf_player.h>
#include <core/interfaces.h>
#include <core/logger.h>
#include <core/patch.h>
#include <utils/utils.h>

CSurfMiscPlugin g_SurfMisc;

extern void HookEvents();
extern void RegisterCommands();

void CSurfMiscPlugin::OnPluginStart() {
	HookEvents();
	RegisterCommands();

	PATCH::DoMovementUnlocker();
}

void CSurfMiscPlugin::OnActivateServer(CNetworkGameServerBase* pGameServer) {
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

void CSurfMiscPlugin::OnWeaponDropPost(CCSPlayer_WeaponServices* pService, CBasePlayerWeapon* pWeapon, const int& iDropType,
									   const Vector* targetPos) {
	pWeapon->AcceptInput("kill");
}

bool CSurfMiscPlugin::OnProcessMovement(CCSPlayer_MovementServices* ms, CMoveData* mv) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(ms);
	if (!player) {
		return true;
	}

	auto& pMiscService = player->m_pMiscService;
	pMiscService->EnableGodMode();
	pMiscService->HideLegs();

	return true;
}

void CSurfMiscService::EnableGodMode() {
	CCSPlayerPawn* pawn = this->GetPlayer()->GetPlayerPawn();
	if (!pawn) {
		return;
	}
	if (pawn->m_bTakesDamage()) {
		pawn->m_bTakesDamage(false);
	}
}

void CSurfMiscService::HideLegs() {
	CCSPlayerPawn* pawn = this->GetPlayer()->GetPlayerPawn();
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

void CSurfMiscService::Reset() {}
