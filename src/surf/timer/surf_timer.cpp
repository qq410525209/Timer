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
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pPlayer) {
		return;
	}

	auto& pTimerService = pPlayer->m_pTimerService;
	if (pPlayer->IsAlive() && pTimerService->m_bTimerRunning && !pTimerService->m_bPaused) {
		pTimerService->m_fCurrentTime += ENGINE_FIXED_TICK_INTERVAL;
	}
}

bool CSurfTimerPlugin::OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == ZoneType::Zone_End) {
		pPlayer->m_pTimerService->DoTimerFinish();
	}

	return true;
}

bool CSurfTimerPlugin::OnStayZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == ZoneType::Zone_Start) {
		pPlayer->m_pTimerService->DoTimerStart();
	}

	return true;
}

bool CSurfTimerPlugin::OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == ZoneType::Zone_Start) {
		pPlayer->m_pTimerService->PlayStartTimerSound();
	}

	return true;
}

void CSurfTimerService::DoTimerStart() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerStart, pSurfPlayer);

	this->m_fCurrentTime = 0.0f;
	this->m_bTimerRunning = true;
	this->m_iCurrentStage = 0;
}

bool CSurfTimerService::DoTimerStop() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return false;
	}

	FORWARD_PRE(CSurfForward, OnTimerStop, false, pSurfPlayer);

	this->m_fCurrentTime = 0.0f;

	FORWARD_POST(CSurfForward, OnTimerStopPost, pSurfPlayer);

	return true;
}

void CSurfTimerService::DoTimerFinish() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	if (!pSurfPlayer->IsAlive()) {
		return;
	}

	if (!m_bTimerRunning) {
		PlayErrorSound();
		m_fLastFalseEndTime = UTIL::GetServerGlobals()->curtime;
		return;
	}

	m_fCurrentTime += ENGINE_FIXED_TICK_INTERVAL;

	FORWARD_PRE_void(CSurfForward, OnTimerFinish, pSurfPlayer);

	if (!DoTimerStop()) {
		return;
	}

	m_bTimerRunning = false;
	m_fLastEndTime = UTIL::GetServerGlobals()->curtime;
	PlayFinishTimerSound(m_iTrack);

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

void CSurfTimerService::PlayStartTimerSound() {
	UTIL::PlaySoundToClient(GetPlayer()->GetPlayerSlot(), SURF_SND_TIMER_START, 3.0f);
}

void CSurfTimerService::PlayFinishTimerSound(ZoneTrack iTrack) {
	switch (iTrack) {
		case ZoneTrack::Track_Main: {
			UTIL::PlaySoundToClient(GetPlayer()->GetPlayerSlot(), SURF_SND_TIMER_FINISH_MAIN);
			break;
		}
		case ZoneTrack::Track_Bonus: {
			UTIL::PlaySoundToClient(GetPlayer()->GetPlayerSlot(), SURF_SND_TIMER_FINISH_BONUS);
			break;
		}
	}
}
