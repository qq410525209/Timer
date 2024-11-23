#include "memory.h"
#include "gameconfig.h"
#include <sdk/common.h>
#include <libmem/libmem_helper.h>
#include <core/forwards.h>

#include <sdk/usercmd.h>
#include <sdk/entity/services.h>
#include <sdk/entity/ccsplayerpawn.h>

template<>
CSurfForward* CForwardBase<CSurfForward>::m_pFirst = nullptr;

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
	return MEM::SDKCall<ReturnTypeValue>(fnCurrent, __VA_ARGS__);

#define CALL_ADDRESS(sig, fnCurrent, ...) \
	static auto fnSig = g_pGameConfig->GetAddress(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnCurrent, __VA_ARGS__);

#define HOOK_SIG(sig, fnHook, fnTrampoline) \
	static auto fn##fnHook = g_pGameConfig->GetMemSig(sig); \
	SURF_ASSERT(fn##fnHook); \
	if (fn##fnHook) { \
		libmem::HookFunc(fn##fnHook, fnHook, g_fnMovementServicesRunCmds_Trampoline); \
	}

void MEM::CALL::SwitchTeam(CCSPlayerController* controller, int team) {
	CALL_SIG("CCSPlayerController_SwitchTeam", SwitchTeam, controller, team);
}

void MEM::CALL::SetPawn(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool a3, bool a4, bool a5) {
	CALL_SIG("CBasePlayerController_SetPawn", SetPawn, controller, pawn, a3, a4, a5);
}

void* g_fnMovementServicesRunCmds_Trampoline;

void Hook_OnMovementServicesRunCmds(CPlayer_MovementServices* pMovementServices, CUserCmd* pUserCmd) {
	CCSPlayerPawn* pawn = pMovementServices->GetPawn();
	if (!pawn) {
		MEM::SDKCall<void>(g_fnMovementServicesRunCmds_Trampoline, pMovementServices, pUserCmd);
		return;
	}

	CCSPlayerController* controller = pawn->GetController<CCSPlayerController>();
	if (!controller) {
		MEM::SDKCall<void>(g_fnMovementServicesRunCmds_Trampoline, pMovementServices, pUserCmd);
		return;
	}

	int32_t seed;
	int32_t weapon;
	int32_t cmdnum;
	int32_t tickcount;
	int32_t mouse[2] = {0, 0};

	float vec[3] = {0.0f, 0.0f, 0.0f};
	float angles[3] = {0.0f, 0.0f, 0.0f};

	CInButtonStatePB* buttons_state = nullptr;
	CBaseUserCmdPB* baseCmd = pUserCmd->mutable_base();
	if (baseCmd) {
		weapon = baseCmd->weaponselect();
		cmdnum = baseCmd->legacy_command_number();
		tickcount = baseCmd->client_tick();
		seed = baseCmd->random_seed();
		mouse[0] = baseCmd->mousedx();
		mouse[1] = baseCmd->mousedy();
		vec[0] = baseCmd->leftmove();
		vec[1] = baseCmd->forwardmove();
		vec[2] = baseCmd->upmove();
		angles[0] = baseCmd->viewangles().x();
		angles[1] = baseCmd->viewangles().y();
		angles[2] = baseCmd->viewangles().z();
		buttons_state = baseCmd->mutable_buttons_pb();
	}

	CInButtonState* button = (CInButtonState*)buttons_state;

	bool block = false;
	for (auto p = CSurfForward::m_pFirst; p; p = p->m_pNext) {
		if (!p->OnPlayerRunCmd(pawn, button, vec, angles, weapon, cmdnum, tickcount, seed, mouse)) {
			block = true;
		}
	}

	if (baseCmd) {
		baseCmd->set_weaponselect(weapon);
		baseCmd->set_legacy_command_number(cmdnum);
		baseCmd->set_client_tick(tickcount);
		baseCmd->set_random_seed(seed);
		baseCmd->set_mousedx(mouse[0]);
		baseCmd->set_mousedy(mouse[1]);
		baseCmd->set_leftmove(vec[0]);
		baseCmd->set_forwardmove(vec[1]);
		baseCmd->set_upmove(vec[2]);

		if (baseCmd->has_viewangles()) {
			CMsgQAngle* viewangles = baseCmd->mutable_viewangles();
			if (viewangles) {
				viewangles->set_x(angles[0]);
				viewangles->set_y(angles[1]);
				viewangles->set_z(angles[2]);
			}
		}

		if (baseCmd->has_buttons_pb()) {
			CInButtonStatePB* buttons_pb = baseCmd->mutable_buttons_pb();
			if (buttons_pb) {
				buttons_pb->set_buttonstate1(button->pressed);
				buttons_pb->set_buttonstate2(button->changed);
				buttons_pb->set_buttonstate3(button->scroll);
			}
		}
	}

	if (!block) {
		MEM::SDKCall<void>(g_fnMovementServicesRunCmds_Trampoline, pMovementServices, pUserCmd);
	}

	for (auto p = CSurfForward::m_pFirst; p; p = p->m_pNext) {
		p->OnPlayerRunCmdPost(pawn, button, vec, angles, weapon, cmdnum, tickcount, seed, mouse);
	}
}

static bool SetupDetours() {
	HOOK_SIG("CPlayer_MovementServices::RunCmds", Hook_OnMovementServicesRunCmds, g_fnMovementServicesRunCmds_Trampoline);

	return true;
}

static bool SetupVMTHooks() {
	return true;
}

void MEM::SetupHooks() {
	SURF_ASSERT(SetupDetours());
	SURF_ASSERT(SetupVMTHooks());
}
