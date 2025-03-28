#include "surf_timer.h"
#include <utils/utils.h>
#include <surf/api.h>

CSurfTimerPlugin g_SurfTimerPlugin;

CSurfTimerPlugin* SURF::TimerPlugin() {
	return &g_SurfTimerPlugin;
}

void CSurfTimerPlugin::OnPluginStart() {
	RegisterCommand();
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
		player->m_pTimerService->DoTimerStop();
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
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerStart, pSurfPlayer);

	this->m_fCurrentTime = 0.0f;
	this->m_bTimerRunning = true;
	this->m_iCurrentStage = 0;

	if (playSound) {
		// this->PlayTimerStartSound();
	}
}

void CSurfTimerService::DoTimerStop() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerStop, pSurfPlayer);

	this->m_fCurrentTime = 0.0f;

	FORWARD_POST(CSurfForward, OnTimerStopPost, pSurfPlayer);
}

void CSurfTimerService::DoTimerFinish() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	if (!this->m_bTimerRunning) {
		// this->PlayTimerFalseEndSound();
		this->m_fLastFalseEndTime = UTIL::GetServerGlobals()->curtime;
		return;
	}

	this->m_fCurrentTime += ENGINE_FIXED_TICK_INTERVAL;

	FORWARD_PRE_void(CSurfForward, OnTimerFinish, pSurfPlayer);

	this->m_bTimerRunning = false;
	this->m_fLastEndTime = UTIL::GetServerGlobals()->curtime;
	// this->PlayTimerEndSound();

	FORWARD_POST(CSurfForward, OnTimerFinishPost, pSurfPlayer);
}

void CSurfTimerService::DoTimerPause() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerPause, pSurfPlayer);

	m_bPaused = true;
}

void CSurfTimerService::DoTimerResume() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerResume, pSurfPlayer);

	m_bPaused = false;
}

void CSurfTimerService::BuildSnapshot(timer_snapshot_t& buffer) const {
	buffer = static_cast<const timer_snapshot_t&>(*this);
}

void CSurfTimerService::FromSnapshot(const timer_snapshot_t& snapshot) {
	static_cast<timer_snapshot_t&>(*this) = snapshot;
}

void SURF::FormatTime(f64 time, char* output, u32 length, bool precise) {
	int roundedTime = RoundFloatToInt(time * 1000); // Time rounded to number of ms

	int milliseconds = roundedTime % 1000;
	roundedTime = (roundedTime - milliseconds) / 1000;
	int seconds = roundedTime % 60;
	roundedTime = (roundedTime - seconds) / 60;
	int minutes = roundedTime % 60;
	roundedTime = (roundedTime - minutes) / 60;
	int hours = roundedTime;

	if (hours == 0) {
		if (precise) {
			snprintf(output, length, "%02i:%02i.%03i", minutes, seconds, milliseconds);
		} else {
			snprintf(output, length, "%i:%02i", minutes, seconds);
		}
	} else {
		if (precise) {
			snprintf(output, length, "%i:%02i:%02i.%03i", hours, minutes, seconds, milliseconds);
		} else {
			snprintf(output, length, "%i:%02i:%02i", hours, minutes, seconds);
		}
	}
}

CUtlString SURF::FormatTime(f64 time, bool precise) {
	char temp[32];
	FormatTime(time, temp, sizeof(temp), precise);
	return CUtlString(temp);
}

void SURF::FormatDiffTime(f64 time, char* output, u32 length, bool precise) {
	char temp[32];
	if (time > 0) {
		FormatTime(time, temp, sizeof(temp), precise);
		V_snprintf(output, length, "+%s", temp);
	} else {
		FormatTime(-time, temp, sizeof(temp), precise);
		V_snprintf(output, length, "-%s", temp);
	}
}

CUtlString SURF::FormatDiffTime(f64 time, bool precise) {
	char temp[32];
	FormatDiffTime(time, temp, sizeof(temp), precise);
	return CUtlString(temp);
}
