#include "cbaseplayerpawn.h"
#include <libmem/libmem_virtual.h>

void CBasePlayerPawn::CommitSuicide(bool bExplode, bool bForce) {
	this->m_bTakesDamage(true);
	CALL_VIRTUAL(void, GAMEDATA::GetOffset("CommitSuicide"), this, bExplode, bForce);
	this->m_bTakesDamage(false);
}
