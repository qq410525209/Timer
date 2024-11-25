#include "movement.h"

#include <core/gamedata.h>
#include <core/memory.h>

static void Hook_OnMovementServicesRunCmds(CPlayer_MovementServices* pMovementServices, CUserCmd* pUserCmd) {
	CCSPlayerPawn* pawn = pMovementServices->GetPawn();
	if (!pawn) {
		MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnMovementServicesRunCmds, pMovementServices, pUserCmd);
		return;
	}

	CCSPlayerController* controller = pawn->GetController<CCSPlayerController>();
	if (!controller) {
		MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnMovementServicesRunCmds, pMovementServices, pUserCmd);
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
	for (auto p = CMovementForward::m_pFirst; p; p = p->m_pNext) {
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

	if (block) {
		return;
	}

	MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnMovementServicesRunCmds, pMovementServices, pUserCmd);

	FORWARD_POST(CMovementForward, OnPlayerRunCmdPost, pawn, button, vec, angles, weapon, cmdnum, tickcount, seed, mouse);
}

static void Hook_OnTryPlayerMove(CCSPlayer_MovementServices* ms, CMoveData* mv, Vector* pFirstDest, trace_t* pFirstTrace) {
	bool block = false;
	for (auto p = CMovementForward::m_pFirst; p; p = p->m_pNext) {
		if (!p->OnTryPlayerMove(ms, mv, pFirstDest, pFirstTrace)) {
			block = true;
		}
	}

	if (block) {
		return;
	}

	MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnTryPlayerMove, ms, mv, pFirstDest, pFirstTrace);

	FORWARD_POST(CMovementForward, OnTryPlayerMovePost, ms, mv, pFirstDest, pFirstTrace);
}

static void Hook_OnCategorizePosition(CCSPlayer_MovementServices* ms, CMoveData* mv, bool bStayOnGround) {
	bool block = false;
	for (auto p = CMovementForward::m_pFirst; p; p = p->m_pNext) {
		if (!p->OnCategorizePosition(ms, mv, bStayOnGround)) {
			block = true;
		}
	}
	if (block) {
		return;
	}

	MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnCategorizePosition, ms, mv, bStayOnGround);

	CMovementPlayer* player = MOVEMENT::GetPlayerManager()->ToPlayer(ms);
	bool oldOnGround = !!(player->GetPlayerPawn()->m_fFlags() & FL_ONGROUND);
	bool ground = !!(player->GetPlayerPawn()->m_fFlags() & FL_ONGROUND);
	if (oldOnGround != ground) {
		if (ground) {
			player->RegisterLanding(mv->m_vecVelocity);
			// player->duckBugged = player->processingDuck;
			FORWARD_POST(CMovementForward, OnStartTouchGround, player);
		} else {
			player->RegisterTakeoff(false);
			FORWARD_POST(CMovementForward, OnStopTouchGround, player);
		}
	}

	FORWARD_POST(CMovementForward, OnCategorizePositionPost, ms, mv, bStayOnGround);
}

static void Hook_OnJump(CCSPlayer_MovementServices* ms, CMoveData* mv) {
	bool block = false;
	for (auto p = CMovementForward::m_pFirst; p; p = p->m_pNext) {
		if (!p->OnJump(ms, mv)) {
			block = true;
		}
	}
	if (block) {
		return;
	}

	MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnJump, ms, mv);

	CMovementPlayer* player = MOVEMENT::GetPlayerManager()->ToPlayer(ms);
	Vector oldOutWishVel = mv->m_outWishVel;
	MoveType_t oldMoveType = player->GetPlayerPawn()->m_MoveType();
	if (mv->m_outWishVel != oldOutWishVel) {
		player->RegisterTakeoff(true);
		FORWARD_POST(CMovementForward, OnStopTouchGround, player);
	}

	FORWARD_POST(CMovementForward, OnJumpPost, ms, mv);
}

static void Hook_OnProcessMovement(CCSPlayer_MovementServices* ms, CMoveData* mv) {
	CMovementPlayer* player = MOVEMENT::GetPlayerManager()->ToPlayer(ms);
	player->currentMoveData = mv;
	player->moveDataPre = CMoveData(*mv);

	bool block = false;
	for (auto p = CMovementForward::m_pFirst; p; p = p->m_pNext) {
		if (!p->OnProcessMovement(ms, mv)) {
			block = true;
		}
	}
	if (block) {
		return;
	}

	MEM::SDKCall<void>(MOVEMENT::TRAMPOLINE::g_fnProcessMovement, ms, mv);

	player->moveDataPost = CMoveData(*mv);

	FORWARD_POST(CMovementForward, OnProcessMovementPost, ms, mv);
}

void MOVEMENT::SetupHooks() {
	HOOK_SIG("CPlayer_MovementServices::RunCmds", Hook_OnMovementServicesRunCmds, MOVEMENT::TRAMPOLINE::g_fnMovementServicesRunCmds);
	HOOK_SIG("CCSPlayer_MovementServices::TryPlayerMove", Hook_OnTryPlayerMove, MOVEMENT::TRAMPOLINE::g_fnTryPlayerMove);
	HOOK_SIG("CCSPlayer_MovementServices::CategorizePosition", Hook_OnCategorizePosition, MOVEMENT::TRAMPOLINE::g_fnCategorizePosition);
	HOOK_SIG("CCSPlayer_MovementServices::OnJump", Hook_OnJump, MOVEMENT::TRAMPOLINE::g_fnJump);
	HOOK_SIG("CCSPlayer_MovementServices::ProcessMovement", Hook_OnProcessMovement, MOVEMENT::TRAMPOLINE::g_fnProcessMovement);
}

CMovementPlayerManager* MOVEMENT::GetPlayerManager() {
	return static_cast<CMovementPlayerManager*>(::GetPlayerManager());
}
