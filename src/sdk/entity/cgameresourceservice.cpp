#include "cgameresourceservice.h"

#include <core/gameconfig.h>

extern CGameConfig* g_pGameConfig;

CGameEntitySystem* CGameResourceService::GetGameEntitySystem() {
	return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(this) + g_pGameConfig->GetOffset("GameEntitySystem"));
}
