#include "surf_timer.h"
#include <core/concmdmanager.h>
#include <utils/utils.h>

CCMD_CALLBACK(Command_StartTimer) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& timerService = player->m_pTimerService;
	timerService->DoTimerStart();
}

CCMD_CALLBACK(Command_EndTimer) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& timerService = player->m_pTimerService;
	timerService->DoTimerEnd();
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

void CSurfTimerService::OnPluginStart() {
	CONCMD::RegConsoleCmd("sm_r", Command_StartTimer);
	CONCMD::RegConsoleCmd("sm_end", Command_EndTimer);
}

void CSurfTimerService::OnPhysicsSimulatePost(CCSPlayerController* pController) {
	if (this->GetPlayer()->IsAlive() && this->m_bTimerRunning && !this->m_bPaused) {
		this->m_fCurrentTime += ENGINE_FIXED_TICK_INTERVAL;
	}
}
