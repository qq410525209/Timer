#include "surf_hud.h"
#include <surf/timer/surf_timer.h>
#include <utils/print.h>

void CSurfHudService::OnPhysicsSimulatePost() {
	CSurfPlayer* player = this->GetPlayer();

	CUtlString sTime = SURF::FormatTime(player->m_pTimerService->m_fCurrentTime);
	auto timerController = player->GetController();
	const char* name = timerController->GetPlayerName();
	UTIL::PrintAlert(timerController, "name: %s, time: %s", name, sTime.Get());
}
