#include "surf_zones.h"
#include <utils/print.h>
#include <utils/utils.h>

extern void RegisterCommand();
extern void Hook_OnStartTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);
extern void Hook_OnTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);
extern void Hook_OnEndTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther);

Vector g_testVec1;
Vector g_testVec2;

CSurfZonePlugin g_SurfZonePlugin;

CSurfZonePlugin* SurfZonePlugin() {
	return &g_SurfZonePlugin;
}

void* g_tram1;

void* TEST(CCollisionProperty* a1, Vector* a2, Vector* a3) {
	auto outer = a1->GetOuter();
	if (strstr(outer->GetClassname(), "trigger_")) {
		return nullptr;
	}
	auto ret = MEM::SDKCall<void*>(g_tram1, a1, a2, a3);
	return ret;
}

void CSurfZonePlugin::OnPluginStart() {
	RegisterCommand();

	// auto fn = libmem::SignScan("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? F2 0F 10 02 49 8B F0", LIB::server);
	// libmem::HookFunc(fn, TEST, g_tram1);

	// auto fn = libmem::SignScan("40 53 48 83 EC ? 48 8B 49 ? 48 8B DA 48 8B 01 FF 50 ? 48 85 C0 74 ? 48 8B 0B", LIB::server);
	// libmem::HookFunc(fn, TEST, g_tram1);

	/*auto fn = libmem::SignScan("48 89 5C 24 ? 48 89 7C 24 ? 55 48 8B EC 48 83 EC ? 48 8B F9 4C 8B C2", LIB::server);
	libmem::HookFunc(fn, TEST, g_tram1);*/
	/*auto fn4 = libmem::SignScan("48 8D 55 ? 48 89 5D ? E8 ? ? ? ? 48 C7 45", LIB::server);
	for (int i = 0; i < 13; i++) {
		libmem::StoreToAddress((void*)((char*)fn4 + i), 0x90);
	}*/
}

void CSurfZonePlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3],
										 const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pawn);
	if (!player) {
		return;
	}

	player->m_pZoneService->EditZone(pawn, buttons);
}

#include <core/logger.h>

void CSurfZonePlugin::OnResourcePrecache(IEntityResourceManifest* pResourceManifest) {
	pResourceManifest->AddResource("models/props/cs_office/vending_machine.vmdl");
	/*void* ret = nullptr;
	const char* test = "models/props/cs_office/vending_machine.vmdl";
	auto ret2 = vmt::CallVirtual<void*>(12, IFACE::pGameModelInfo, IFACE::pGameModelInfo, ret, test);
	if (ret) {
		LOG::Print("fuckl");
	}*/
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
					this->m_vTestBeam.clear();
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
					this->CreateNormalZone(this->m_vecEditingMins, this->m_vecEditingMaxs);
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

#include <utils/ctimer.h>

CBaseEntity* CSurfZoneService::CreateNormalZone(const Vector& vecMins, const Vector& vecMaxs) {
	Vector vecCenter = (vecMins + vecMaxs) / 2.0;
	auto testMins = Vector(-100.0f, -100.5417f, -100.000019f);
	auto testMaxs = Vector(100.000029f, 100.000011f, 100.000664f);
	auto testFN = libmem::SignScan("48 8B C4 48 89 50 ? 55 41 55", LIB::server);
	auto pZone = MEM::SDKCall<CBaseTrigger*>(testFN, &vecCenter, &testMins, &testMaxs);

	// auto pZone = (CBaseTrigger*)MEM::CALL::CreateEntityByName("trigger_physics");
	if (!pZone) {
		SURF_ASSERT(false);
		return nullptr;
	}

	static int iStartTouchOffset = GAMEDATA::GetOffset("CBaseEntity::StartTouch");
	libmem::VmtHook(pZone, iStartTouchOffset, Hook_OnStartTouchPost);

	static int iTouchOffset = GAMEDATA::GetOffset("CBaseEntity::Touch");
	libmem::VmtHook(pZone, iTouchOffset, Hook_OnTouchPost);

	static int iEndTouchOffset = GAMEDATA::GetOffset("CBaseEntity::EndTouch");
	libmem::VmtHook(pZone, iEndTouchOffset, Hook_OnEndTouchPost);

	return pZone;

	g_testVec1 = vecMins;
	g_testVec2 = vecMaxs;

	// CEntityKeyValues* pKV = new CEntityKeyValues();
	// pKV->SetVector("origin", vecCenter);
	// pKV->SetInt("spawnflags", 1);
	//// pKV->SetString("model", "models/props/cs_office/vending_machine.vmdl");
	// pKV->SetString("targetname", "surf_zone_1");

	// pZone->DispatchSpawn(pKV);

	static auto setmodel = libmem::SignScan("48 89 5C 24 ? 48 89 7C 24 ? 55 48 8B EC 48 83 EC ? 48 8B F9 4C 8B C2", LIB::server);
	MEM::SDKCall<void*>(setmodel, pZone, "models/props/cs_office/vending_machine.vmdl");

	pZone->Teleport(&vecCenter, nullptr, nullptr);
	this->GetPlayer()->GetPlayerPawn()->Teleport(&vecCenter, nullptr, nullptr);

	// auto testMins = Vector(-100.0f, -100.5417f, -100.000019f);
	// auto testMaxs = Vector(100.000029f, 100.000011f, 100.000664f);
	// auto& testMins = vecMins;
	// auto& testMaxs = vecMaxs;

	static auto activate = libmem::SignScan("4C 8B DC 53 56 48 81 EC", LIB::server);
	MEM::SDKCall<void*>(activate, pZone);

	auto fn = libmem::SignScan("48 81 C1 00 06 00 00 E9 04 8F 4A 00", LIB::server);
	auto ret = MEM::SDKCall<void*>(fn, pZone, &testMins, &testMaxs);

	/*pZone->m_Collision()->m_nSolidType(SOLID_BSP);
	pZone->m_Collision()->m_CollisionGroup(5);
	pZone->m_Collision()->m_collisionAttribute().m_nCollisionGroup(5);
	pZone->m_Collision()->m_vecSpecifiedSurroundingMaxs(vecMaxs);
	pZone->m_Collision()->m_vecSpecifiedSurroundingMins(vecMins);
	pZone->m_Collision()->m_vecSurroundingMaxs(vecMaxs);
	pZone->m_Collision()->m_vecSurroundingMins(vecMins);
	pZone->m_Collision()->m_nSurroundType(USE_OBB_COLLISION_BOUNDS);
	pZone->m_Collision()->m_usSolidFlags(FSOLID_TRIGGER);*/

	// pZone->m_flWait(0.0f);
	pZone->AcceptInput("Enable");
	// pZone->CollisionRulesChanged();
	// pZone->NetworkStateChanged();
}

void CSurfZoneService::Reset() {
	m_iEditStep = EditStep_None;
	m_bEditing = false;
	m_vTestBeam.clear();
}
