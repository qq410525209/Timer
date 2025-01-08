#include "surf_global.h"
#include <hv/requests.h>
#include <hv/hthread.h>

void CSurfGlobalAPIPlugin::CreateRequest(std::string sEndpointAlias, const GlobalAPIRequest& req, HttpResponseCallback& cb) {
	if (!m_umEndpoint.contains(sEndpointAlias)) {
		SURF_ASSERT(false);
		return;
	}

	auto& sEndpoint = m_umEndpoint[sEndpointAlias];

	switch (req.m_iMethod) {
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
			SURF_ASSERT(false);
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
	} // namespace MAP

	namespace STYLE {}

	namespace PLAYER {}

	namespace RECORD {}

	namespace ZONE {
		void Update(const std::string& map, const std::vector<zoneinfo_t>& vInfo, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;
			req.m_sCustomToken = pPlugin->m_UpdaterAuth.m_sToken;
			req.m_iMethod = HTTP_POST;

			json jArr = json::array();
			for (const auto& info : vInfo) {
				jArr.emplace_back(info.to_json(map));
			}

			req["data"] = jArr;

			pPlugin->CreateRequest("zone_update", req, cb);
		}

		void Pull(const std::string& map, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;

			req["map"] = map;

			pPlugin->CreateRequest("zone_pull", req, cb);
		}
	} // namespace ZONE
} // namespace SURF::GLOBALAPI
