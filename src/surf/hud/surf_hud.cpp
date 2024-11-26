#include "surf_hud.h"
#include <surf/timer/surf_timer.h>
#include <utils/print.h>

CSurfHudPlugin g_SurfHudPlugin;

void CSurfHudPlugin::OnPhysicsSimulatePost(CCSPlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	CUtlString sTime = SURF::FormatTime(player->m_pTimerService->m_fCurrentTime);
	auto timerController = player->GetController();
	const char* name = timerController->GetPlayerName();
	UTIL::PrintAlert(timerController, "name: %s, time: %s", name, sTime.Get());
}
