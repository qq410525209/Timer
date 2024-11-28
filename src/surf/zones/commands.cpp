#include <core/concmdmanager.h>
#include "surf_zones.h"

CCMD_CALLBACK(Command_Zones) {}

CCMD_CALLBACK(Command_Drawme) {
	auto pawn = pController->GetPlayerPawn();
	auto pos = pawn->GetAbsOrigin();
	auto mins = pawn->m_pCollision()->m_vecMins() + pos;
	auto maxs = pawn->m_pCollision()->m_vecMaxs() + pos;

	Vector points[8];
	SurfZonePlugin()->CreateZonePoints(mins, maxs, points);
	SurfZonePlugin()->DrawZone(points);
}

void RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_drawme", Command_Drawme);
}
