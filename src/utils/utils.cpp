#include "utils.h"
#include <fstream>
#include <steam/isteamgameserver.h>
#include <core/interfaces.h>
#include <public/iserver.h>
#include <sdk/entity/ccsplayercontroller.h>
#include <sdk/entity/ccsplayerpawn.h>
#include <cs2surf.h>

std::string UTIL::GetWorkingDirectory() {
	return PATH::Join(std::filesystem::current_path().string(), "..", "..", GAME_NAME, "addons", "cs2surf");
}

std::string UTIL::GetPublicIP() {
	static std::string ipv4;
	if (!ipv4.empty()) {
		return ipv4;
	}

	auto sAddr = SteamGameServer()->GetPublicIP();
	if (!sAddr.IsSet()) {
		return std::string();
	}

	uint32_t ip = sAddr.m_unIPv4;
	std::ostringstream oss;

	// clang-format off
	oss << ((ip >> 24) & 0xFF) << "." 
		<< ((ip >> 16) & 0xFF) << "." 
		<< ((ip >>  8) & 0xFF) << "." 
		<< ((ip >>  0) & 0xFF);
	// clang-format on

	ipv4 = oss.str();
	return ipv4;
}

json UTIL::LoadJsonc(std::string sFilePath) {
	std::ifstream file(sFilePath);
	if (!file.good()) {
		return json();
	}

	return json::parse(file, nullptr, true, true);
}

std::wstring UTIL::ToWideString(const char* pszCharStr) {
	if (!pszCharStr || !pszCharStr[0]) {
		return std::wstring();
	}

	auto size_needed = WIN_LINUX(MultiByteToWideChar(CP_UTF8, 0, pszCharStr, -1, NULL, 0), mbstowcs(NULL, pszCharStr, 0));
	if (size_needed > 0) {
		std::wstring wSayContent;
		wSayContent.resize(size_needed, L'\0');
#ifdef _WIN32
		MultiByteToWideChar(CP_UTF8, 0, pszCharStr, -1, &wSayContent[0], size_needed);
#else
		mbstowcs(&wSayContent[0], pszCharStr, size_needed);
#endif
		return wSayContent;
	}

	return std::wstring();
}

CGlobalVars* UTIL::GetGlobals() {
	INetworkGameServer* server = g_pNetworkServerService->GetIGameServer();

	if (!server) {
		return nullptr;
	}

	return server->GetGlobals();
}

CGlobalVars* UTIL::GetServerGlobals() {
	return SurfPlugin()->simulatingPhysics ? &(SurfPlugin()->serverGlobals) : GetGlobals();
};

CBasePlayerController* UTIL::GetController(CBaseEntity* entity) {
	CCSPlayerController* controller = nullptr;
	if (!V_stricmp(entity->GetClassname(), "observer")) {
		CBasePlayerPawn* pawn = static_cast<CBasePlayerPawn*>(entity);
		if (!pawn->m_hController()->IsValid() || pawn->m_hController()->Get() == 0) {
			for (i32 i = 0; i <= GetServerGlobals()->maxClients; i++) {
				controller = (CCSPlayerController*)UTIL::GetController(CPlayerSlot(i));
				if (controller && controller->m_hObserverPawn() && controller->m_hObserverPawn().Get() == entity) {
					return controller;
				}
			}
			return nullptr;
		}
		return pawn->m_hController()->Get();
	}
	if (entity->IsPawn()) {
		CBasePlayerPawn* pawn = static_cast<CBasePlayerPawn*>(entity);
		if (!pawn->m_hController()->IsValid() || pawn->m_hController()->Get() == 0) {
			// Seems like the pawn lost its controller, we can try looping through the controllers to find this pawn instead.
			for (i32 i = 0; i <= GetServerGlobals()->maxClients; i++) {
				controller = (CCSPlayerController*)UTIL::GetController(CPlayerSlot(i));
				if (controller && controller->m_hPlayerPawn() && controller->m_hPlayerPawn().Get() == entity) {
					return controller;
				}
			}
			return nullptr;
		}
		return pawn->m_hController()->Get();
	} else if (entity->IsController()) {
		return static_cast<CBasePlayerController*>(entity);
	} else {
		return nullptr;
	}
}

CBasePlayerController* UTIL::GetController(CPlayerSlot slot) {
	if (!GameEntitySystem() || slot.Get() < 0 || slot.Get() > MAXPLAYERS) {
		return nullptr;
	}
	CBaseEntity* ent = static_cast<CBaseEntity*>(GameEntitySystem()->GetEntityInstance(CEntityIndex(slot.Get() + 1)));
	if (!ent) {
		return nullptr;
	}
	return ent->IsController() ? static_cast<CBasePlayerController*>(ent) : nullptr;
}
