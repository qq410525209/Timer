#include "surf_timer.h"
#include <surf/misc/surf_misc.h>
#include <core/concmdmanager.h>

CCMD_CALLBACK(Command_StartTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		pPlayer->PrintWarning("非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	auto& pZoneService = pPlayer->m_pZoneService;

	auto track = pTimerService->m_iCurrentTrack;
	if (!pZoneService->TeleportToZone(track, EZoneType::Zone_Start)) {
		pTimerService->PrintWarning("您的计时器将不会启动，因为地图未设置{orchid}%s{default}起点区域.", SURF::GetTrackName(track).c_str());
		pZoneService->PlayErrorSound();
		return;
	}
}

CCMD_CALLBACK(Command_StopTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		pPlayer->PrintWarning("非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	pTimerService->DoTimerStop();
}

CCMD_CALLBACK(Command_EndTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		pPlayer->PrintWarning("非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	auto& pZoneService = pPlayer->m_pZoneService;

	auto track = pTimerService->m_iCurrentTrack;
	auto type = EZoneType::Zone_End;

	const auto vEndZones = SURF::ZonePlugin()->GetZones(track, type);
	if (vEndZones.empty()) {
		pTimerService->PrintWarning("地图未设置{orchid}%s{default}终点区域!", SURF::GetTrackName(track).c_str());
		pZoneService->PlayErrorSound();
		return;
	}

	pTimerService->DoTimerStop();
	pZoneService->TeleportToZone(track, type);
}

CCMD_CALLBACK(Command_PauseTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		pPlayer->PrintWarning("非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	if (pTimerService->m_bPaused) {
		pTimerService->PrintWarning("已处于暂停状态!");
		pTimerService->PlayErrorSound();
		return;
	}

	pTimerService->DoTimerPause();
	pTimerService->Print("计时已暂停");
}

CCMD_CALLBACK(Command_ResumeTimer) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		pPlayer->PrintWarning("非存活状态!");
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	if (!pTimerService->m_bPaused) {
		pTimerService->PrintWarning("非暂停状态!");
		pTimerService->PlayErrorSound();
		return;
	}

	pTimerService->DoTimerResume();
	pTimerService->Print("计时已恢复");
}

void CSurfTimerPlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_r", Command_StartTimer);
	CONCMD::RegConsoleCmd("sm_stop", Command_StopTimer);
	CONCMD::RegConsoleCmd("sm_end", Command_EndTimer);
	CONCMD::RegConsoleCmd("sm_pause", Command_PauseTimer);
	CONCMD::RegConsoleCmd("sm_unpause", Command_ResumeTimer);
	CONCMD::RegConsoleCmd("sm_resume", Command_ResumeTimer);
}
