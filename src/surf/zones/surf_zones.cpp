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
	if (pZoneService->m_bCanDraw) {
		auto pos = pawn->GetAbsOrigin();
		auto mins = pawn->m_pCollision()->m_vecMins() + pos;
		auto maxs = pawn->m_pCollision()->m_vecMaxs() + pos;
		pZoneService->UpdateZone(pZoneService->m_vecTestBeam, mins, maxs);
	}

	/*this->DrawEditZone(pawn);

	auto iEditStep = pZoneService->m_iEditStep;
	if (iEditStep > EditStep_None && iEditStep < EditStep_Final) {
	}*/
}

void CSurfZonePlugin::DrawEditZone(CCSPlayerPawnBase* pawn) {
	trace_t tr;
	UTIL::GetPlayerAiming(pawn, tr);
	Vector& aimPos = tr.m_vEndPos;
}

void CSurfZoneService::CreateZonePoints(const Vector& vecMin, const Vector& vecMax, Vector out[8]) {
	out[0] = vecMin;
	out[7] = vecMax;

	for (int i = 1; i < 7; i++) {
		out[i].x = (i & 4) ? vecMax.x : vecMin.x;
		out[i].y = (i & 2) ? vecMax.y : vecMin.y;
		out[i].z = (i & 1) ? vecMax.z : vecMin.z;
	}
}

void CSurfZoneService::CreateZone(const Vector points[8], bool flat, std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < (flat ? 4 : 12); i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs[i][0]], points[m_iZonePairs[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void CSurfZoneService::UpdateZone(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[8];
	CreateZonePoints(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		pBeam->Teleport(&points[m_iZonePairs[i][0]], nullptr, nullptr);
		pBeam->m_vecEndPos(points[m_iZonePairs[i][1]]);
	}
}

void CSurfZoneService::Reset() {
	m_iEditStep = EditStep_None;
	m_bCanDraw = false;
}
