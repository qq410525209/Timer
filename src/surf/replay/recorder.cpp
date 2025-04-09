#include <surf/replay/surf_replay.h>
#include <surf/timer/surf_timer.h>

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

	m_vReplayFrames.emplace_back(frame);
}

void CSurfReplayService::SaveRecord() {
	auto plugin = SURF::ReplayPlugin();
	auto& pTimerService = GetPlayer()->m_pTimerService;
	plugin->m_aTrackReplays[pTimerService->m_iCurrentTrack] = m_vReplayFrames;
	m_vReplayFrames.clear();
	m_bEnabled = false;
}
