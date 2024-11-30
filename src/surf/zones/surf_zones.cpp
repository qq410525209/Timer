#include "surf_zones.h"
#include <utils/print.h>
#include <utils/utils.h>

extern void RegisterCommand();
extern void Hook_OnStartTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);
extern void Hook_OnTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);
extern void Hook_OnEndTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);

CSurfZonePlugin g_SurfZonePlugin;

CSurfZonePlugin* SurfZonePlugin() {
	return &g_SurfZonePlugin;
}

void CSurfZonePlugin::OnPluginStart() {
	RegisterCommand();
}

void CSurfZonePlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3],
										 const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pawn);
	if (!player) {
		return;
	}

	player->m_pZoneService->EditZone(pawn, buttons);
}

void CSurfZoneService::EditZone(CCSPlayerPawnBase* pawn, const CInButton* buttons) {
	auto& iEditStep = m_ZoneEdit.m_iStep;
	if (m_ZoneEdit.m_bEnabled) {
		trace_t tr;
		UTIL::GetPlayerAiming(pawn, tr);
		Vector& aim = tr.m_vEndPos;

		if (buttons->Pressed(IN_USE)) {
			iEditStep = (ZoneEditStep)(iEditStep + 1);

			m_ZoneEdit.CreateEditZone(aim);
		}

		m_ZoneEdit.UpdateZone(aim);
	}
}

void CSurfZoneService::CreatePoints2D(const Vector& vecMin, const Vector& vecMax, Vector out[4]) {
	out[0] = Vector(vecMin.x, vecMin.y, vecMin.z);
	out[1] = Vector(vecMax.x, vecMin.y, vecMin.z);
	out[2] = Vector(vecMax.x, vecMax.y, vecMin.z);
	out[3] = Vector(vecMin.x, vecMax.y, vecMin.z);
}

void CSurfZoneService::CreatePoints3D(const Vector& vecMin, const Vector& vecMax, Vector out[8]) {
	out[0] = vecMin;
	out[7] = vecMax;

	for (int i = 1; i < 7; i++) {
		out[i].x = (i & 4) ? vecMax.x : vecMin.x;
		out[i].y = (i & 2) ? vecMax.y : vecMin.y;
		out[i].z = (i & 1) ? vecMax.z : vecMin.z;
	}
}

void CSurfZoneService::CreateZone2D(const Vector points[4], std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < 4; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs3D[i][0]], points[m_iZonePairs3D[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void CSurfZoneService::CreateZone3D(const Vector points[8], std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < 12; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs3D[i][0]], points[m_iZonePairs3D[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void CSurfZoneService::UpdateZone2D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[4];
	CreatePoints2D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		auto& vecStart = points[m_iZonePairs2D[i][0]];
		auto& vecEnd = points[m_iZonePairs2D[i][1]];
		pBeam->Teleport(&vecStart, nullptr, nullptr);
		pBeam->m_vecEndPos(vecEnd);
	}
}

void CSurfZoneService::UpdateZone3D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[8];
	CreatePoints3D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		auto& vecStart = points[m_iZonePairs3D[i][0]];
		auto& vecEnd = points[m_iZonePairs3D[i][1]];
		pBeam->Teleport(&vecStart, nullptr, nullptr);
		pBeam->m_vecEndPos(vecEnd);
	}
}

CBaseEntity* CSurfZoneService::CreateNormalZone(const Vector& vecMins, const Vector& vecMaxs) {
	Vector vecCenter = (vecMins + vecMaxs) / 2.0;
	Vector mins(vecMins), maxs(vecMaxs);
	FillBoxMinMax(mins, maxs);
	auto pZone = MEM::CALL::CreateAABBTrigger(vecCenter, mins, maxs);
	if (!pZone) {
		SURF_ASSERT(false);
		return nullptr;
	}

	MEM::SDKHOOK::StartTouchPost(pZone, Hook_OnStartTouchPost);
	MEM::SDKHOOK::TouchPost(pZone, Hook_OnTouchPost);
	MEM::SDKHOOK::EndTouchPost(pZone, Hook_OnEndTouchPost);

	pZone->m_pEntity->m_name = GameEntitySystem()->AllocPooledString("surf_zone");

	return pZone;
}

