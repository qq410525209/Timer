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
	pZoneService->m_bEditing = true;
	pZoneService->m_iEditStep = EditStep_None;
	trace_t tr;
	UTIL::GetPlayerAiming(pController->GetPlayerPawn(), tr);
	Vector& aimPos = tr.m_vEndPos;
	auto pBeam = UTIL::CreateBeam(tr.m_vEndPos, tr.m_vEndPos);
	pZoneService->m_vTestBeam.emplace_back(pBeam->GetRefEHandle());
}

void RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_editzone", Command_EditZone);
}
