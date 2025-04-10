#include "surf_global.h"

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
			req["limit_prespeed"] = info.m_bLimitPrespeed;

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
