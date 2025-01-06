#include "surf_misc.h"
#include <surf/surf_player.h>
#include <core/interfaces.h>
#include <core/logger.h>
#include <utils/utils.h>

CSurfMiscPlugin g_SurfMisc;

CSurfMiscPlugin* SurfMiscPlugin() {
	return &g_SurfMisc;
}

extern void HookEvents();
extern void RegisterCommands();

void CSurfMiscPlugin::OnPluginStart() {
	HookEvents();
	RegisterCommands();
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

	m_vTriggers.clear();

	// Restart round to ensure settings (e.g. mp_weapons_allow_map_placed) are applied
	IFACE::pEngine->ServerCommand("mp_restartgame 1");
}

void CSurfMiscPlugin::OnWeaponDropPost(CCSPlayer_WeaponServices* pService, CBasePlayerWeapon* pWeapon, const int& iDropType,
									   const Vector* targetPos) {
	pWeapon->AcceptInput("kill");
}

void CSurfMiscPlugin::OnEntitySpawned(CEntityInstance* pEntity) {
	const char* sClassname = pEntity->GetClassname();
	if (V_strstr(sClassname, "trigger_")) {
		m_vTriggers.emplace_back(pEntity->GetRefEHandle());
	}
}

bool CSurfMiscPlugin::OnProcessMovement(CCSPlayer_MovementServices* ms, CMoveData* mv) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(ms);
	if (!player) {
		return true;
	}

	// legacy csgo duck fix
	ms->m_flDuckSpeed(CS_PLAYER_DUCK_SPEED_IDEAL);

	auto& pMiscService = player->m_pMiscService;
	pMiscService->HideLegs();

	return true;
}

bool CSurfMiscPlugin::OnTakeDamage(CCSPlayerPawn* pVictim, CTakeDamageInfo* info) {
	return false;
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
