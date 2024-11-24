#include "memory.h"
#include <core/gamedata.h>
#include <core/interfaces.h>

#include <sdk/usercmd.h>
#include <sdk/entity/services.h>
#include <sdk/entity/ccsplayerpawn.h>
#include <utils/ctimer.h>
#include <utils/utils.h>

PLUGIN_GLOBALVARS();

template<typename T>
struct ReturnType;

template<typename Ret, typename... Args>
struct ReturnType<Ret (*)(Args...)> {
	using type = Ret;
};

#define CALL_SIG(sig, fnCurrent, ...) \
	static auto fnSig = GAMEDATA::GetMemSig(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnSig, __VA_ARGS__);

#define CALL_ADDRESS(sig, fnCurrent, ...) \
	static auto fnSig = GAMEDATA::GetAddress(sig); \
	SURF_ASSERT(fnSig); \
	using FunctionType = decltype(&fnCurrent); \
	using ReturnTypeValue = ReturnType<FunctionType>::type; \
	return MEM::SDKCall<ReturnTypeValue>(fnSig, __VA_ARGS__);

#define HOOK_SIG(sig, fnHook, fnTrampoline) \
	static auto fn##fnHook = GAMEDATA::GetMemSig(sig); \
	SURF_ASSERT(fn##fnHook); \
	if (fn##fnHook) { \
		libmem::HookFunc(fn##fnHook, fnHook, fnTrampoline); \
	}

#define HOOK_VMT(gdOffsetKey, pModule, fnHook, fnTrampoline) \
	SURF_ASSERT(MEM::VmtHookEx(GAMEDATA::GetOffset(gdOffsetKey), pModule.get(), gdOffsetKey, fnHook, fnTrampoline));

void MEM::CALL::SwitchTeam(CCSPlayerController* controller, int team) {
	CALL_SIG("CCSPlayerController_SwitchTeam", SwitchTeam, controller, team);
}

void MEM::CALL::SetPawn(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool a3, bool a4, bool a5) {
	CALL_SIG("CBasePlayerController_SetPawn", SetPawn, controller, pawn, a3, a4, a5);
}

IGameEventListener2* MEM::CALL::GetLegacyGameEventListener(CPlayerSlot slot) {
	CALL_SIG("GetLegacyGameEventListener", GetLegacyGameEventListener, slot);
}

bool MEM::CALL::TraceShape(const Ray_t& ray, const Vector& vecStart, const Vector& vecEnd, const CTraceFilter& filter, CGameTrace* tr) {
	CALL_SIG("TraceShape", TraceShape, IFACE::pGameTraceService, &ray, &vecStart, &vecEnd, &filter, tr);
}

#pragma region hooks

static void Hook_OnMovementServicesRunCmds(CPlayer_MovementServices* pMovementServices, CUserCmd* pUserCmd) {
	CCSPlayerPawn* pawn = pMovementServices->GetPawn();
	if (!pawn) {
		MEM::SDKCall<void>(MEM::g_fnMovementServicesRunCmds_Trampoline, pMovementServices, pUserCmd);
		return;
	}

	CCSPlayerController* controller = pawn->GetController<CCSPlayerController>();
	if (!controller) {
		MEM::SDKCall<void>(MEM::g_fnMovementServicesRunCmds_Trampoline, pMovementServices, pUserCmd);
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
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
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
		MEM::SDKCall<void>(MEM::g_fnMovementServicesRunCmds_Trampoline, pMovementServices, pUserCmd);
	}

	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnPlayerRunCmdPost(pawn, button, vec, angles, weapon, cmdnum, tickcount, seed, mouse);
	}
}

// clang-format off
SH_DECL_HOOK1_void(IGameSystem, ServerGamePostSimulate, SH_NOATTRIB, false, const EventServerGamePostSimulate_t*);
static void Hook_OnServerGamePostSimulate(const EventServerGamePostSimulate_t* a2) {
	UTIL::ProcessTimers();
}

SH_DECL_HOOK3_void(ISource2Server, GameFrame, SH_NOATTRIB, false, bool, bool, bool);
static void Hook_OnGameFrame(bool simulating, bool bFirstTick, bool bLastTick) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnGameFrame(META_IFACEPTR(ISource2Server), simulating, bFirstTick, bLastTick);
	}
}

