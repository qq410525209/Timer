#pragma once

#include <surf/surf_player.h>
#include <hv/HttpMessage.h>

#ifndef HTTPRES_CALLBACK
#define HTTPRES_CALLBACK(fn) static void fn(const HttpResponsePtr& res)
#endif

#ifndef HTTPRES_CALLBACK_L
#define HTTPRES_CALLBACK_L(...) [__VA_ARGS__](const HttpResponsePtr& res) -> void
#endif

#define GAPIRES_CHECK(http_res, gapi_res) \
	if (!http_res || http_res->status_code != HTTP_STATUS_OK) { \
		return; \
	} \
	GlobalAPIResponse gapi_res(http_res->body); \
	if (gapi_res.m_iCode != HTTP_STATUS_OK) { \
		return; \
	}

struct GlobalAPIRequest : json {
	int m_iTimeout = 10;
	std::string m_sCustomToken = "";
	http_method m_iMethod = HTTP_GET;
};

struct GlobalAPIResponse {
	int m_iCode;
	std::string m_sMessage;
	json m_Data;
	uint64 m_iTimestamp;

	GlobalAPIResponse() = default;

	GlobalAPIResponse(const std::string& body) {
		json j = json::parse(body);
		from_json(j);
	}

	GlobalAPIResponse(const json& body) {
		from_json(body);
	}

	json to_json() const {
		return json {{"code", this->m_iCode}, {"message", this->m_sMessage}, {"data", this->m_Data}, {"timestamp", this->m_iTimestamp}};
	}

	void from_json(const json& j) {
		j.at("code").get_to(this->m_iCode);
		j.at("message").get_to(this->m_sMessage);
		j.at("data").get_to(this->m_Data);
		j.at("timestamp").get_to(this->m_iTimestamp);
	}
};

class CSurfGlobalAPIPlugin : CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) override;
	virtual void OnApplyGameSettings(ISource2Server* pServer, KeyValues* pKV) override;

private:
	void Reset();
	void RegisterCommand();

public:
	void ReadAPIKey();
	bool MapCheck() const;
	void GlobalCheck(CBasePlayerController* pController) const;

public:
	void CreateRequest(std::string sEndpointAlias, const GlobalAPIRequest& req, HttpResponseCallback& cb);

public:
	bool m_bAPIKeyCheck;
	bool m_bBannedCommandsCheck;
	bool m_bEnforcerOnFreshMap;
	bool m_bMapValidated;
	uint64 m_iMapWorkshopID;
	std::string m_sMapName;
	std::unordered_map<std::string, std::string> m_umEndpoint;

public:
	struct GlobalAPIPayload {
		std::string m_sKey;
		std::string m_sToken;
	};

	GlobalAPIPayload m_GlobalAuth;
	GlobalAPIPayload m_UpdaterAuth;
};

class CSurfGlobalAPIService : CSurfBaseService {
private:
	virtual void Reset() override;

public:
	using CSurfBaseService::CSurfBaseService;

	void CheckGlobalBan();

public:
	bool m_bGloballyVerified;
};

namespace SURF {
	extern CSurfGlobalAPIPlugin* GlobalPlugin();

	namespace GLOBALAPI {
		inline constexpr const char* BaseUrl = "";

		namespace AUTH {
			void GetGlobalToken(std::string global_key, HttpResponseCallback cb);
			void GetUpdaterToken(std::string global_key, HttpResponseCallback cb);
		} // namespace AUTH

		namespace BAN {}

		namespace MAP {
			inline struct mapinfo_t {
				int m_iTier;
				float m_fMaxvel;
				bool m_bLimitPrespeed;
			} g_MapInfo;

			void UpdateInfo(uint64 workshopID, std::string mapName, const mapinfo_t& info, HttpResponseCallback cb);
			void PullInfo(uint64 workshopID, std::string mapName, HttpResponseCallback cb);
		} // namespace MAP

		namespace STYLE {}

		namespace PLAYER {}

		namespace RECORD {}

		namespace ZONE {
			struct zoneinfo_t {
				int m_iDatabaseID = -1;
				int m_iTrack;
				int m_iType;
				int m_iValue;
				Vector m_vecMins;
				Vector m_vecMaxs;
				Vector m_vecDes;
				int m_iFlag = -1;
				int m_iHookHammerid = -1;
				std::string m_sHookName = std::string();
				float m_fLimitSpeed = -1.0f;
			};

			void Update(const std::string& map, const zoneinfo_t& info, HttpResponseCallback cb);
			void Pull(const std::string& map, HttpResponseCallback cb);
		} // namespace ZONE
	} // namespace GLOBALAPI
} // namespace SURF
