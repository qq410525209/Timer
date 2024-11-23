#include "cgameresourceservice.h"

#include <core/gameconfig.h>

CGameEntitySystem* CGameResourceService::GetGameEntitySystem() {
	return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(this) + GAMEDATA::GetOffset("GameEntitySystem"));
}
