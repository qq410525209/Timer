#include "surf_zones.h"
#include <utils/print.h>
#include <utils/utils.h>

CSurfZonePlugin g_SurfZonePlugin;

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

void CSurfZoneService::Reset() {
	m_iEditStep = EditStep_None;
}
