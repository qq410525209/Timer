#include "edit.h"
#include "surf_zones.h"
#include <surf/misc/surf_misc.h>
#include <core/menu.h>
#include <utils/utils.h>
#include <fmt/format.h>

void ZoneEditProperty::Init(CSurfZoneService* outer) {
	m_pOuter = outer;
	this->Reset();
}

void ZoneEditProperty::Reset() {
	m_bEnabled = false;
	m_iStep = EditStep_None;
	m_bAwaitValueInput = false;
	m_bAwaitVelocityInput = false;

	ZoneData_t::Reset();

	this->ClearBeams();
}

void ZoneEditProperty::StartEditZone() {
	m_bEnabled = true;
	m_iStep = EditStep_None;

	trace_t tr;
	UTIL::GetPlayerAiming(m_pOuter->GetPlayer()->GetPlayerPawn(), tr);
	Vector& aimPos = tr.m_vEndPos;

	m_vBeam.clear();
	auto pBeam = UTIL::CreateBeam(tr.m_vEndPos, tr.m_vEndPos);
	m_vBeam.emplace_back(pBeam->GetRefEHandle());
}

void ZoneEditProperty::CreateEditZone(const Vector& playerAim) {
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
			SURF::ZONE::CreatePoints3D(this->m_vecMins, playerAim, points3D);
			this->ClearBeams();
			this->CreateZone3D(points3D, this->m_vBeam);
			break;
		}
		case EditStep_Third: {
			this->m_vecMaxs.z = playerAim.z;
			this->EnsureDestination();
			this->EnsureSettings();
			break;
		}
	}
}

void ZoneEditProperty::UpdateZone(const Vector& playerAim) {
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

void ZoneEditProperty::CreateZone2D(const Vector points[4], std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < 4; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs3D[i][0]], points[m_iZonePairs3D[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void ZoneEditProperty::CreateZone3D(const Vector points[8], std::vector<CHandle<CBeam>>& out) {
	for (int i = 0; i < 12; i++) {
		CBeam* beam = (CBeam*)UTIL::CreateBeam(points[m_iZonePairs3D[i][0]], points[m_iZonePairs3D[i][1]]);
		out.emplace_back(beam->GetRefEHandle());
	}
}

void ZoneEditProperty::UpdateZone2D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[4];
	SURF::ZONE::CreatePoints2D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		auto& vecStart = points[m_iZonePairs2D[i][0]];
		auto& vecEnd = points[m_iZonePairs2D[i][1]];
		pBeam->Teleport(&vecStart, nullptr, nullptr);
		pBeam->m_vecEndPos(vecEnd);
	}
}

void ZoneEditProperty::UpdateZone3D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax) {
	Vector points[8];
	SURF::ZONE::CreatePoints3D(vecMin, vecMax, points);
	for (int i = 0; i < vBeams.size(); i++) {
		auto pBeam = vBeams[i].Get();
		auto& vecStart = points[m_iZonePairs3D[i][0]];
		auto& vecEnd = points[m_iZonePairs3D[i][1]];
		pBeam->Teleport(&vecStart, nullptr, nullptr);
		pBeam->m_vecEndPos(vecEnd);
	}
}

void ZoneEditProperty::ClearBeams() {
	for (const auto& hBeam : m_vBeam) {
		auto pBeam = hBeam.Get();
		if (pBeam) {
			pBeam->Kill();
		}
	}
	m_vBeam.clear();
}

void ZoneEditProperty::EnsureSettings() {
	auto pPlayer = this->m_pOuter->GetPlayer();
	if (!pPlayer) {
		SDK_ASSERT(false);
		return;
	}

	auto wpMenu = MENU::Create(
		pPlayer->GetController(), MENU_CALLBACK_L(this) {
			auto pPlayer = this->m_pOuter->GetPlayer();
			if (!pPlayer) {
				SDK_ASSERT(false);
				return;
			}

			if (action == EMenuAction::SelectItem) {
				switch (iItem) {
					case 0: {
						pPlayer->m_pZoneService->Print("已确认!");

						SURF::ZonePlugin()->UpsertZone(*this);
						this->Reset();
						hMenu.Close();
						break;
					}
					case 1: {
						pPlayer->m_pZoneService->Print("已取消.");
						this->Reset();
						hMenu.Close();
						break;
					}
					case 2: {
						pPlayer->m_pZoneService->Print("在聊天中输入您需要的数据.");
						this->m_bAwaitValueInput = true;
						break;
					}
					case 3: {
						pPlayer->Teleport(&this->m_vecDestination, this->m_angDestination.IsValid() ? &this->m_angDestination : nullptr, &SURF::ZERO_VEC);
						pPlayer->m_pZoneService->Print("已传送.");
						break;
					}
					case 4: {
						if (!IsInsideBox(this->m_vecDestination)) {
							pPlayer->m_pZoneService->Print("未处于区域内.");
						} else {
							pPlayer->GetOrigin(this->m_vecDestination);
							pPlayer->GetAngles(this->m_angDestination);
							pPlayer->m_pZoneService->Print("已设置传送点.");
						}

						break;
					}
					case 5: {
						pPlayer->m_pZoneService->Print("功能没处理.");
						break;
					}
					case 6: {
						pPlayer->m_pZoneService->Print("在聊天中输入您需要的数据, {green}0{default}则表示无限速.");
						this->m_bAwaitVelocityInput = true;
						break;
					}
				}
			} else if (action == EMenuAction::Exit) {
				this->Reset();
			}
		});

	if (wpMenu.expired()) {
		SDK_ASSERT(false);
		return;
	}

	auto pMenu = wpMenu.lock();
	pMenu->SetTitle("确认区域设置");

	pMenu->AddItem("是");
	pMenu->AddItem("否");

	auto sData = fmt::format("当前值: {}", m_iValue);
	pMenu->AddItem(sData);

	pMenu->AddItem("传送到区域");
	pMenu->AddItem("设置传送点");
	pMenu->AddItem("调整区域");

	auto sVelocity = fmt::format("当前限速: {}", m_iLimitSpeed);
	pMenu->AddItem(sVelocity);

	// pMenu->AddItem("hookname: ?");
	// pMenu->AddItem("hammerid: ?");

	pMenu->Display();
}

void ZoneData_t::EnsureDestination() {
	for (const auto& hTele : SURF::MiscPlugin()->m_vTeleDestination) {
		auto pTeleEnt = hTele.Get();
		if (pTeleEnt) {
			const Vector& origin = pTeleEnt->GetOrigin();
			if (IsInsideBox(origin)) {
				m_vecDestination = origin;
				m_angDestination = pTeleEnt->GetAbsAngles();
				return;
			}
		}
	}

	m_vecDestination = GetCenter();
	m_angDestination.Invalidate();
}
