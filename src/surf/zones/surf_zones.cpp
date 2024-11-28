#include "surf_zones.h"
#include <utils/print.h>
#include <utils/utils.h>

CSurfZonePlugin g_SurfZonePlugin;

CSurfZonePlugin* SurfZonePlugin() {
	return &g_SurfZonePlugin;
}

extern void RegisterCommand();

void CSurfZonePlugin::OnPluginStart() {
	RegisterCommand();
}

void CSurfZonePlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3],
										 const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pawn);
	if (!player) {
		return;
	}

	auto& pZoneService = player->m_pZoneService;
	this->DrawEditZone(pawn);

	auto iEditStep = pZoneService->m_iEditStep;
	if (iEditStep > EditStep_None && iEditStep < EditStep_Final) {
	}
}

void CSurfZonePlugin::DrawEditZone(CCSPlayerPawnBase* pawn) {
	trace_t tr;
	UTIL::GetPlayerAiming(pawn, tr);
	Vector& aimPos = tr.m_vEndPos;
}

void CSurfZonePlugin::CreateZonePoints(const Vector& vecMin, const Vector& vecMax, Vector out[8]) {
	out[0] = vecMin;
	out[7] = vecMax;

	for (int i = 1; i < 7; i++) {
		out[i].x = (i & 4) ? vecMax.x : vecMin.x;
		out[i].y = (i & 2) ? vecMax.y : vecMin.y;
		out[i].z = (i & 1) ? vecMax.z : vecMin.z;
	}
}

void CSurfZonePlugin::DrawZone(const Vector points[8], bool flat) {
	static constexpr const int pairs[][2] = {{0, 2}, {2, 6}, {6, 4}, {4, 0}, {0, 1}, {3, 1}, {3, 2}, {3, 7}, {5, 1}, {5, 4}, {6, 7}, {7, 5}};

	for (int i = 0; i < (flat ? 4 : 12); i++) {
		UTIL::CreateBeam(points[pairs[i][0]], points[pairs[i][1]]);
	}
}

void CSurfZoneService::Reset() {
	m_iEditStep = EditStep_None;
}
