#pragma once

#include "cbaseplayerpawn.h"

class CCSPlayer_ViewModelServices;

class CCSPlayerPawnBase : public CBasePlayerPawn {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerPawnBase);

	SCHEMA_FIELD(CCSPlayer_ViewModelServices*, m_pViewModelServices);
};

class CCSPlayerPawn : public CCSPlayerPawnBase {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerPawn);
	SCHEMA_FIELD(float, m_ignoreLadderJumpTime);
	SCHEMA_FIELD(float, m_flSlopeDropOffset);
	SCHEMA_FIELD(float, m_flSlopeDropHeight);

	SCHEMA_FIELD(float, m_flVelocityModifier);

public:
	template<typename T = CBasePlayerController>
	T* GetController() {
		return (T*)(m_hController()->Get());
	}

	void Respawn() {
		CALL_VIRTUAL(void, GAMEDATA::GetOffset("Respawn"), this);
	}
};
