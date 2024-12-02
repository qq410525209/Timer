#include <surf/replay/surf_replay.h>

void DoPlayback(CCSPlayerPawn* pBotPawn, CCSBot* pBot) {
	auto& aFrames = GetReplayPlugin()->m_umTrackReplays[0];
	auto iFrameSize = aFrames.size();
	if (iFrameSize == 0) {
		return;
	}

	auto slot = pBotPawn->GetEntityIndex().Get() - 1;
	auto& currentTick = GetReplayPlugin()->m_iCurrentTick[slot];
	if (currentTick < iFrameSize) {
		auto& frame = aFrames.at(currentTick);
		pBot->m_buttonFlags(frame.buttons);
		pBotPawn->m_angEyeAngles(frame.ang);
		pBotPawn->Teleport(&frame.pos, nullptr, nullptr);
		pBotPawn->m_fFlags(frame.flags);
		pBotPawn->SetMoveType(frame.mt);

		currentTick++;
	} else {
		currentTick = 0;
	}
}
