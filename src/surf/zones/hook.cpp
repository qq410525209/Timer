#include "surf_zones.h"
#include <utils/utils.h>
#include <surf/api.h>

#define _ZONE_DEBUG

namespace SURF::ZONE::HOOK {
	bool OnStartTouch(CBaseEntity* pSelf, CBaseEntity* pOther) {
		if (!pOther->IsPawn()) {
			return true;
		}

		// if not our zone, ignore endtouch fix
		auto res = SURF::ZonePlugin()->FindZone(pSelf);
		if (!res.has_value()) {
			return true;
		}

		auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
		if (!player) {
			return true;
		}

		if (player->m_bJustTeleported) {
			auto hTrigger = pSelf->GetRefEHandle();
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

	void OnStartTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
		if (!pOther->IsPawn()) {
			return;
		}

		auto res = SURF::ZonePlugin()->FindZone(pSelf);
		if (!res.has_value()) {
			return;
		}

		auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
		if (!player) {
			return;
		}

		auto& zone = res.value();
		FORWARD_POST(CSurfForward, OnEnterZone, zone, player);

#ifdef _ZONE_DEBUG
		UTIL::PrintChatAll("start touch! self: %s, other: %s\n", pSelf->m_pEntity->m_name, pOther->GetClassname());
#endif
	}

	void OnTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
		if (!pOther->IsPawn()) {
			return;
		}

		auto res = SURF::ZonePlugin()->FindZone(pSelf);
		if (!res.has_value()) {
			return;
		}

		auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
		if (!player) {
			return;
		}

		auto& zone = res.value();
		FORWARD_POST(CSurfForward, OnStayZone, zone, player);
	}

	void OnEndTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
		if (!pOther->IsPawn()) {
			return;
		}

		auto res = SURF::ZonePlugin()->FindZone(pSelf);
		if (!res.has_value()) {
			return;
		}

		auto player = SURF::GetPlayerManager()->ToPlayer((CBasePlayerPawn*)pOther);
		if (!player) {
			return;
		}

		auto& zone = res.value();
		FORWARD_POST(CSurfForward, OnLeaveZone, zone, player);

#ifdef _ZONE_DEBUG
		UTIL::PrintChatAll("end touch!\n");
#endif
	}
} // namespace SURF::ZONE::HOOK
