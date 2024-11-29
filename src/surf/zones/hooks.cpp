#include <sdk/entity/cbaseentity.h>
#include <utils/print.h>
#include <core/memory.h>

void Hook_OnStartTouchPost(CBaseEntity* pOther) {
	UTIL::PrintChatAll("start touch!\n");
}

void Hook_OnTouchPost(CBaseEntity* pOther) {}

void Hook_OnEndTouchPost(CBaseEntity* pOther) {
	UTIL::PrintChatAll("end touch!\n");
}
