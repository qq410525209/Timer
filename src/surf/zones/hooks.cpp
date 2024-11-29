#include <sdk/entity/cbaseentity.h>
#include <utils/print.h>
#include <core/memory.h>

void Hook_OnStartTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
	UTIL::PrintChatAll("start touch!\n");
}

void Hook_OnTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
	UTIL::PrintChatAll("touching!\n");
}

void Hook_OnEndTouchPost(CBaseEntity* pSelf, CBaseEntity* pOther) {
	UTIL::PrintChatAll("end touch!\n");
}
