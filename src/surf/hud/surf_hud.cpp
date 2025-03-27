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
		pSpeedText->SetPos(-0.5f, 3.0f);
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

	auto pController = pSurfPlayer->GetController();
	if (!pController) {
		return;
	}

	auto& pTimerService = pSurfPlayer->m_pTimerService;
	auto& pHudService = pSurfPlayer->m_pHudService;

	CUtlString sTime = SURF::FormatTime(pTimerService->m_fCurrentTime);
	UTIL::PrintHTMLCenter(pController, "时间: %s", sTime.Get());

	Vector vVel;
	pSurfPlayer->GetVelocity(vVel);
	int iVel = std::round(vVel.Length2D());
	std::string sSpeed = fmt::format("{}", iVel);

	auto& pSpeedText = pHudService->m_pSpeedText;
	Color iSpeedColor = iVel < pHudService->m_iPrevSpeed ? Color(255, 0, 0, 255) : Color(0, 0, 255, 255);
	pSpeedText->SetColor(iSpeedColor);
	pSpeedText->SetText(sSpeed);

	pHudService->m_iPrevSpeed = iVel;
}

void CSurfHudService::OnReset() {
	if (m_pSpeedText) {
		m_pSpeedText.reset();
	}
}
