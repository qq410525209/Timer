#include "cbaseplayercontroller.h"
#include <core/memory.h>

void CBasePlayerController::SetPawn(CCSPlayerPawn* pawn) {
	MEM::CALL::SetPawn(this, pawn, true, false, false);
}