SH_DECL_HOOK6(ISource2GameClients, ClientConnect, SH_NOATTRIB, false, bool, CPlayerSlot, const char*, uint64, const char*, bool, CBufferString*);
static bool Hook_ClientConnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1,
							   CBufferString* pRejectReason) {
	RETURN_META_VALUE(MRES_IGNORED, true);
}

SH_DECL_HOOK6_void(ISource2GameClients, OnClientConnected, SH_NOATTRIB, false, CPlayerSlot, const char *, uint64, const char *, const char *, bool);
static void Hook_OnClientConnected(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, const char* pszAddress,
								   bool bFakePlayer) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientConnected(META_IFACEPTR(ISource2GameClients), slot, pszName, xuid, pszNetworkID, pszAddress, bFakePlayer);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK1_void(ISource2GameClients, ClientFullyConnect, SH_NOATTRIB, false, CPlayerSlot);
static void Hook_ClientFullyConnect(CPlayerSlot slot) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientFullyConnect(META_IFACEPTR(ISource2GameClients), slot);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK4_void(ISource2GameClients, ClientPutInServer, SH_NOATTRIB, false, CPlayerSlot, char const *, int, uint64);
static void Hook_ClientPutInServer(CPlayerSlot slot, char const* pszName, int type, uint64 xuid) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientPutInServer(META_IFACEPTR(ISource2GameClients), slot, pszName, type, xuid);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK4_void(ISource2GameClients, ClientActive, SH_NOATTRIB, false, CPlayerSlot, bool, const char *, uint64);
static void Hook_ClientActive(CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientActive(META_IFACEPTR(ISource2GameClients), slot, bLoadGame, pszName, xuid);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK5_void(ISource2GameClients, ClientDisconnect, SH_NOATTRIB, false, CPlayerSlot, ENetworkDisconnectionReason, const char *, uint64, const char *);
static void Hook_ClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientDisconnect(META_IFACEPTR(ISource2GameClients), slot, reason, pszName, xuid, pszNetworkID);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK1_void(ISource2GameClients, ClientVoice, SH_NOATTRIB, false, CPlayerSlot);
static void Hook_ClientVoice(CPlayerSlot slot) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientVoice(META_IFACEPTR(ISource2GameClients), slot);
	}
}

SH_DECL_HOOK2_void(ISource2GameClients, ClientCommand, SH_NOATTRIB, false, CPlayerSlot, const CCommand &);
static void Hook_ClientCommand(CPlayerSlot slot, const CCommand& args) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnClientCommand(META_IFACEPTR(ISource2GameClients), slot, args);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t &, ISource2WorldSession *, const char *);
static void Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnStartupServer(META_IFACEPTR(INetworkServerService), config);
	}

	RETURN_META(MRES_IGNORED);
}

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, false, bool, IGameEvent *, bool);
static bool Hook_FireEvent(IGameEvent* pEvent, bool bDontBroadcast) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnFireEvent(META_IFACEPTR(IGameEventManager2), pEvent, bDontBroadcast);
	}

	RETURN_META_VALUE(MRES_IGNORED, true);
}

SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandHandle, const CCommandContext &, const CCommand &);
static void Hook_DispatchConCommand(ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnDispatchConCommand(META_IFACEPTR(ICvar), cmd, ctx, args);
	}
}

SH_DECL_HOOK8_void(IGameEventSystem, PostEventAbstract, SH_NOATTRIB, 0, CSplitScreenSlot, bool, int, const uint64 *, INetworkMessageInternal *, const CNetMessage *, unsigned long, NetChannelBufType_t);
static void Hook_PostEvent(CSplitScreenSlot nSlot, bool bLocalOnly, int nClientCount, const uint64* clients, INetworkMessageInternal* pEvent,
						   const CNetMessage* pData, unsigned long nSize, NetChannelBufType_t bufType) {
	for (auto p = CCoreForward::m_pFirst; p; p = p->m_pNext) {
		p->OnPostEventAbstract(META_IFACEPTR(IGameEventSystem), nSlot, nClientCount, clients, pEvent, pData);
	}
}

