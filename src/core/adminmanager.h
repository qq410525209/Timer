#pragma once

#include <sdk/entity/ccsplayercontroller.h>

/**
 * Access levels (flags) for admins.
 */
enum class AdminFlag : uint32_t {
	None = 0,           /* No permissions */
	Generic = 1 << 0,   /* a: Generic admin abilities */
	Kick = 1 << 1,      /* b: Kick another user */
	Ban = 1 << 2,       /* c: Ban another user */
	Unban = 1 << 3,     /* d: Unban another user */
	Slay = 1 << 4,      /* e: Slay/kill/damage another user */
	Changemap = 1 << 5, /* f: Change the map */
	Convars = 1 << 6,   /* g: Change basic convars */
	Config = 1 << 7,    /* h: Change configuration */
	Chat = 1 << 8,      /* i: Special chat privileges */
	Vote = 1 << 9,      /* j: Special vote privileges */
	Password = 1 << 10, /* k: Set a server password */
	RCON = 1 << 11,     /* l: Use RCON */
	Cheats = 1 << 12,   /* m: Change sv_cheats and use its commands */
	Root = 1 << 13      /* z: All access by default */
};

inline AdminFlag operator|(AdminFlag a, AdminFlag b) {
	return static_cast<AdminFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline AdminFlag operator&(AdminFlag a, AdminFlag b) {
	return static_cast<AdminFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline AdminFlag operator~(AdminFlag a) {
	return static_cast<AdminFlag>(~static_cast<uint32_t>(a));
}

struct CAdminInfo {
	uint64 m_iSteamID;
	AdminFlag m_nFlag;
};

namespace ADMIN {
	bool IsAdmin(uint64 xuid);
	bool IsAdmin(CCSPlayerController* controller);
	CAdminInfo GetAdmin(uint64 xuid);
	CAdminInfo GetAdmin(CCSPlayerController* controller);
	bool CheckAccess(uint64 xuid, AdminFlag flag);
	bool CheckAccess(CCSPlayerController* controller, AdminFlag flag);
	void AddAdmin(uint64 xuid, std::string nick, AdminFlag flag);

	inline std::unordered_map<uint64, CAdminInfo> m_umAdmins;
} // namespace ADMIN
