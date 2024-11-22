#include "memory.h"
#include "gameconfig.h"
#include <sdk/common.h>
#include <libmem/libmem_helper.h>

template<typename T>
struct ReturnType;

template<typename Ret, typename... Args>
struct ReturnType<Ret (*)(Args...)> {
	using type = Ret;
};

#define CALL_SIG(sig, fnCurrent, ...) \
	static auto fnSig = g_pGameConfig->GetMemSig(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return SDKCall<ReturnTypeValue>(fnCurrent, __VA_ARGS__);

#define CALL_ADDRESS(sig, fnCurrent, ...) \
	static auto fnSig = g_pGameConfig->GetAddress(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return SDKCall<ReturnTypeValue>(fnCurrent, __VA_ARGS__);

void MEM::CALL::SwitchTeam(CCSPlayerController* controller, int team) {
	CALL_SIG("CCSPlayerController_SwitchTeam", SwitchTeam, controller, team);
}

void MEM::CALL::SetPawn(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool a3, bool a4, bool a5) {
	CALL_SIG("CBasePlayerController_SetPawn", SetPawn, controller, pawn, a3, a4, a5);
}

static bool SetupDetours() {
	return true;
}

static bool SetupVMTHooks() {
	return true;
}

void MEM::SetupHooks() {
	SURF_ASSERT(SetupDetours());
	SURF_ASSERT(SetupVMTHooks());
}
