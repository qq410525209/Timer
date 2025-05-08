#include <surf/replay/surf_replay.h>
#include <surf/timer/surf_timer.h>
#include <utils/utils.h>

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

void CSurfReplayService::OnTimerFinishPost_SaveRecording() {
	auto pPlayer = GetPlayer();
	if (pPlayer->IsPracticeMode() || m_iCurrentFrame == 0) {
		return;
	}

	m_iTrackFinishFrame = m_iCurrentFrame;

	auto fTrackPostRunTime = SURF::ReplayPlugin()->m_cvarTrackPostRunTime->GetFloat();
	if (fTrackPostRunTime > 0.0f) {
		m_bGrabbingTrackPostFrame = true;

		m_hPostFrameTimer.Close();
		m_hPostFrameTimer = UTIL::CreateTimer(
			fTrackPostRunTime,
			[](CEntityHandle hController) {
				auto pController = (CCSPlayerController*)hController.Get();
				if (!pController || !pController->IsController()) {
					return -1.0;
				}

				auto pPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
				if (!pPlayer) {
					SDK_ASSERT(false);
					return -1.0;
				}

				pPlayer->m_pReplayService->SaveRecord();

				return -1.0;
			},
			pPlayer->GetController()->GetRefEHandle());
	} else {
		SaveRecord();
	}
}

void CSurfReplayService::FinishGrabbingTrackPostFrames() {
	m_bGrabbingTrackPostFrame = false;
	m_hPostFrameTimer.Close();
	SaveRecord();
}

void CSurfReplayService::StartRecord() {
	m_bEnabled = true;
}

void CSurfReplayService::DoRecord(CCSPlayerPawn* pawn, const CPlayerButton& buttons, const QAngle& viewAngles) {
	replay_frame_t frame;
	frame.ang = viewAngles;
	frame.pos = pawn->GetAbsOrigin();
	frame.buttons = buttons;
	frame.flags = pawn->m_fFlags();
	frame.mt = pawn->m_MoveType();

	m_vCurrentFrames.emplace_back(frame);
}

void CSurfReplayService::SaveRecord() {
	auto pPlayer = GetPlayer();
	auto pPlugin = SURF::ReplayPlugin();
	bool bStageTimer = pPlayer->IsStageTimer();

	replay_run_info_t info {};
	info.timestamp = std::time(nullptr);
	info.steamid = pPlayer->GetSteamId64();
	info.time = pPlayer->GetCurrentTime();
	info.style = pPlayer->GetStyle();
	info.track = pPlayer->GetCurrentTrack();
	info.stage = bStageTimer ? pPlayer->GetCurrentStage() : 0;
	info.preframes = bStageTimer ? m_iStagePrerunFrame : m_iTrackPrerunFrame;

	info.postframes = m_iCurrentFrame - bStageTimer ? m_iStageFinishFrame : m_iTrackFinishFrame;
	info.framelength = m_iCurrentFrame - info.preframes;

	ReplayArray_t arrFrames;
	bool bStageReplay = info.stage == 0;
	if (bStageReplay) {
		// FIXME: stage preframes
		arrFrames = UTIL::VECTOR::Slice(m_vCurrentFrames, 114514, m_iCurrentFrame);
	} else {
		arrFrames = m_vCurrentFrames;
	}

	pPlugin->AsyncWriteReplayFile(info, arrFrames);

	ClearFrames();
}

void CSurfReplayService::ClearFrames() {
	m_bEnabled = false;
	m_vCurrentFrames.clear();
	m_iCurrentFrame = 0;
	m_iTrackPrerunFrame = 0;
	m_iStagePrerunFrame = 0;
	m_iTrackFinishFrame = 0;
	m_iStageFinishFrame = 0;
}
