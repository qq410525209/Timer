#pragma once

#include <core/eventmanager.h>
#include <core/interfaces.h>
#include <sdk/gamerules.h>
#include <utils/utils.h>

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

void HookEvents() {
	EVENT::HookEvent("player_death", ::OnPlayerDeath);
	EVENT::HookEvent("round_prestart", ::OnRoundPrestart);
	EVENT::HookEvent("round_start", ::OnRoundStart);
	EVENT::HookEvent("player_team", ::OnPlayerTeam);
	EVENT::HookEvent("player_spawn", ::OnPlayerSpawm);
}
