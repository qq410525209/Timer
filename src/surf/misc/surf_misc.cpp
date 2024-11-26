#include "surf_misc.h"
#include <surf/surf_player.h>
#include <core/interfaces.h>
#include <core/logger.h>
#include <core/eventmanager.h>
#include <core/patch.h>
#include <utils/utils.h>

CSurfMisc g_SurfMisc;

static void OnPlayerDeath(const char* szName, bool bDontBroadcast) {}

static void OnRoundPrestart(const char* szName, bool bDontBroadcast) {}

static void OnRoundStart(const char* szName, bool bDontBroadcast) {
	IFACE::pEngine->ServerCommand("sv_full_alltalk 1");

	CCSGameRules* gameRules = UTIL::GetGameRules();
	if (gameRules) {
		gameRules->m_bGameRestart(true);
		gameRules->m_iRoundWinStatus(1);
		// Make sure that the round time is synchronized with the global time.
		gameRules->m_fRoundStartTime().SetTime(0.0f);
		gameRules->m_flGameStartTime().SetTime(0.0f);
	}
}

static void OnPlayerTeam(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {
	pEvent->SetBool("silent", true);
}

static void OnPlayerSpawm(const char* szName, bool bDontBroadcast) {}

void CSurfMisc::OnPluginStart() {
	EVENT::HookEvent("player_death", ::OnPlayerDeath);
	EVENT::HookEvent("round_prestart", ::OnRoundPrestart);
	EVENT::HookEvent("round_start", ::OnRoundStart);
	EVENT::HookEvent("player_team", ::OnPlayerTeam);
	EVENT::HookEvent("player_spawn", ::OnPlayerSpawm);

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

	return true;
}
