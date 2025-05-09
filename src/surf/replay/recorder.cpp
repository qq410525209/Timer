#include <surf/replay/surf_replay.h>
#include <surf/timer/surf_timer.h>
#include <utils/utils.h>

void CSurfReplayService::OnEnterStart_Recording() {
	m_bEnabled = true;

	auto pPlayer = GetPlayer();

	if (m_ExtraStageFrame.bGrabEnd && pPlayer->GetCurrentStage() == 1) {
		FinishGrabbingStagePostFrames();
	}

	if (m_ExtraTrackFrame.bGrabEnd) {
		FinishGrabbingTrackPostFrames();
	}
}

void CSurfReplayService::OnStart_Recording() {
	size_t iMaxPreFrames = std::floor(SURF::ReplayPlugin()->m_cvarTrackPreRunTime->GetFloat() * SURF_TICKRATE);
	size_t iFrameDifference = m_iCurrentFrame - iMaxPreFrames;
	if (iFrameDifference > 0) {
		if (iFrameDifference > 100) {
			// need figure out why diff > 100
			SDK_ASSERT(false);

			/*for (size_t i = iFrameDifference; i < m_iCurrentFrame; i++) {
				size_t iSwapFrame = i - iFrameDifference;
				std::swap(m_vCurrentFrames.at(i), m_vCurrentFrames.at(iSwapFrame));
			}

			m_iCurrentFrame = iMaxPreFrames;*/
		} else {
			m_vCurrentFrames.erase(m_vCurrentFrames.begin(), m_vCurrentFrames.begin() + iFrameDifference);

			m_iCurrentFrame = 0;
		}
	}

	m_ExtraTrackFrame.iPreEnd = m_iCurrentFrame;
}

void CSurfReplayService::OnTimerFinishPost_SaveRecording() {
	auto pPlayer = GetPlayer();
	if (pPlayer->IsPracticeMode() || m_iCurrentFrame == 0) {
		return;
	}

	m_ExtraTrackFrame.iEndStart = m_iCurrentFrame;

	auto fTrackPostRunTime = SURF::ReplayPlugin()->m_cvarTrackPostRunTime->GetFloat();
	if (fTrackPostRunTime > 0.0f) {
		m_ExtraTrackFrame.bGrabEnd = true;

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

void CSurfReplayService::FinishGrabbingStagePostFrames() {
	m_ExtraStageFrame.bGrabEnd = false;
	m_hPostFrameTimer.Close(); // should be stagepostframetimer
	SaveRecord();
}

void CSurfReplayService::FinishGrabbingTrackPostFrames() {
	m_ExtraTrackFrame.bGrabEnd = false;
	m_hPostFrameTimer.Close();
	SaveRecord();
}

void CSurfReplayService::StartRecord() {
	m_bEnabled = true;
}

void CSurfReplayService::DoRecord(CCSPlayerPawn* pawn, const CPlayerButton& buttons, const QAngle& viewAngles) {
	bool bCanRecord = m_bEnabled || m_ExtraStageFrame.bGrabEnd || m_ExtraTrackFrame.bGrabEnd;
	if (!bCanRecord) {
		return;
	}

	if (m_vCurrentFrames.size() <= m_iCurrentFrame) {
		size_t newSize = static_cast<size_t>(m_iCurrentFrame) * 2;
		m_vCurrentFrames.resize(newSize);
	}

	replay_frame_data_t frame;
	frame.ang = viewAngles;
	frame.pos = pawn->GetAbsOrigin();
	frame.buttons = buttons;
	frame.flags = pawn->m_fFlags();
	frame.mt = pawn->m_MoveType();

	m_vCurrentFrames[m_iCurrentFrame] = frame;
	m_iCurrentFrame++;
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
	info.framelength = m_iCurrentFrame - bStageTimer ? m_ExtraStageFrame.iPreStart : 0;

	ReplayArray_t arrFrames;
	bool bStageReplay = info.stage == 0;
	if (bStageReplay) {
		arrFrames = UTIL::VECTOR::Slice(m_vCurrentFrames, m_ExtraStageFrame.iPreStart, m_iCurrentFrame);
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
	m_ExtraStageFrame = {};
	m_ExtraTrackFrame = {};
}
