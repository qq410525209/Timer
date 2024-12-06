#include "edit.h"
#include <utils/utils.h>
#include <surf/zones/surf_zones.h>

void CZoneEditProperty::Init(CSurfZoneService* outer) {
	m_pOuter = outer;
	this->Reset();
}

void CZoneEditProperty::Reset() {
	m_bEnabled = false;
	m_iStep = EditStep_None;
	m_vecMins = Vector();
	m_vecMaxs = Vector();

	this->ClearBeams();
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
			this->ClearBeams();
			this->CreateZone2D(points2D, this->m_vBeam);
			break;
		}
		case EditStep_Second: {
			this->m_vecMaxs = playerAim;
			Vector points3D[8];
			m_pOuter->CreatePoints3D(this->m_vecMins, playerAim, points3D);
			this->ClearBeams();
			this->CreateZone3D(points3D, this->m_vBeam);
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
			Vector mins(this->m_vecMins), maxs(this->m_vecMaxs);
			this->Reset();
			m_pOuter->AddZone(mins, maxs);
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
			this->UpdateZone2D(this->m_vBeam, this->m_vecMins, playerAim);
			break;
		}
		case EditStep_Second: {
			this->m_vecMaxs.z = playerAim.z;
			this->UpdateZone3D(this->m_vBeam, this->m_vecMins, this->m_vecMaxs);
			break;
		}
		case EditStep_Third: {
			this->UpdateZone3D(this->m_vBeam, this->m_vecMins, this->m_vecMaxs);
			break;
		}
	}
}

void CZoneEditProperty::CreateZone2D(const Vector points[4], std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < 4; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs3D[i][0]], points[m_iZonePairs3D[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void CZoneEditProperty::CreateZone3D(const Vector points[8], std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < 12; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs3D[i][0]], points[m_iZonePairs3D[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void CZoneEditProperty::UpdateZone2D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[4];
	CSurfZoneService::CreatePoints2D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		auto& vecStart = points[m_iZonePairs2D[i][0]];
		auto& vecEnd = points[m_iZonePairs2D[i][1]];
		pBeam->Teleport(&vecStart, nullptr, nullptr);
		pBeam->m_vecEndPos(vecEnd);
	}
}

void CZoneEditProperty::UpdateZone3D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[8];
	CSurfZoneService::CreatePoints3D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		auto& vecStart = points[m_iZonePairs3D[i][0]];
		auto& vecEnd = points[m_iZonePairs3D[i][1]];
		pBeam->Teleport(&vecStart, nullptr, nullptr);
		pBeam->m_vecEndPos(vecEnd);
	}
}

void CZoneEditProperty::ClearBeams() {
	for (const auto& hBeam : m_vBeam) {
		auto pBeam = hBeam.Get();
		if (pBeam) {
			pBeam->AcceptInput("kill");
		}
	}
	m_vBeam.clear();
}
