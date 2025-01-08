#include "surf_global.h"
#include <surf/api.h>
#include <utils/utils.h>
#include <fmt/format.h>
#include <regex>

CSurfGlobalAPIPlugin g_GlobalApi;

CSurfGlobalAPIPlugin* SURF::GlobalPlugin() {
	return &g_GlobalApi;
}

void CSurfGlobalAPIPlugin::OnPluginStart() {
	RegisterCommand();
	ReadAPIKey();
}

void CSurfGlobalAPIPlugin::OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(slot);
	if (!pPlayer) {
		return;
	}

	pPlayer->m_pGlobalAPIService->CheckGlobalBan();
}

void CSurfGlobalAPIPlugin::OnApplyGameSettings(ISource2Server* pServer, KeyValues* pKV) {
	Reset();

	if (!pKV) {
		return;
	}

	auto pGlobal = UTIL::GetGlobals();
	if (!pGlobal) {
		SURF_ASSERT(false);
		return;
	}

	m_iMapWorkshopID = pKV->FindKey("launchoptions")->GetUint64("customgamemode");
	m_sMapName = pKV->FindKey("launchoptions")->GetString("levelname", pGlobal->mapname.ToCStr());

	SURF::GLOBALAPI::AUTH::GetGlobalToken(
		m_GlobalAuth.m_sKey, HTTPRES_CALLBACK_L() {
			GAPIRES_CHECK(res, r);

			JSON_GETTER(r.m_Data, token, SURF::GlobalPlugin()->m_GlobalAuth.m_sToken);

			FORWARD_POST(CSurfGlobalForward, OnGlobalInit);
		});

	SURF::GLOBALAPI::AUTH::GetUpdaterToken(
		m_UpdaterAuth.m_sKey, HTTPRES_CALLBACK_L() {
			GAPIRES_CHECK(res, r);

			JSON_GETTER(r.m_Data, token, SURF::GlobalPlugin()->m_UpdaterAuth.m_sToken);

			FORWARD_POST(CSurfGlobalForward, OnGlobalZoneHelperInit);
		});

	if (m_iMapWorkshopID != 0) {
		SURF::GLOBALAPI::MAP::PullInfo(
			m_iMapWorkshopID, m_sMapName, HTTPRES_CALLBACK_L() {
				GAPIRES_CHECK(res, r);

				SURF::GlobalPlugin()->m_bMapValidated = true;

				JSON_GETTER(r.m_Data, tier, SURF::GLOBALAPI::MAP::g_MapInfo.m_iTier);
				JSON_GETTER(r.m_Data, maxvel, SURF::GLOBALAPI::MAP::g_MapInfo.m_fMaxvel);
				JSON_GETTER(r.m_Data, limitpre, SURF::GLOBALAPI::MAP::g_MapInfo.m_bLimitPrespeed);

				FORWARD_POST(CSurfGlobalForward, OnGlobalMapValidated);
			});
	}
}

void CSurfGlobalAPIPlugin::Reset() {
	m_bBannedCommandsCheck = false;
	m_bEnforcerOnFreshMap = false;
	m_bMapValidated = false;
	m_iMapWorkshopID = -1;
	m_sMapName = "";
}

void CSurfGlobalAPIPlugin::ReadAPIKey() {
	static auto sPath = UTIL::PATH::Join(UTIL::GetWorkingDirectory(), "configs", "api.jsonc");
	json j = UTIL::LoadJsonc(sPath);
	if (j.is_null() || j.empty()) {
		return;
	}

	JSON_GETTER(j, global_key, m_GlobalAuth.m_sKey);
	JSON_GETTER(j, updater_key, m_UpdaterAuth.m_sKey);

	if (j.contains("endpoints")) {
		auto& endpoints = j["endpoints"];
		for (const auto& [name, endpoint] : endpoints.items()) {
			m_umEndpoint[name] = SURF::GLOBALAPI::BaseUrl + endpoint.get<std::string>();
		}
	}
}

bool CSurfGlobalAPIPlugin::MapCheck() const {
	return m_bMapValidated;
}

void CSurfGlobalAPIPlugin::GlobalCheck(CBasePlayerController* pController) const {
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
					GCArg(!m_GlobalAuth.m_sToken.empty()),
					GCArg(m_bBannedCommandsCheck),
					GCArg(m_bEnforcerOnFreshMap),
					GCArg(MapCheck()),
					GCArg(bClientGloballyVerified));
	
	std::string sMapGC = fmt::format("[grey]地图数据上报API接入检测:\n[grey]API密钥 {}",
					GCArg(!m_UpdaterAuth.m_sToken.empty()));
	// clang-format on

	if (!pController) {
		std::regex pattern(R"(\[[^\]]*\])");
		sGC = std::regex_replace(sGC, pattern, "");
		sMapGC = std::regex_replace(sMapGC, pattern, "");
	}

	UTIL::Print(pController, sGC.c_str());
	UTIL::Print(pController, sMapGC.c_str());
}

bool CSurfGlobalAPIPlugin::IsGlobalEnabled() const {
	return !m_GlobalAuth.m_sToken.empty() && m_bBannedCommandsCheck && m_bEnforcerOnFreshMap && MapCheck();
}

bool CSurfGlobalAPIPlugin::IsGlobalUpdaterEnabled() const {
	return !m_UpdaterAuth.m_sToken.empty();
}

void CSurfGlobalAPIService::Reset() {
	m_bGloballyVerified = false;
}

void CSurfGlobalAPIService::CheckGlobalBan() {
	auto steam64 = this->GetPlayer()->GetSteamId64();
	// globalapi http stuff
}
