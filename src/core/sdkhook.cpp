#include "sdkhook.h"
#include <libmem/libmem_helper.h>
#include <ranges>

#define SDKHOOK_POSTBIND(eType, fnType) template bool SDKHOOK::HookEntity<eType>(CBaseEntity * pEnt, fnType pFn);

SDKHOOK_POSTBIND(SDKHook_StartTouch, SDKHOOK_PRE(HookTouch_t));
SDKHOOK_POSTBIND(SDKHook_StartTouchPost, SDKHOOK_POST(HookTouch_t));
SDKHOOK_POSTBIND(SDKHook_Touch, SDKHOOK_PRE(HookTouch_t));
SDKHOOK_POSTBIND(SDKHook_TouchPost, SDKHOOK_POST(HookTouch_t));
SDKHOOK_POSTBIND(SDKHook_EndTouch, SDKHOOK_PRE(HookTouch_t));
SDKHOOK_POSTBIND(SDKHook_EndTouchPost, SDKHOOK_POST(HookTouch_t));
SDKHOOK_POSTBIND(SDKHook_Teleport, SDKHOOK_PRE(HookTeleport_t));
SDKHOOK_POSTBIND(SDKHook_TeleportPost, SDKHOOK_POST(HookTeleport_t));

static bool IsVMTHooked(void* pVtable, uint32_t iOffset) {
	auto range = SDKHOOK::m_ummVtableHooks.equal_range(pVtable);
	return std::ranges::any_of(range.first, range.second, [iOffset](const auto& pair) { return pair.second == iOffset; });
}

static void Hook_OnStartTouch(CBaseEntity* pEnt, void* pCallback, bool post) {
	static HookTouch_t pTrampoline = nullptr;
	HookTouch_t pHook = [](CBaseEntity* pSelf, CBaseEntity* pOther) {
		void* vtable = *(void**)pSelf;

		bool block = false;
		const auto& preHooks = SDKHOOK::m_umSDKHooks[SDKHook_StartTouch][vtable];
		for (const auto& pFnPre : preHooks) {
			if (!reinterpret_cast<SDKHOOK_PRE(HookTouch_t)*>(pFnPre)(pSelf, pOther)) {
				block = true;
			}
		}
		if (block) {
			return;
		}

		pTrampoline(pSelf, pOther);

		const auto& postHooks = SDKHOOK::m_umSDKHooks[SDKHook_StartTouchPost][vtable];
		for (const auto& pFnPost : postHooks) {
			reinterpret_cast<SDKHOOK_POST(HookTouch_t)*>(pFnPost)(pSelf, pOther);
		}
	};

	static int iOffset = GAMEDATA::GetOffset("CBaseEntity::StartTouch");
	void* pVtable = *(void**)pEnt;
	if (post) {
		SDKHOOK::m_umSDKHooks[SDKHook_StartTouch][pVtable].insert(pCallback);
	} else {
		SDKHOOK::m_umSDKHooks[SDKHook_StartTouchPost][pVtable].insert(pCallback);
	}

	if (!IsVMTHooked(pVtable, iOffset)) {
		libmem::VmtHookEx(pEnt, iOffset, pHook, (void*&)pTrampoline);
		SDKHOOK::m_ummVtableHooks.insert({pVtable, iOffset});
	}
}

static void Hook_OnTouch(CBaseEntity* pEnt, void* pCallback, bool post) {
	static HookTouch_t pTrampoline = nullptr;
	HookTouch_t pHook = [](CBaseEntity* pSelf, CBaseEntity* pOther) {
		void* vtable = *(void**)pSelf;

		bool block = false;
		const auto& preHooks = SDKHOOK::m_umSDKHooks[SDKHook_Touch][vtable];
		for (const auto& pFnPre : preHooks) {
			if (!reinterpret_cast<SDKHOOK_PRE(HookTouch_t)*>(pFnPre)(pSelf, pOther)) {
				block = true;
			}
		}
		if (block) {
			return;
		}

		pTrampoline(pSelf, pOther);

		const auto& postHooks = SDKHOOK::m_umSDKHooks[SDKHook_TouchPost][vtable];
		for (const auto& pFnPost : postHooks) {
			reinterpret_cast<SDKHOOK_POST(HookTouch_t)*>(pFnPost)(pSelf, pOther);
		}
	};

	static int iOffset = GAMEDATA::GetOffset("CBaseEntity::Touch");
	void* pVtable = *(void**)pEnt;
	if (post) {
		SDKHOOK::m_umSDKHooks[SDKHook_Touch][pVtable].insert(pCallback);
	} else {
		SDKHOOK::m_umSDKHooks[SDKHook_TouchPost][pVtable].insert(pCallback);
	}

	if (!IsVMTHooked(pVtable, iOffset)) {
		libmem::VmtHookEx(pEnt, iOffset, pHook, (void*&)pTrampoline);
		SDKHOOK::m_ummVtableHooks.insert({pVtable, iOffset});
	}
}

