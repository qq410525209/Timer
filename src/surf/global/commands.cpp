#include "surf_global.h"
#include <core/concmdmanager.h>
#include <utils/utils.h>
#include <fmt/format.h>
#include <regex>

static bool MapCheck() {
	auto pAPIPlugin = SURF::GlobalPlugin();

	return pAPIPlugin->m_bMapValidated;
}

static void DoGlobalCheck(CBasePlayerController* pController) {
	auto pAPIPlugin = SURF::GlobalPlugin();

	bool bClientGloballyVerified = true;
	if (pController) {
		auto pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
		if (pSurfPlayer) {
			bClientGloballyVerified = pSurfPlayer->m_pGlobalAPIService->m_bGloballyVerified;
		}
	}

	auto GCArg = [](bool bStatus) -> const char* { return bStatus ? "[green]✓" : "[darkred]X"; };

	// clang-format off
	std::string sGC = fmt::format("[grey]API接入检测:\n[grey]API密钥 {}[grey] | 插件 {}[grey] | 参数设置 {}[grey] | 地图 {}[grey] | 你 {}",
					GCArg(pAPIPlugin->m_bAPIKeyCheck),
					GCArg(pAPIPlugin->m_bBannedCommandsCheck),
					GCArg(pAPIPlugin->m_bEnforcerOnFreshMap),
					GCArg(MapCheck()),
					GCArg(bClientGloballyVerified));
	// clang-format on

	if (!pController) {
		std::regex pattern(R"(\[[^\]]*\])");
		sGC = std::regex_replace(sGC, pattern, "");
	}

	UTIL::Print(pController, sGC.c_str());
}

CCMD_CALLBACK(Command_GlobalCheck) {
	DoGlobalCheck(pController);
}

SCMD_CALLBACK(Command_GlobalCheck_Srv) {
	DoGlobalCheck(nullptr);
}

CCMD_CALLBACK(Command_GlobalAPIInfo) {}

CCMD_CALLBACK(Command_GlobalAPIReload) {
	SURF::GlobalPlugin()->ReadAPIKey();
}

void CSurfGlobalAPIPlugin::RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_gc", Command_GlobalCheck);
	CONCMD::RegConsoleCmd("sm_globalcheck", Command_GlobalCheck);
	CONCMD::RegServerCmd("sm_gc", Command_GlobalCheck_Srv);
	CONCMD::RegServerCmd("sm_globalcheck", Command_GlobalCheck_Srv);
	CONCMD::RegAdminCmd("sm_globalapi_info", Command_GlobalAPIInfo, AdminFlag::Root);
	CONCMD::RegAdminCmd("sm_globalapi_reload_apikey", Command_GlobalAPIReload, AdminFlag::Root);
}
