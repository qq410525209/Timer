#include "cbaseplayercontroller.h"
#include <core/memory.h>
#include <utils/utils.h>

bool CBasePlayerController::IsObserver() {
	auto pPawn = GetCurrentPawn();
	if (!pPawn) {
		SDK_ASSERT(false);
	}

	return pPawn->IsObserver();
}

void CBasePlayerController::SetPawn(CCSPlayerPawn* pawn) {
	MEM::CALL::SetPawn(this, pawn, true, false, false);
}

void CBasePlayerController::SendFullUpdate() {
	CServerSideClient* pClient = UTIL::GetClientBySlot(GetPlayerSlot());
	if (pClient) {
		pClient->ForceFullUpdate();
	}
}
