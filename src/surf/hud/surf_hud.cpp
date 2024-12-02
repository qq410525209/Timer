#include "surf_hud.h"
#include <surf/timer/surf_timer.h>
#include <utils/print.h>

CSurfHudPlugin g_SurfHudPlugin;

void CSurfHudPlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const float (&vec)[3], const QAngle& viewAngles,
										const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pawn);
	if (!player) {
		return;
	}

	CUtlString sTime = SURF::FormatTime(player->m_pTimerService->m_fCurrentTime);
	auto timerController = player->GetController();
	const char* name = timerController->GetPlayerName();
	UTIL::PrintAlert(timerController, "name: %s, time: %s", name, sTime.Get());
}
