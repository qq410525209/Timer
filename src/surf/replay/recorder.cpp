#include <surf/replay/surf_replay.h>

void CSurfReplayService::StartRecord() {
	m_bEnabled = true;
}

void CSurfReplayService::DoRecord(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const QAngle& viewAngles) {
	ReplayFrame_t frame;
	frame.ang = viewAngles;
	GetPlayer()->GetOrigin(frame.pos);
	frame.buttons = buttons->down;
	frame.flags = pawn->m_fFlags();
	frame.mt = pawn->m_MoveType();

	m_vReplayFrames.emplace_back(frame);
}

void CSurfReplayService::SaveRecord() {
	GetReplayPlugin()->m_umTrackReplays[0] = m_vReplayFrames;
	m_vReplayFrames.clear();
}
