#include <core/concmdmanager.h>
#include "surf_zones.h"

CCMD_CALLBACK(Command_Zones) {}

CCMD_CALLBACK(Command_Drawme) {
	auto player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto pawn = pController->GetPlayerPawn();
	auto pos = pawn->GetAbsOrigin();
	auto mins = pawn->m_pCollision()->m_vecMins() + pos;
	auto maxs = pawn->m_pCollision()->m_vecMaxs() + pos;

	Vector points[8];
	player->m_pZoneService->CreateZonePoints(mins, maxs, points);
	player->m_pZoneService->CreateZone(points, false, player->m_pZoneService->m_vecTestBeam);
	player->m_pZoneService->m_bCanDraw = true;
}

void RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_drawme", Command_Drawme);
}
