#include <surf/replay/surf_replay.h>
#include <utils/utils.h>

void DoPlayback(CCSPlayerPawn* pBotPawn, CCSBot* pBot) {
	auto& aFrames = GetReplayPlugin()->m_umTrackReplays[0];
	auto iFrameSize = aFrames.size();
	if (iFrameSize == 0) {
		return;
	}

	auto slot = pBotPawn->GetController()->GetPlayerSlot();
	auto& currentTick = GetReplayPlugin()->m_iCurrentTick.at(slot);
	if (currentTick >= iFrameSize) {
		currentTick = 0;
	}

	auto& frame = aFrames.at(currentTick);
	pBotPawn->m_angEyeAngles(frame.ang);

	auto botFlags = pBotPawn->m_fFlags();
	pBotPawn->m_fFlags(botFlags ^= frame.flags);

	pBot->m_buttonFlags(frame.buttons);
	pBotPawn->SetMoveType(frame.mt);

	if ((GetReplayPlugin()->m_iLastReplayFlag.at(slot) & FL_ONGROUND) && !(botFlags & FL_ONGROUND)) {
		static auto fn = libmem::SignScan("48 81 C1 80 0F 00 00 E9 F4 56 01 00", LIB::server);
		MEM::SDKCall<void*>(fn, pBotPawn, PlayerAnimEvent_t::PLAYERANIMEVENT_JUMP, 0);
	}

	Vector& currentPos = pBotPawn->GetAbsOrigin();
	if (currentTick == 0 || currentPos.DistTo(frame.pos) > 15000.0) {
		static Vector zeroVel {0.0f, 0.0f, 0.0f};
		pBotPawn->SetMoveType(MoveType_t::MOVETYPE_NOCLIP);
		pBotPawn->Teleport(&frame.pos, nullptr, &zeroVel);
	} else {
		constexpr auto replay_tickrate = ENGINE_FIXED_TICK_RATE;
		Vector calculatedVelocity = (frame.pos - currentPos) * replay_tickrate;
		pBotPawn->Teleport(nullptr, nullptr, &calculatedVelocity);
	}

	currentTick++;
	GetReplayPlugin()->m_iLastReplayFlag.at(slot) = botFlags;
}
