#include "surf_hud.h"
#include <surf/timer/surf_timer.h>
#include <utils/print.h>
#include <utils/utils.h>
#include <core/eventmanager.h>
#include <fmt/format.h>

CSurfHudPlugin g_SurfHudPlugin;

CSurfHudPlugin* SURF::HudPlugin() {
	return &g_SurfHudPlugin;
}

void CSurfHudPlugin::OnPlayerSpawn(IGameEvent* pEvent, const char* szName, bool bDontBroadcast) {
	auto pController = (CCSPlayerController*)pEvent->GetPlayerController("userid");
	if (!pController) {
		return;
	}

	CSurfPlayer* pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pSurfPlayer) {
		return;
	}

	auto& pSpeedText = pSurfPlayer->m_pHudService->m_pSpeedText;
	if (!pSpeedText) {
		pSpeedText = VGUI::CreateScreenText();
		pSpeedText->SetPos(-1.0f, -4.0f);
		pSpeedText->SetFontSize(40.0f);
	}

	CHandle<CCSPlayerController> hController = pController->GetRefEHandle();
	UTIL::RequestFrame([hController, pSpeedText]() {
		CCSPlayerController* pController = hController.Get();
		if (!pController) {
			return;
		}

		if (!pController->m_bPawnIsAlive()) {
			return;
		}

		CBasePlayerPawn* pPawn = pController->GetCurrentPawn();
		if (!pPawn || !pPawn->IsAlive()) {
			return;
		}

		VGUI::Render(pController, pSpeedText);
	});
}

void CSurfHudPlugin::OnPluginStart() {
	EVENT::HookEvent("player_spawn", OnPlayerSpawn);
}

void CSurfHudPlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pPawn);
	if (!pSurfPlayer) {
		return;
	}

	if (UTIL::GetGlobals()->tickcount % 2 == 0) {
		return;
	}

	CUtlString sTime = SURF::FormatTime(pSurfPlayer->m_pTimerService->m_fCurrentTime);
	auto timerController = pSurfPlayer->GetController();
	if (!timerController) {
		return;
	}

	UTIL::PrintHTMLCenter(timerController, "time: %s", sTime.Get());

	Vector vel;
	pSurfPlayer->GetVelocity(vel);
	std::string sSpeed = fmt::format("vel: {}", std::roundf(vel.Length2D()));

	auto& pSpeedText = pSurfPlayer->m_pHudService->m_pSpeedText;
	pSpeedText->SetText(sSpeed);
}
