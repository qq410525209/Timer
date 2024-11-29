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

	player->m_pZoneService->EditZone(pawn, buttons);
}

void CSurfZoneService::EditZone(CCSPlayerPawnBase* pawn, const CInButton* buttons) {
	auto& iEditStep = this->m_iEditStep;
	if (this->m_bEditing) {
		trace_t tr;
		UTIL::GetPlayerAiming(pawn, tr);
		Vector& aim = tr.m_vEndPos;

		if (buttons->Pressed(IN_USE)) {
			iEditStep = (ZoneEditStep)(iEditStep + 1);

			switch (iEditStep) {
				case EditStep_First: {
					this->m_vecEditingMins = aim;
					Vector points2D[4] = {aim};
					this->CreateZone2D(points2D, this->m_vTestBeam);
					break;
				}
				case EditStep_Second: {
					this->m_vecEditingMaxs = aim;
					Vector points3D[8];
					this->CreatePoints3D(this->m_vecEditingMins, aim, points3D);
					this->m_vTestBeam.clear();
					this->CreateZone3D(points3D, this->m_vTestBeam);
					break;
				}
				case EditStep_Third: {
					this->m_vecEditingMaxs.z = aim.z;
					auto pController = pawn->GetController();
					UTIL::PrintChat(pController, "The last step! Confirmed?");
					break;
				}
				case EditStep_Final: {
					auto pController = pawn->GetController();
					UTIL::PrintChat(pController, "Confirmed!\n");
					UTIL::PrintChat(pController, "mins: %f %f %f\n", this->m_vecEditingMins.x, this->m_vecEditingMins.y, this->m_vecEditingMins.z);
					UTIL::PrintChat(pController, "maxs: %f %f %f\n", this->m_vecEditingMaxs.x, this->m_vecEditingMaxs.y, this->m_vecEditingMaxs.z);
					this->Reset();
					return;
				}
			}
		}

		switch (this->m_iEditStep) {
			case EditStep_None: {
				auto pBeam = this->m_vTestBeam[0].Get();
				pBeam->Teleport(&aim, nullptr, nullptr);
				pBeam->m_vecEndPos(aim);
				break;
			}
			case EditStep_First: {
				this->UpdateZone2D(this->m_vTestBeam, this->m_vecEditingMins, aim);
				break;
			}
			case EditStep_Second: {
				this->m_vecEditingMaxs.z = aim.z;
				this->UpdateZone3D(this->m_vTestBeam, this->m_vecEditingMins, this->m_vecEditingMaxs);
				break;
			}
			case EditStep_Third: {
				this->UpdateZone3D(this->m_vTestBeam, this->m_vecEditingMins, this->m_vecEditingMaxs);
				break;
			}
		}
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
		pBeam->Teleport(&points[m_iZonePairs2D[i][0]], nullptr, nullptr);
		pBeam->m_vecEndPos(points[m_iZonePairs2D[i][1]]);
	}
}

void CSurfZoneService::UpdateZone3D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[8];
	CreatePoints3D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		pBeam->Teleport(&points[m_iZonePairs3D[i][0]], nullptr, nullptr);
		pBeam->m_vecEndPos(points[m_iZonePairs3D[i][1]]);
	}
}

void CSurfZoneService::Reset() {
	m_iEditStep = EditStep_None;
	m_bEditing = false;
}
