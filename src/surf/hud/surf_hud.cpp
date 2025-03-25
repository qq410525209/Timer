#include "surf_hud.h"
#include <surf/timer/surf_timer.h>
#include <utils/print.h>
#include <utils/utils.h>

CSurfHudPlugin g_SurfHudPlugin;

CSurfHudPlugin* SURF::HudPlugin() {
	return &g_SurfHudPlugin;
}

void CSurfHudPlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pawn);
	if (!player) {
		return;
	}

	if (UTIL::GetGlobals()->tickcount % 2 == 0) {
		return;
	}

	CUtlString sTime = SURF::FormatTime(player->m_pTimerService->m_fCurrentTime);
	auto timerController = player->GetController();
	if (!timerController) {
		return;
	}

	const char* name = timerController->GetPlayerName();
	Vector vel;
	player->GetVelocity(vel);
	UTIL::PrintHTMLCenter(timerController, "time: %s, vel: %.3f", sTime.Get(), (float)vel.Length2D());
}
