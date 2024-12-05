#include <core/concmdmanager.h>
#include "surf_zones.h"
#include <utils/utils.h>

CCMD_CALLBACK(Command_Zones) {}

CCMD_CALLBACK(Command_EditZone) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pZoneService = player->m_pZoneService;
	pZoneService->m_ZoneEdit.StartEditZone();
}

CCMD_CALLBACK(Command_TPStart) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	player->GetPlayerPawn()->Teleport(&SurfZonePlugin()->m_vecTestStartZone, nullptr, nullptr);
}

void RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_editzone", Command_EditZone);
	CONCMD::RegConsoleCmd("sm_r", Command_TPStart);
}
