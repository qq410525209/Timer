#include "surf_replay.h"
#include <utils/print.h>
#include <core/concmdmanager.h>
#include <core/sdkhook.h>

CSurfReplayPlugin g_SurfReplay;

CSurfReplayPlugin* SURF::ReplayPlugin() {
	return &g_SurfReplay;
}

void CSurfReplayPlugin::OnPluginStart() {}

void CSurfReplayPlugin::OnEntitySpawned(CEntityInstance* pEntity) {
	const char* sClassname = pEntity->GetClassname();
	if (V_strstr(sClassname, "trigger_") || V_strstr(sClassname, "_door")) {
		SDKHOOK::HookEntity<SDKHookType::SDKHook_StartTouch>((CBaseEntity*)pEntity, SURF::REPLAY::HOOK::HookBotTrigger);
		SDKHOOK::HookEntity<SDKHookType::SDKHook_EndTouch>((CBaseEntity*)pEntity, SURF::REPLAY::HOOK::HookBotTrigger);
		SDKHOOK::HookEntity<SDKHookType::SDKHook_Touch>((CBaseEntity*)pEntity, SURF::REPLAY::HOOK::HookBotTrigger);
		// SDKHOOK::HookEntity<SDKHookType::SDKHook_Use>((CBaseEntity*)pEntity, SURF::REPLAY::HOOK::HookBotTrigger);
	}
}

bool CSurfReplayPlugin::OnPlayerRunCmd(CCSPlayerPawn* pPawn, CInButtonState& buttons, float (&vec)[3], QAngle& viewAngles, int& weapon, int& cmdnum, int& tickcount, int& seed, int (&mouse)[2]) {
	if (pPawn->IsBot()) {
		CSurfBot* pBot = SURF::GetBotManager()->ToPlayer(pPawn);
		if (pBot) {
			pBot->m_pReplayService->DoPlayback(pPawn, buttons);
		}
	}

	return true;
}

void CSurfReplayPlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pPawn);
	if (!player) {
		return;
	}

	auto& pReplayService = player->m_pReplayService;
	if (pReplayService->m_bEnabled) {
		pReplayService->DoRecord(pPawn, buttons, viewAngles);
	}
}

bool CSurfReplayPlugin::OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == ZoneType::Zone_End) {
		pPlayer->m_pReplayService->SaveRecord();
	}

	return true;
}

bool CSurfReplayPlugin::OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == ZoneType::Zone_Start) {
		pPlayer->m_pReplayService->StartRecord();
	}

	return true;
}

void CSurfReplayService::OnReset() {
	m_bEnabled = false;
	m_vReplayFrames.clear();
}
