#include "cvarmanager.h"
#include <pch.h>
#include <utils/utils.h>

CConVarBaseData* CVAR::Find(const char* name) {
	ConVarHandleS2 cvarHandle = reinterpret_cast<ICvarS2*>(g_pCVar)->FindConVar(name, true);
	if (!cvarHandle.IsValid()) {
		return nullptr;
	}

	return reinterpret_cast<ICvarS2*>(g_pCVar)->GetConVar(cvarHandle);
}

void ReplicateConVarValue(const char* name, const char* value, CPlayerSlot slot) {
	UTIL::SendConVarValue(slot, name, value);
}
