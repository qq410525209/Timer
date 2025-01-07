#pragma once

#include <surf/surf_player.h>

class CSurfGlobalAPIPlugin : CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) override;

private:
	void Reset();
	void RegisterCommand();

public:
	void ReadAPIKey();

public:
	bool m_bAPIKeyCheck;
	bool m_bBannedCommandsCheck;
	bool m_bEnforcerOnFreshMap;
	bool m_bMapValidated;
	int m_iMapWorkshopID;
	int m_iCurrentMapWorkshopID;
	std::string m_sAPIKey;
	std::string m_sZoneHelperKey;
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

	namespace GLOBAL {
		inline constexpr const char* BaseUrl = "";
	} // namespace GLOBAL
} // namespace SURF