// clang-format on
#pragma endregion

#pragma region setup

static bool SetupDetours() {
	// clang-format off
	HOOK_SIG("CPlayer_MovementServices::RunCmds", Hook_OnMovementServicesRunCmds, MEM::g_fnMovementServicesRunCmds_Trampoline);
	// clang-format on

	return true;
}

static bool SetupVMTHooks() {
	// clang-format off
	// HOOK_VMT("CEntityDebugGameSystem::ServerGamePostSimulate", MEM::MODULE::server, Hook_OnServerGamePostSimulate, MEM::g_fnServerGamePostSimulate_Trampoline);

	// clang-format on
	return true;
}

static bool SetupSourceHooks() {
	SH_ADD_HOOK(ISource2Server, GameFrame, IFACE::pServer, SH_STATIC(Hook_OnGameFrame), false);

	SH_ADD_HOOK(ISource2GameClients, ClientConnect, g_pSource2GameClients, SH_STATIC(Hook_ClientConnect), false);
	SH_ADD_HOOK(ISource2GameClients, OnClientConnected, g_pSource2GameClients, SH_STATIC(Hook_OnClientConnected), false);
	SH_ADD_HOOK(ISource2GameClients, ClientFullyConnect, g_pSource2GameClients, SH_STATIC(Hook_ClientFullyConnect), false);
	SH_ADD_HOOK(ISource2GameClients, ClientPutInServer, g_pSource2GameClients, SH_STATIC(Hook_ClientPutInServer), false);
	SH_ADD_HOOK(ISource2GameClients, ClientActive, g_pSource2GameClients, SH_STATIC(Hook_ClientActive), false);
	SH_ADD_HOOK(ISource2GameClients, ClientDisconnect, g_pSource2GameClients, SH_STATIC(Hook_ClientDisconnect), true);
	SH_ADD_HOOK(ISource2GameClients, ClientVoice, g_pSource2GameClients, SH_STATIC(Hook_ClientVoice), false);
	SH_ADD_HOOK(ISource2GameClients, ClientCommand, g_pSource2GameClients, SH_STATIC(Hook_ClientCommand), false);

	SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_STATIC(Hook_StartupServer), true);

	SH_ADD_HOOK(IGameEventManager2, FireEvent, IFACE::pGameEventManager, SH_STATIC(Hook_FireEvent), false);

	SH_ADD_HOOK(ICvar, DispatchConCommand, g_pCVar, SH_STATIC(Hook_DispatchConCommand), false);

	SH_ADD_HOOK(IGameEventSystem, PostEventAbstract, IFACE::pGameEventSystem, SH_STATIC(Hook_PostEvent), false);

	// clang-format off
	SH_ADD_DVPHOOK(IGameSystem, 
		ServerGamePostSimulate, 
		MEM::MODULE::server->GetVirtualTableByName("CEntityDebugGameSystem").RCast<IGameSystem*>(), 
		SH_STATIC(Hook_OnServerGamePostSimulate), 
		true
	);
	// clang-format on
	return true;
}

void MEM::SetupHooks() {
	SURF_ASSERT(SetupDetours());
	SURF_ASSERT(SetupVMTHooks());
	SURF_ASSERT(SetupSourceHooks());
}

void MEM::MODULE::Setup() {
	engine = std::make_shared<libmodule::CModule>();
	engine->InitFromName(LIB::engine2, true);

	tier0 = std::make_shared<libmodule::CModule>();
	tier0->InitFromName(LIB::tier0, true);

	server = std::make_shared<libmodule::CModule>();
	server->InitFromMemory(libmem::GetModule(LIB::server).base);

	schemasystem = std::make_shared<libmodule::CModule>();
	schemasystem->InitFromName(LIB::schemasystem, true);

	steamnetworkingsockets = std::make_shared<libmodule::CModule>();
	steamnetworkingsockets->InitFromName(LIB::steamnetworkingsockets, true);
}

#pragma endregion
