#pragma once

#include <pch.h>
#include <utils/typehelper.h>

// ======================== //
using SDKHookRet_Pre = bool;
using SDKHookRet_Post = void;

#define SDKHOOK_BIND(eType, fnType) \
	template<> \
	struct SDKHookBindings<eType> { \
		using Type = fnType; \
	};

#define SDKHOOK_PRE(fnType)  RebindFunction_t<fnType, SDKHookRet_Pre>
#define SDKHOOK_POST(fnType) RebindFunction_t<fnType, SDKHookRet_Post>
// ======================== //

using HookTouch_t = void (*)(CBaseEntity* pSelf, CBaseEntity* pOther);
using HookTeleport_t = void (*)(CBaseEntity* pSelf, const Vector* newPosition, const QAngle* newAngles, const Vector* newVelocity);

enum SDKHookType {
	SDKHook_StartTouch,
	SDKHook_StartTouchPost,
	SDKHook_Touch,
	SDKHook_TouchPost,
	SDKHook_EndTouch,
	SDKHook_EndTouchPost,
	SDKHook_Teleport,
	SDKHook_TeleportPost,
	MAX_TYPE
};

namespace SDKHOOK {
	template<SDKHookType T>
	struct SDKHookBindings;

	SDKHOOK_BIND(SDKHook_StartTouch, SDKHOOK_PRE(HookTouch_t));
	SDKHOOK_BIND(SDKHook_StartTouchPost, SDKHOOK_POST(HookTouch_t));
	SDKHOOK_BIND(SDKHook_Touch, SDKHOOK_PRE(HookTouch_t));
	SDKHOOK_BIND(SDKHook_TouchPost, SDKHOOK_POST(HookTouch_t));
	SDKHOOK_BIND(SDKHook_EndTouch, SDKHOOK_PRE(HookTouch_t));
	SDKHOOK_BIND(SDKHook_EndTouchPost, SDKHOOK_POST(HookTouch_t));
	SDKHOOK_BIND(SDKHook_Teleport, SDKHOOK_PRE(HookTeleport_t));
	SDKHOOK_BIND(SDKHook_TeleportPost, SDKHOOK_POST(HookTeleport_t));

	template<SDKHookType T>
	bool HookEntity(CBaseEntity* pEnt, typename SDKHookBindings<T>::Type pFn);

	// vtable for key
	inline std::unordered_map<void*, std::set<uint32_t>> m_umVFuncHookMarks;
	inline std::unordered_map<void*, std::set<void*>> m_umSDKHooks[SDKHookType::MAX_TYPE];
	inline std::unordered_map<void*, void*> m_umSDKHookTrampolines[SDKHookType::MAX_TYPE];
} // namespace SDKHOOK
