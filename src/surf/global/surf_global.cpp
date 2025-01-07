#include "surf_global.h"
#include <surf/api.h>
#include <utils/utils.h>
#include <hv/requests.h>
#include <hv/hthread.h>
#include <fmt/format.h>
#include <regex>

CSurfGlobalAPIPlugin g_GlobalApi;

CSurfGlobalAPIPlugin* SURF::GlobalPlugin() {
	return &g_GlobalApi;
}

void CSurfGlobalAPIPlugin::OnPluginStart() {
	RegisterCommand();
	ReadAPIKey();

	// FIXME: should move to http auth callback
	FORWARD_POST(CSurfGlobalForward, OnInit);
}

void CSurfGlobalAPIPlugin::OnActivateServer(CNetworkGameServerBase* pGameServer) {
	Reset();
}

void CSurfGlobalAPIPlugin::OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) {
	CSurfPlayer* pPlayer = SURF::GetPlayerManager()->ToPlayer(slot);
	if (!pPlayer) {
		return;
	}

	pPlayer->m_pGlobalAPIService->CheckGlobalBan();
}

void CSurfGlobalAPIPlugin::Reset() {
	m_bAPIKeyCheck = false;
	m_bBannedCommandsCheck = false;
	m_bEnforcerOnFreshMap = false;
	m_bMapValidated = false;
	m_iMapWorkshopID = -1;
	m_iCurrentMapWorkshopID = -1;
}

void CSurfGlobalAPIPlugin::ReadAPIKey() {
	static auto sPath = UTIL::PATH::Join(UTIL::GetWorkingDirectory(), "configs", "api.jsonc");
	json j = UTIL::LoadJsonc(sPath);
	if (j.is_null() || j.empty()) {
		return;
	}

	JSON_GETTER(j, global_key, m_sAPIKey);
	JSON_GETTER(j, zone_helper_key, m_sZoneHelperKey);

	if (j.contains("endpoints")) {
		auto& endpoints = j["endpoints"];
		for (const auto& [name, endpoint] : endpoints.items()) {
			m_umEndpoint[name] = SURF::GLOBAL::BaseUrl + endpoint.get<std::string>();
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
					GCArg(m_bAPIKeyCheck),
					GCArg(m_bBannedCommandsCheck),
					GCArg(m_bEnforcerOnFreshMap),
					GCArg(MapCheck()),
					GCArg(bClientGloballyVerified));
	// clang-format on

	if (!pController) {
		std::regex pattern(R"(\[[^\]]*\])");
		sGC = std::regex_replace(sGC, pattern, "");
	}

	UTIL::Print(pController, sGC.c_str());
}

void CSurfGlobalAPIPlugin::CreateRequest(std::string sEndpointAlias, const GlobalAPIRequest& req, HttpResponseCallback cb) {
	if (!m_umEndpoint.contains(sEndpointAlias)) {
		return;
	}

	auto& sEndpoint = m_umEndpoint[sEndpointAlias];

	switch (req.method) {
		case HTTP_GET: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_GET;

			std::string fullUrl = sEndpoint;
			if (!req.empty()) {
				std::string queryString;
				for (auto it = req.begin(); it != req.end(); ++it) {
					if (!queryString.empty()) {
						queryString += "&";
					}
					queryString += it.key() + "=" + it.value().dump();
				}

				fullUrl = sEndpoint + "?" + queryString;
			}
			http_req->url = fullUrl;

			http_req->timeout = req.iTimeout;

			if (req.bBearerToken) {
				http_req->SetBearerTokenAuth(m_sBearerToken);
			}

			http_client_send_async(http_req, cb);
			break;
		}
		case HTTP_POST: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_POST;
			http_req->url = sEndpoint;
			http_req->timeout = req.iTimeout;

			if (!req.empty()) {
				std::string sBody = req.dump();
				http_req->SetBody(sBody);
			}

			if (req.bBearerToken) {
				http_req->SetBearerTokenAuth(m_sBearerToken);
			}

			http_req->SetHeader("Content-Type", "application/json");

			http_client_send_async(http_req, cb);
			break;
		}
		default:
			break;
	}
}

void CSurfGlobalAPIService::Reset() {
	m_bGloballyVerified = false;
}

void CSurfGlobalAPIService::CheckGlobalBan() {
	auto steam64 = this->GetPlayer()->GetSteamId64();
	// globalapi http stuff
}
