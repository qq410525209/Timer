#include <core/concmdmanager.h>
#include <surf/surf_player.h>
#include <utils/print.h>

static void Command_HideLegs(CCSPlayerController* pController, const std::vector<std::string>& vArgs) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	player->m_bHideLegs = !player->m_bHideLegs;

	UTIL::PrintChat(pController, "%s 腿部\n", player->m_bHideLegs ? "已隐藏" : "已显示");
}

void RegisterCommands() {
	CONCMD::RegConsoleCmd("sm_hideleg", Command_HideLegs);
	CONCMD::RegConsoleCmd("sm_hidelegs", Command_HideLegs);
}
