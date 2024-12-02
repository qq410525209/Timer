#include "surf_timer.h"
#include <core/concmdmanager.h>
#include <utils/utils.h>
#include <surf/api.h>

CCMD_CALLBACK(Command_StartTimer) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pTimerService = player->m_pTimerService;
	pTimerService->DoTimerStart();
}

CCMD_CALLBACK(Command_EndTimer) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pTimerService = player->m_pTimerService;
	pTimerService->DoTimerEnd();
}

CSurfTimerPlugin g_SurfTimerPlugin;

void CSurfTimerPlugin::OnPluginStart() {
	CONCMD::RegConsoleCmd("sm_r", Command_StartTimer);
	CONCMD::RegConsoleCmd("sm_end", Command_EndTimer);
}

void CSurfTimerPlugin::OnPhysicsSimulatePost(CCSPlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pTimerService = player->m_pTimerService;
	if (player->IsAlive() && pTimerService->m_bTimerRunning && !pTimerService->m_bPaused) {
		pTimerService->m_fCurrentTime += ENGINE_FIXED_TICK_INTERVAL;
	}
}

bool CSurfTimerPlugin::OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* player) {
	if (zone.m_iType == ZoneType::Zone_End) {
		player->m_pTimerService->DoTimerEnd();
	}

	return true;
}

bool CSurfTimerPlugin::OnStayZone(const ZoneCache_t& zone, CSurfPlayer* player) {
	if (zone.m_iType == ZoneType::Zone_Start) {
		player->m_pTimerService->DoTimerStart();
	}

	return true;
}

void CSurfTimerService::DoTimerStart(bool playSound) {
	for (auto p = CSurfForward::m_pFirst; p; p = p->m_pNext) {
		p->OnTimerStart(this->GetPlayer());
	};

	this->m_fCurrentTime = 0.0f;
	this->m_bTimerRunning = true;
	this->m_iCurrentStage = 0;

	if (playSound) {
		// this->PlayTimerStartSound();
	}
}

void CSurfTimerService::DoTimerEnd() {
	auto player = this->GetPlayer();
	if (!player->IsAlive()) {
		return;
	}

	if (!this->m_bTimerRunning) {
		// this->PlayTimerFalseEndSound();
		this->m_fLastFalseEndTime = UTIL::GetServerGlobals()->curtime;
		return;
	}

	f64 time = this->m_fCurrentTime + UTIL::GetServerGlobals()->frametime;

	for (auto p = CSurfForward::m_pFirst; p; p = p->m_pNext) {
		p->OnTimerEnd(this->GetPlayer());
	};

	// Update current time for one last time.
	this->m_fCurrentTime = time;

	this->m_bTimerRunning = false;
	this->m_fLastEndTime = UTIL::GetServerGlobals()->curtime;
	// this->PlayTimerEndSound();
}
