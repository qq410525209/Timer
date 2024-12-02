#include <core/concmdmanager.h>
#include <surf/surf_player.h>
#include <utils/print.h>
#include <surf/misc/surf_misc.h>

static void Command_HideLegs(CCSPlayerController* pController, const std::vector<std::string>& vArgs) {
	CSurfPlayer* player = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!player) {
		return;
	}

	auto& pMiscService = player->m_pMiscService;
	pMiscService->m_bHideLegs = !pMiscService->m_bHideLegs;

	UTIL::PrintChat(pController, "%s 腿部\n", pMiscService->m_bHideLegs ? "已隐藏" : "已显示");
}

void RegisterCommands() {
	CONCMD::RegConsoleCmd("sm_hideleg", Command_HideLegs);
	CONCMD::RegConsoleCmd("sm_hidelegs", Command_HideLegs);
}
