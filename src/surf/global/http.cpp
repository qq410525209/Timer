#include "surf_global.h"
#include <hv/requests.h>
#include <hv/hthread.h>
#include <core/logger.h>

void CSurfGlobalAPIPlugin::CreateRequest(std::string sEndpointAlias, const GlobalAPIRequest& req, HttpResponseCallback& cb) {
	if (!m_umEndpoint.contains(sEndpointAlias)) {
		LOG::Warning("[SurfGlobalAPIPlugin::CreateRequest] Endpoint alias \'%s\' not exists!\n", sEndpointAlias.c_str());
		return;
	}

	auto& sEndpoint = m_umEndpoint[sEndpointAlias];

	switch (req.m_iMethod) {
		case HTTP_GET: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_GET;

			if (!req.empty()) {
				for (auto it = req.begin(); it != req.end(); ++it) {
					auto& val = it.value();
					http_req->query_params[it.key()] = val.is_string() ? val.get<std::string>() : val.dump();
				}
			}
			http_req->url = sEndpoint;

			http_req->timeout = req.m_iTimeout;

			http_req->SetBearerTokenAuth(req.m_sCustomToken.empty() ? m_GlobalAuth.m_sToken : req.m_sCustomToken);

			http_client_send_async(http_req, cb);
			break;
		}
		case HTTP_POST: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_POST;
			http_req->url = sEndpoint;
			http_req->timeout = req.m_iTimeout;

			if (!req.empty()) {
				std::string sBody = req.dump();
				http_req->SetBody(sBody);
			}

			http_req->SetBearerTokenAuth(req.m_sCustomToken.empty() ? m_GlobalAuth.m_sToken : req.m_sCustomToken);
			http_req->SetHeader("Content-Type", "application/json");

			http_client_send_async(http_req, cb);
			break;
		}
		default: {
			SDK_ASSERT(false);
			break;
		}
	}
}

namespace SURF::GLOBALAPI {
	namespace AUTH {
		void GetGlobalToken(std::string global_key, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;
			req.m_sCustomToken = global_key;
			pPlugin->CreateRequest("auth_status", req, cb);
		}

		void GetUpdaterToken(std::string zone_key, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;
			req.m_sCustomToken = zone_key;
			pPlugin->CreateRequest("auth_updater", req, cb);
		}
	} // namespace AUTH

	namespace BAN {}

	namespace MAP {
		void UpdateInfo(uint64 workshopID, std::string mapName, const mapinfo_t& info, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;
			req.m_sCustomToken = pPlugin->m_UpdaterAuth.m_sToken;
			req.m_iMethod = HTTP_POST;

			req["workshopid"] = workshopID;
			req["map"] = mapName;
			req["tier"] = info.m_iTier;
			req["maxvelocity"] = info.m_fMaxvel;
			req["limitPrespeed"] = info.m_bLimitPrespeed;

			pPlugin->CreateRequest("map_info_update", req, cb);
		}

		void PullInfo(uint64 workshopID, std::string mapName, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;

			req["workshopid"] = workshopID;
			req["map"] = mapName;

			pPlugin->CreateRequest("map_info_pull", req, cb);
		}

		void UpdateZone(const zoneinfo_t& info, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;
			req.m_sCustomToken = pPlugin->m_UpdaterAuth.m_sToken;
			req.m_iMethod = HTTP_POST;

			json jInfo = info.ToJson();
			req.merge_patch(jInfo);

			pPlugin->CreateRequest("map_zone_update", req, cb);
		}

		void PullZone(HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;

			req["map"] = pPlugin->m_sMapName;

			pPlugin->CreateRequest("map_zone_pull", req, cb);
		}
	} // namespace MAP

	namespace STYLE {}

	namespace PLAYER {}

	namespace RECORD {}
} // namespace SURF::GLOBALAPI