void CSurfZoneService::FillBoxMinMax(Vector& vecMin, Vector& vecMax) {
	if (vecMin.x > vecMax.x) {
		std::swap((float&)vecMin.x, (float&)vecMax.x);
	}
	if (vecMin.y > vecMax.y) {
		std::swap((float&)vecMin.y, (float&)vecMax.y);
	}
	if (vecMin.z > vecMax.z) {
		std::swap((float&)vecMin.z, (float&)vecMax.z);
	}

	// Calculate the size of the original bounding box
	Vector size(vecMax.x - vecMin.x, vecMax.y - vecMin.y, vecMax.z - vecMin.z);

	// Generate new mins and maxs centered at (0, 0, 0)
	vecMin.x = -size.x / 2;
	vecMin.y = -size.y / 2;
	vecMin.z = -size.z / 2;

	vecMax.x = size.x / 2;
	vecMax.y = size.y / 2;
	vecMax.z = size.z / 2;
}

void CSurfZoneService::Reset() {
	m_ZoneEdit.Init(this);
}

void CZoneEditProperty::StartEditZone() {
	m_bEnabled = true;
	m_iStep = EditStep_None;

	trace_t tr;
	UTIL::GetPlayerAiming(m_pOuter->GetPlayer()->GetPlayerPawn(), tr);
	Vector& aimPos = tr.m_vEndPos;

	auto pBeam = UTIL::CreateBeam(tr.m_vEndPos, tr.m_vEndPos);
	m_vBeam.clear();
	m_vBeam.emplace_back(pBeam->GetRefEHandle());
}

void CZoneEditProperty::CreateEditZone(const Vector& playerAim) {
	switch (m_iStep) {
		case EditStep_First: {
			this->m_vecMins = playerAim;
			Vector points2D[4] = {playerAim};
			this->m_vBeam.clear();
			m_pOuter->CreateZone2D(points2D, this->m_vBeam);
			break;
		}
		case EditStep_Second: {
			this->m_vecMaxs = playerAim;
			Vector points3D[8];
			m_pOuter->CreatePoints3D(this->m_vecMins, playerAim, points3D);
			this->m_vBeam.clear();
			m_pOuter->CreateZone3D(points3D, this->m_vBeam);
			break;
		}
		case EditStep_Third: {
			this->m_vecMaxs.z = playerAim.z;
			auto pController = m_pOuter->GetPlayer()->GetController();
			UTIL::PrintChat(pController, "The final step! Confirmed?");
			break;
		}
		case EditStep_Final: {
			auto pController = m_pOuter->GetPlayer()->GetController();
			UTIL::PrintChat(pController, "Confirmed!\n");
			m_pOuter->CreateNormalZone(this->m_vecMins, this->m_vecMaxs);
			m_pOuter->Reset();
			return;
		}
	}
}

void CZoneEditProperty::UpdateZone(const Vector& playerAim) {
	switch (this->m_iStep) {
		case EditStep_None: {
			auto pBeam = this->m_vBeam[0].Get();
			pBeam->Teleport(&playerAim, nullptr, nullptr);
			pBeam->m_vecEndPos(playerAim);
			break;
		}
		case EditStep_First: {
			m_pOuter->UpdateZone2D(this->m_vBeam, this->m_vecMins, playerAim);
			break;
		}
		case EditStep_Second: {
			this->m_vecMaxs.z = playerAim.z;
			m_pOuter->UpdateZone3D(this->m_vBeam, this->m_vecMins, this->m_vecMaxs);
			break;
		}
		case EditStep_Third: {
			m_pOuter->UpdateZone3D(this->m_vBeam, this->m_vecMins, this->m_vecMaxs);
			break;
		}
	}
}
