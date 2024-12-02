#include <surf/replay/surf_replay.h>

void DoPlayback(CCSPlayerPawn* pBotPawn, CCSBot* pBot) {
	auto& aFrames = GetReplayPlugin()->m_umTrackReplays[0];
	auto iFrameSize = aFrames.size();
	if (iFrameSize == 0) {
		return;
	}

	auto slot = pBotPawn->GetEntityIndex().Get() - 1;
	auto& currentTick = GetReplayPlugin()->m_iCurrentTick[slot];
	if (currentTick >= iFrameSize) {
		currentTick = 0;
	}

	auto& frame = aFrames.at(currentTick);
	pBot->m_buttonFlags(frame.buttons);
	pBotPawn->m_angEyeAngles(frame.ang);
	pBotPawn->m_fFlags(pBotPawn->m_fFlags() ^= frame.flags);
	pBotPawn->SetMoveType(frame.mt);

	Vector& currentPos = pBotPawn->GetOrigin();
	if (currentTick == 0 || currentPos.DistTo(frame.pos) > 15000.0) {
		static Vector zeroVel {0.0f, 0.0f, 0.0f};
		pBotPawn->SetMoveType(MoveType_t::MOVETYPE_NOCLIP);
		pBotPawn->Teleport(&frame.pos, nullptr, &zeroVel);
	} else {
		Vector calculatedVelocity = (frame.pos - currentPos) * ENGINE_FIXED_TICK_RATE;
		pBotPawn->Teleport(nullptr, nullptr, &calculatedVelocity);
	}

	currentTick++;
}
