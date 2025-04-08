#include "surf_timer.h"
#include <surf/misc/surf_misc.h>
#include <core/concmdmanager.h>

CCMD_CALLBACK(Command_StartTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		SURF::PrintWarning(pPlayer, "非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	auto& pZoneService = pPlayer->m_pZoneService;

	auto track = pTimerService->m_iTrack;
	if (!pZoneService->TeleportToZone(track, ZoneType::Zone_Start)) {
		SURF::PrintWarning(pPlayer, "您的计时器将不会启动，因为地图未设置{orchid}%s{default}起点区域.", SURF::ZONE::GetZoneNameByTrack(track));
		return;
	}
}

CCMD_CALLBACK(Command_EndTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		SURF::PrintWarning(pPlayer, "非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	auto& pZoneService = pPlayer->m_pZoneService;

	auto track = pTimerService->m_iTrack;
	if (!pZoneService->TeleportToZone(track, ZoneType::Zone_End)) {
		SURF::PrintWarning(pPlayer, "您的计时器将不会启动，因为地图未设置{orchid}%s{default}终点区域.", SURF::ZONE::GetZoneNameByTrack(track));
		return;
	}
}

void CSurfTimerPlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_r", Command_StartTimer);
	CONCMD::RegConsoleCmd("sm_end", Command_EndTimer);
}
