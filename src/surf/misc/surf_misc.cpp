#include "surf_misc.h"
#include <surf/surf_player.h>
#include <core/interfaces.h>
#include <core/logger.h>
#include <core/cvarmanager.h>
#include <core/concmdmanager.h>
#include <utils/utils.h>

CSurfMiscPlugin g_SurfMisc;

CSurfMiscPlugin* SURF::MiscPlugin() {
	return &g_SurfMisc;
}

void CSurfMiscPlugin::OnPluginStart() {
	TweakCvars();
	HookEvents();
	RegisterCommands();
}

void CSurfMiscPlugin::TweakCvars() {
	static constexpr const char* tweakCvars[] = {"sv_infinite_ammo", "bot_stop"};
	static constexpr const char* tweakCmds[] = {"sv_debug_overlays_broadcast"};

	for (size_t i = 0; i < ARRAYSIZE(tweakCvars); i++) {
		auto cvar = CVAR::Find(tweakCvars[i]);
		if (cvar) {
			cvar->RemoveFlags(FCVAR_CHEAT);
		} else {
			LOG::Warning("Warning: %s is not found!\n", tweakCvars[i]);
		}
	}

	for (size_t i = 0; i < ARRAYSIZE(tweakCmds); i++) {
		auto pConCommand = CONCMD::Find(tweakCmds[i]);
		if (pConCommand) {
			pConCommand->RemoveFlags(FCVAR_CHEAT);
		} else {
			LOG::Warning("Warning: %s is not found!\n", tweakCvars[i]);
		}
	}
}

void CSurfMiscPlugin::OnActivateServer(CNetworkGameServerBase* pGameServer) {
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

void CSurfMiscPlugin::OnClientDisconnect(ISource2GameClients* pClient, CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName,
										 uint64 xuid, const char* pszNetworkID) {
	auto pController = dynamic_cast<CCSPlayerController*>(UTIL::GetController(slot));
	if (!pController) {
		return;
	}

	// Immediately remove the player off the list. We don't need to keep them around.
	pController->m_LastTimePlayerWasDisconnectedForPawnsRemove().SetTime(0.01f);
	pController->SwitchTeam(0);
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
