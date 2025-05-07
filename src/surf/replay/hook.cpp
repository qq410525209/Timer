#include "surf_replay.h"

namespace SURF::REPLAY::HOOK {
	bool OnBotTrigger(CBaseEntity* pSelf, CBaseEntity* pOther) {
		auto pPawn = reinterpret_cast<CBasePlayerPawn*>(pOther);
		if (!pPawn->IsPawn()) {
			return true;
		}

		if (pPawn->IsBot()) {
			return false;
		}

		return true;
	}
} // namespace SURF::REPLAY::HOOK
