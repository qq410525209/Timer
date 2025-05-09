#include "surf_timer.h"
#include <utils/utils.h>
#include <surf/api.h>
#include <surf/misc/surf_misc.h>

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
	if (zone.m_iType == EZoneType::Zone_End) {
		pPlayer->m_pTimerService->DoTimerFinish();
	}

	return true;
}

bool CSurfTimerPlugin::OnStayZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == EZoneType::Zone_Start) {
		pPlayer->m_pTimerService->DoTimerStart();
	}

	return true;
}

bool CSurfTimerPlugin::OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
	if (zone.m_iType == EZoneType::Zone_Start) {
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
	PlayFinishTimerSound(m_iCurrentTrack);

	FORWARD_POST(CSurfForward, OnTimerFinishPost, pSurfPlayer);
}

void CSurfTimerService::DoTimerPause() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	auto pPawn = pSurfPlayer->GetPlayerPawn();
	if (!pPawn) {
		PrintWarning("未知错误: %s", FILE_LINE_STRING);
		return;
	}

	if (!pPawn->IsAlive()) {
		PrintWarning("未存活");
		return;
	}

	auto pMoveService = pSurfPlayer->GetMoveServices();
	if (!pMoveService) {
		PrintWarning("未知错误: %s", FILE_LINE_STRING);
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerPause, pSurfPlayer);

	m_bPaused = true;

	pSurfPlayer->GetOrigin(m_Pause.m_vecPos);
	pSurfPlayer->GetAngles(m_Pause.m_vecAng);
	pSurfPlayer->GetVelocity(m_Pause.m_vecVel);
	m_Pause.m_nMoveType = pPawn->m_MoveType();
	m_Pause.m_nActualMoveType = pPawn->m_nActualMoveType();
	m_Pause.m_fGravity = pPawn->m_flGravityScale();
	m_Pause.m_fSpeed = pPawn->m_flVelocityModifier();
	m_Pause.m_iFlags = pPawn->m_fFlags();
	m_Pause.m_hGroundEntity = pPawn->m_hGroundEntity();

	m_Pause.m_sTargetName = pPawn->m_pEntity->m_name.String();
	m_Pause.m_nMoveType = MOVETYPE_WALK;
	m_Pause.m_nActualMoveType = MOVETYPE_WALK;

	m_Pause.m_vecLadderNormal = pMoveService->m_vecLadderNormal();
	m_Pause.m_bDucked = pMoveService->m_bDucked();
	m_Pause.m_bDucking = pMoveService->m_bDucking();
	m_Pause.m_fDuckTime = pMoveService->m_flDuckAmount();
	m_Pause.m_fDuckSpeed = pMoveService->m_flDuckSpeed();
}

void CSurfTimerService::DoTimerResume() {
	CSurfPlayer* pSurfPlayer = this->GetPlayer();
	auto pPawn = pSurfPlayer->GetPlayerPawn();
	if (!pPawn) {
		PrintWarning("未知错误: %s", FILE_LINE_STRING);
		return;
	}

	if (!pPawn->IsAlive()) {
		PrintWarning("未存活");
		return;
	}

	auto pMoveService = pSurfPlayer->GetMoveServices();
	if (!pMoveService) {
		PrintWarning("未知错误: %s", FILE_LINE_STRING);
		return;
	}

	FORWARD_PRE_void(CSurfForward, OnTimerResume, pSurfPlayer);

	m_bPaused = false;

	pPawn->m_MoveType(m_Pause.m_nMoveType);
	pPawn->m_nActualMoveType(m_Pause.m_nActualMoveType);
	pPawn->m_flGravityScale(m_Pause.m_fGravity);
	pPawn->m_flVelocityModifier(m_Pause.m_fSpeed);
	pPawn->m_fFlags(m_Pause.m_iFlags);
	pPawn->m_hGroundEntity(m_Pause.m_hGroundEntity);

	pPawn->SetName(m_Pause.m_sTargetName.c_str(), true);

	pMoveService->m_vecLadderNormal(m_Pause.m_vecLadderNormal);
	pMoveService->m_bDucked(m_Pause.m_bDucked);
	pMoveService->m_bDucking(m_Pause.m_bDucking);
	pMoveService->m_flDuckAmount(m_Pause.m_fDuckTime);
	pMoveService->m_flDuckSpeed(m_Pause.m_fDuckSpeed);

	pSurfPlayer->Teleport(&m_Pause.m_vecPos, &m_Pause.m_vecAng, &m_Pause.m_vecVel);
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

void CSurfTimerService::PlayFinishTimerSound(TimerTrack iTrack) {
	switch (iTrack) {
		case EZoneTrack::Track_Main: {
			UTIL::PlaySoundToClient(GetPlayer()->GetPlayerSlot(), SURF_SND_TIMER_FINISH_MAIN);
			break;
		}
		case EZoneTrack::Track_Bonus: {
			UTIL::PlaySoundToClient(GetPlayer()->GetPlayerSlot(), SURF_SND_TIMER_FINISH_BONUS);
			break;
		}
	}
}
