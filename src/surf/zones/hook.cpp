#include "surf_zones.h"
#include <utils/utils.h>
#include <surf/api.h>

#define _ZONE_DEBUG

namespace SURF::ZONE::HOOK {
	bool OnStartTouch(CBaseEntity* pTrigger, CBaseEntity* pOther) {
		auto pPawn = reinterpret_cast<CBasePlayerPawn*>(pOther);
		if (!pPawn->IsPawn() || pPawn->IsBot()) {
			return true;
		}

		// if not our zone, ignore endtouch fix
		auto res = SURF::ZonePlugin()->FindZone(pTrigger);
		if (!res.has_value()) {
			return true;
		}

		auto pPlayer = SURF::GetPlayerManager()->ToPlayer(pPawn);
		if (!pPlayer) {
			return true;
		}

		if (pPlayer->m_bJustTeleported) {
			auto hTrigger = pTrigger->GetRefEHandle();
			auto hPlayer = pOther->GetRefEHandle();
			UTIL::RequestFrame([hTrigger, hPlayer] {
				auto pTrigger = (CBaseEntity*)hTrigger.Get();
				if (!pTrigger) {
					return;
				}
				auto pPlayer = (CBaseEntity*)hPlayer.Get();
				if (!pPlayer) {
					return;
				}

				OnStartTouchPost(pTrigger, pPlayer);
			});
			return false;
		}

		return true;
	}

	void OnStartTouchPost(CBaseEntity* pTrigger, CBaseEntity* pOther) {
		auto pPawn = reinterpret_cast<CBasePlayerPawn*>(pOther);
		if (!pPawn->IsPawn() || pPawn->IsBot()) {
			return;
		}

		auto res = SURF::ZonePlugin()->FindZone(pTrigger);
		if (!res.has_value()) {
			return;
		}

		auto pPlayer = SURF::GetPlayerManager()->ToPlayer(pPawn);
		if (!pPlayer) {
			return;
		}

		auto& zone = res.value();
		FORWARD_POST(CSurfForward, OnEnterZone, zone, pPlayer);

#ifdef _ZONE_DEBUG
		UTIL::PrintChatAll("start touch! self: %s, other: %s\n", pTrigger->m_pEntity->m_name, pOther->GetClassname());
#endif
	}

	void OnTouchPost(CBaseEntity* pTrigger, CBaseEntity* pOther) {
		auto pPawn = reinterpret_cast<CBasePlayerPawn*>(pOther);
		if (!pPawn->IsPawn() || pPawn->IsBot()) {
			return;
		}

		auto res = SURF::ZonePlugin()->FindZone(pTrigger);
		if (!res.has_value()) {
			return;
		}

		auto pPlayer = SURF::GetPlayerManager()->ToPlayer(pPawn);
		if (!pPlayer) {
			return;
		}

		auto& zone = res.value();
		FORWARD_POST(CSurfForward, OnStayZone, zone, pPlayer);
	}

	void OnEndTouchPost(CBaseEntity* pTrigger, CBaseEntity* pOther) {
		auto pPawn = reinterpret_cast<CBasePlayerPawn*>(pOther);
		if (!pPawn->IsPawn() || pPawn->IsBot()) {
			return;
		}

		auto res = SURF::ZonePlugin()->FindZone(pTrigger);
		if (!res.has_value()) {
			return;
		}

		auto pPlayer = SURF::GetPlayerManager()->ToPlayer(pPawn);
		if (!pPlayer) {
			return;
		}

		auto& zone = res.value();
		FORWARD_POST(CSurfForward, OnLeaveZone, zone, pPlayer);

#ifdef _ZONE_DEBUG
		UTIL::PrintChatAll("end touch!\n");
#endif
	}
} // namespace SURF::ZONE::HOOK