static void Hook_OnEndTouch(CBaseEntity* pEnt, void* pCallback, bool post) {
	static HookTouch_t pTrampoline = nullptr;
	HookTouch_t pHook = [](CBaseEntity* pSelf, CBaseEntity* pOther) {
		void* vtable = *(void**)pSelf;

		bool block = false;
		const auto& preHooks = SDKHOOK::m_umSDKHooks[SDKHook_EndTouch][vtable];
		for (const auto& pFnPre : preHooks) {
			if (!reinterpret_cast<SDKHOOK_PRE(HookTouch_t)*>(pFnPre)(pSelf, pOther)) {
				block = true;
			}
		}
		if (block) {
			return;
		}

		pTrampoline(pSelf, pOther);

		const auto& postHooks = SDKHOOK::m_umSDKHooks[SDKHook_EndTouchPost][vtable];
		for (const auto& pFnPost : postHooks) {
			reinterpret_cast<SDKHOOK_POST(HookTouch_t)*>(pFnPost)(pSelf, pOther);
		}
	};

	static int iOffset = GAMEDATA::GetOffset("CBaseEntity::EndTouch");
	void* pVtable = *(void**)pEnt;
	if (post) {
		SDKHOOK::m_umSDKHooks[SDKHook_EndTouch][pVtable].insert(pCallback);
	} else {
		SDKHOOK::m_umSDKHooks[SDKHook_EndTouchPost][pVtable].insert(pCallback);
	}

	if (!IsVMTHooked(pVtable, iOffset)) {
		libmem::VmtHookEx(pEnt, iOffset, pHook, (void*&)pTrampoline);
		SDKHOOK::m_ummVtableHooks.insert({pVtable, iOffset});
	}
}

static void Hook_OnTeleport(CBaseEntity* pEnt, void* pCallback, bool post) {
	static HookTeleport_t pTrampoline = nullptr;
	HookTeleport_t pHook = [](CBaseEntity* pSelf, const Vector* newPosition, const QAngle* newAngles, const Vector* newVelocity) {
		void* vtable = *(void**)pSelf;

		bool block = false;
		const auto& preHooks = SDKHOOK::m_umSDKHooks[SDKHook_Teleport][vtable];
		for (const auto& pFnPre : preHooks) {
			if (!reinterpret_cast<SDKHOOK_PRE(HookTeleport_t)*>(pFnPre)(pSelf, newPosition, newAngles, newVelocity)) {
				block = true;
			}
		}
		if (block) {
			return;
		}

		pTrampoline(pSelf, newPosition, newAngles, newVelocity);

		const auto& postHooks = SDKHOOK::m_umSDKHooks[SDKHook_TeleportPost][vtable];
		for (const auto& pFnPost : postHooks) {
			reinterpret_cast<SDKHOOK_POST(HookTeleport_t)*>(pFnPost)(pSelf, newPosition, newAngles, newVelocity);
		}
	};

	static int iOffset = GAMEDATA::GetOffset("Teleport");
	void* pVtable = *(void**)pEnt;
	if (post) {
		SDKHOOK::m_umSDKHooks[SDKHook_Teleport][pVtable].insert(pCallback);
	} else {
		SDKHOOK::m_umSDKHooks[SDKHook_TeleportPost][pVtable].insert(pCallback);
	}

	if (!IsVMTHooked(pVtable, iOffset)) {
		libmem::VmtHookEx(pEnt, iOffset, pHook, (void*&)pTrampoline);
		SDKHOOK::m_ummVtableHooks.insert({pVtable, iOffset});
	}
}

template<SDKHookType T>
bool SDKHOOK::HookEntity(CBaseEntity* pEnt, typename SDKHookBindings<T>::Type pFn) {
	switch (T) {
		case SDKHook_StartTouch:
			::Hook_OnStartTouch(pEnt, (void*)pFn, false);
			break;
		case SDKHook_StartTouchPost:
			::Hook_OnStartTouch(pEnt, (void*)pFn, true);
			break;
		case SDKHook_Touch:
			::Hook_OnTouch(pEnt, (void*)pFn, false);
			break;
		case SDKHook_TouchPost:
			::Hook_OnTouch(pEnt, (void*)pFn, true);
			break;
		case SDKHook_EndTouch:
			::Hook_OnEndTouch(pEnt, (void*)pFn, false);
			break;
		case SDKHook_EndTouchPost:
			::Hook_OnEndTouch(pEnt, (void*)pFn, true);
			break;
		case SDKHook_Teleport:
			::Hook_OnTeleport(pEnt, (void*)pFn, false);
			break;
		case SDKHook_TeleportPost:
			::Hook_OnTeleport(pEnt, (void*)pFn, true);
			break;
		default:
			return false;
	}

	return true;
}
