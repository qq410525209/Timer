#include "surf_global.h"
#include <utils/utils.h>

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

	JSON_GETTER(j, global_api, m_sAPIKey);
	JSON_GETTER(j, zone_helper, m_sZoneHelperKey);
}

void CSurfGlobalAPIService::Reset() {
	m_bGloballyVerified = false;
}

void CSurfGlobalAPIService::CheckGlobalBan() {
	auto steam64 = this->GetPlayer()->GetSteamId64();
	// globalapi http stuff
}
