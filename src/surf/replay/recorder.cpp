#include <surf/replay/surf_replay.h>
#include <surf/timer/surf_timer.h>

void CSurfReplayService::OnStart_Recording() {
	auto pPlayer = GetPlayer();

	m_bEnabled = true;

	if (m_bGrabbingStagePostFrame && pPlayer->GetCurrentStage() == 1) {
		// FinishGrabbingPostFrames_Stage
	}

	if (m_bGrabbingTrackPostFrame) {
		FinishGrabbingTrackPostFrames();
	}

	i32 iMaxPreFrames = std::floor(SURF::ReplayPlugin()->m_cvarTrackPreRunTime->GetFloat() * SURF_TICKRATE);
	i32 iFrameDifference = m_iCurrentFrame - iMaxPreFrames;
	if (iFrameDifference > 0) {
		if (iFrameDifference > 100) {
			for (i32 i = iFrameDifference; i < m_iCurrentFrame; i++) {
				i32 iSwapFrame = i - iFrameDifference;
				std::swap(m_vCurrentFrames.at(i), m_vCurrentFrames.at(iSwapFrame));
			}

			m_iCurrentFrame = iMaxPreFrames;
		} else {
			while (iFrameDifference--) {
				m_vCurrentFrames.erase(m_vCurrentFrames.begin());
				m_iCurrentFrame--;
			}
		}
	}

	m_iTrackPrerunFrame = m_iCurrentFrame;
}

void CSurfReplayService::FinishGrabbingTrackPostFrames() {
	m_bGrabbingTrackPostFrame = false;
}

void CSurfReplayService::StartRecord() {
	m_bEnabled = true;
}

void CSurfReplayService::DoRecord(CCSPlayerPawn* pawn, const CPlayerButton& buttons, const QAngle& viewAngles) {
	ReplayFrame_t frame;
	frame.ang = viewAngles;
	frame.pos = pawn->GetAbsOrigin();
	frame.buttons = buttons;
	frame.flags = pawn->m_fFlags();
	frame.mt = pawn->m_MoveType();

	m_vCurrentFrames.emplace_back(frame);
}

void CSurfReplayService::SaveRecord() {
	auto pPlugin = SURF::ReplayPlugin();
	auto& pTimerService = GetPlayer()->m_pTimerService;
	pPlugin->m_aTrackReplays[pTimerService->m_iCurrentTrack] = m_vCurrentFrames;
}

void CSurfReplayService::ClearFrames() {
	m_bEnabled = false;
	m_vCurrentFrames.clear();
	m_iCurrentFrame = 0;
	m_iFinishFrame = 0;
	m_iTrackPrerunFrame = 0;
	m_iStagePrerunFrame = 0;
}
