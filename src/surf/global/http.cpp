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
		void Update(const std::string& map, const zoneinfo_t& info, HttpResponseCallback cb) {
			auto pPlugin = GlobalPlugin();
			GlobalAPIRequest req;
			req.m_sCustomToken = pPlugin->m_UpdaterAuth.m_sToken;
			req.m_iMethod = HTTP_POST;

			req["map"] = map;
			req["track"] = info.m_iTrack;
			req["type"] = info.m_iType;
			req["value"] = info.m_iValue;
			req["mins_x"] = info.m_vecMins.x;
			req["mins_y"] = info.m_vecMins.y;
			req["mins_z"] = info.m_vecMins.z;
			req["maxs_x"] = info.m_vecMaxs.x;
			req["maxs_y"] = info.m_vecMaxs.y;
			req["maxs_z"] = info.m_vecMaxs.z;

			if (info.m_iDatabaseID != -1) {
				req["id"] = info.m_iDatabaseID;
			}

			if (info.m_vecDes.Length() != 0) {
				req["des_x"] = info.m_vecDes.x;
				req["des_y"] = info.m_vecDes.y;
				req["des_z"] = info.m_vecDes.z;
			}

			if (info.m_iFlag != -1) {
				req["flags"] = info.m_iFlag;
			}

			if (info.m_iHookHammerid != -1) {
				req["hammerid"] = info.m_iHookHammerid;
			}

			if (!info.m_sHookName.empty()) {
				req["hookname"] = info.m_sHookName;
			}

			if (info.m_fLimitSpeed != -1.0f) {
				req["limitspeed"] = info.m_fLimitSpeed;
			}

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
