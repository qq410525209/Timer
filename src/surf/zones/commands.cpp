#include "surf_zones.h"
#include <core/concmdmanager.h>
#include <core/menu.h>
#include <utils/utils.h>

static void OpenMenu_SelectZoneType(CBasePlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto wpMenu = MENU::Create(
		pController, MENU_CALLBACK_L(player) {
			auto& pZoneService = player->m_pZoneService;
			pZoneService->m_ZoneEdit.m_iType = (ZoneType)iItem;
			pZoneService->m_ZoneEdit.m_iValue = SURF::ZonePlugin()->GetZoneCount(pZoneService->m_ZoneEdit.m_iTrack, (ZoneType)iItem);
			UTIL::PrintChat(pController, "SELECT: %s, value: %d\n", SURF::ZONE::GetZoneNameByType((ZoneType)iItem).c_str(), pZoneService->m_ZoneEdit.m_iValue);
			pZoneService->m_ZoneEdit.StartEditZone();
		});

	if (wpMenu.expired()) {
		SDK_ASSERT(false);
		return;
	}

	auto pMenu = wpMenu.lock();
	pMenu->SetTitle("选择类型");
	for (int i = ZoneType::Zone_Start; i < ZoneType::ZONETYPES_SIZE; i++) {
		pMenu->AddItem(SURF::ZONE::GetZoneNameByType((ZoneType)i));
	}
	pMenu->Display();
}

static void OpenMenu_SelectZoneTrack(CBasePlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto wpMenu = MENU::Create(
		pController, MENU_CALLBACK_L(player) {
			player->m_pZoneService->m_ZoneEdit.m_iTrack = (ZoneTrack)iItem;
			OpenMenu_SelectZoneType(pController);
		});

	if (wpMenu.expired()) {
		SDK_ASSERT(false);
		return;
	}

	auto pMenu = wpMenu.lock();
	pMenu->SetTitle("选择赛道");
	for (int i = ZoneTrack::Track_Main; i < ZoneTrack::TRACKS_SIZE; i++) {
		pMenu->AddItem(SURF::ZONE::GetZoneNameByTrack((ZoneTrack)i));
	}
	pMenu->Display();
}

CCMD_CALLBACK(Command_Zones) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto wpMenu = MENU::Create(
		pController, MENU_CALLBACK_L() {
			switch (iItem) {
				case 0:
					hMenu.Close();
					OpenMenu_SelectZoneTrack(pController);
					return;
				case 1:
					UTIL::PrintChat(pController, "SELECT 编辑\n");
					break;
				case 2:
					SURF::ZonePlugin()->RefreshZones();
					break;
			}
		});

	if (wpMenu.expired()) {
		SDK_ASSERT(false);
		return;
	}

	auto pMenu = wpMenu.lock();
	pMenu->SetTitle("区域菜单");
	pMenu->AddItem("添加");
	pMenu->AddItem("编辑");
	pMenu->AddItem("刷新");
	pMenu->Display();
}

CCMD_CALLBACK(Command_EditZone) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pZoneService = player->m_pZoneService;
	pZoneService->m_ZoneEdit.StartEditZone();
}

CCMD_CALLBACK(Command_TPStart) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	Vector testStart = {0.0f, 0.0f, 0.0f};

	player->GetPlayerPawn()->Teleport(&testStart, nullptr, nullptr);
}

void CSurfZonePlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_editzone", Command_EditZone);
	CONCMD::RegConsoleCmd("sm_r", Command_TPStart);
}
