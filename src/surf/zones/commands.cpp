#include "surf_zones.h"
#include <core/concmdmanager.h>
#include <core/menu.h>
#include <utils/utils.h>

static void OpenAddZoneMenu(CBasePlayerController* pController) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto menu = MENU::Create(MENU_CALLBACK_L() {
		switch (iSelectedItem) {
			case 1:
				UTIL::PrintChat(pController, "SELECT 1\n");
				break;
			case 2:
				UTIL::PrintChat(pController, "SELECT 2\n");
				break;
		}

		hMenu.Free();
	});

	menu->SetTitle("选择赛道");
	for (int i = ZoneTrack::Track_Main; i < ZoneTrack::TRACKS_SIZE; i++) {
		std::string name = i == Track_Main ? "主线" : "奖励";
		menu->AddItem(name);
	}
	menu->Display(player->GetPlayerPawn());
}

CCMD_CALLBACK(Command_Zones) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto menu = MENU::Create(MENU_CALLBACK_L() {
		switch (iSelectedItem) {
			case 1:
				hMenu.Free();
				OpenAddZoneMenu(pController);
				return;
			case 2:
				UTIL::PrintChat(pController, "SELECT 2\n");
				break;
		}

		hMenu.Free();
	});
	menu->SetTitle("区域菜单");
	menu->AddItem("添加");
	menu->AddItem("编辑");
	menu->Display(player->GetPlayerPawn());
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

	player->GetPlayerPawn()->Teleport(&SurfZonePlugin()->m_vecTestStartZone, nullptr, nullptr);
}

void RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
	CONCMD::RegConsoleCmd("sm_editzone", Command_EditZone);
	CONCMD::RegConsoleCmd("sm_r", Command_TPStart);
}
