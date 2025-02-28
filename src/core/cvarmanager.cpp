#include "cvarmanager.h"
#include <pch.h>
#include <utils/utils.h>

std::optional<ConVarRefAbstract> CVAR::Find(const char* name) {
	ConVarRefAbstract cvarRef = g_pCVar->FindConVar(name, true);
	if (!cvarRef.IsValidRef()) {
		return std::nullopt;
	}

	return cvarRef;
}

void ReplicateConVarValue(const char* name, const char* value, CPlayerSlot slot) {
	UTIL::SendConVarValue(slot, name, value);
}
