#include "cbaseplayercontroller.h"
#include <core/memory.h>
#include <utils/utils.h>

void CBasePlayerController::SetPawn(CCSPlayerPawn* pawn) {
	MEM::CALL::SetPawn(this, pawn, true, false, false);
}

void CBasePlayerController::SendFullUpdate() {
	CServerSideClient* pClient = UTIL::GetClientBySlot(GetPlayerSlot());
	if (pClient) {
		pClient->ForceFullUpdate();
	}
}
