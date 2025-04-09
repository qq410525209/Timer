#include "surf_replay.h"
#include <utils/utils.h>

void CSurfBotReplayService::OnInit() {
	Reset();
}

void CSurfBotReplayService::OnReset() {
	Reset();
}

void CSurfBotReplayService::Reset() {
	m_iCurrentTick = 0;
	m_iCurrentTrack = ZoneTrack::Track_Main;
	m_iCurrentStage = 0;
}

void CSurfBotReplayService::DoPlayback(CCSPlayerPawn* pBotPawn, CInButtonState& buttons) {
	auto& aFrames = SURF::ReplayPlugin()->m_aTrackReplays[m_iCurrentTrack];
	auto iFrameSize = aFrames.size();
	if (iFrameSize == 0) {
		return;
	}

	if (m_iCurrentTick >= iFrameSize) {
		m_iCurrentTick = 0;
	}

	auto& frame = aFrames.at(m_iCurrentTick);
	MEM::CALL::SnapViewAngles(pBotPawn, frame.ang);

	auto botFlags = pBotPawn->m_fFlags();
	pBotPawn->m_fFlags(botFlags ^= frame.flags);

	buttons = frame.buttons;
	pBotPawn->SetMoveType(frame.mt);

	const Vector& currentPos = pBotPawn->GetAbsOrigin();
	if (m_iCurrentTick == 0 || currentPos.DistTo(frame.pos) > 15000.0) {
		static Vector zeroVel {0.0f, 0.0f, 0.0f};
		pBotPawn->SetMoveType(MoveType_t::MOVETYPE_NOCLIP);
		pBotPawn->Teleport(&frame.pos, nullptr, &zeroVel);
	} else {
		constexpr auto replay_tickrate = ENGINE_FIXED_TICK_RATE;
		Vector calculatedVelocity = (frame.pos - currentPos) * replay_tickrate;
		pBotPawn->Teleport(nullptr, nullptr, &calculatedVelocity);
	}

	m_iCurrentTick++;
}
