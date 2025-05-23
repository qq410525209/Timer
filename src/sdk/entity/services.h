#pragma once

#include <sdk/schema.h>
#include <sdk/usercmd.h>

enum ObserverMode_t {
	OBS_MODE_NONE = 0x0,    // not in spectator mode
	OBS_MODE_FIXED = 0x1,   // view from a fixed camera position
	OBS_MODE_IN_EYE = 0x2,  // follow a player in first person view
	OBS_MODE_CHASE = 0x3,   // follow a player in third person view
	OBS_MODE_ROAMING = 0x4, // free roaming
	OBS_MODE_DIRECTED = 0x5,
	NUM_OBSERVER_MODES = 0x6,
};

class CTakeDamageInfo {
public:
	DECLARE_SCHEMA_STRUCT(CTakeDamageInfo);

	SCHEMA_FIELD_POINTER(CHandle<CBaseEntity>, m_hAttacker);
};

class CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayerPawnComponent);

	SCHEMA_FIELD_POINTER(CNetworkVarChainer, __m_pChainEntity);

public:
	CCSPlayerPawn* GetPawn() {
		return (CCSPlayerPawn*)(__m_pChainEntity()->GetObj());
	}
};

class CPlayer_ObserverServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_ObserverServices);

	SCHEMA_FIELD(uint8_t, m_iObserverMode);
	SCHEMA_FIELD_POINTER(CHandle<CBaseEntity>, m_hObserverTarget);
	SCHEMA_FIELD(ObserverMode_t, m_iObserverLastMode);
	SCHEMA_FIELD(bool, m_bForcedObserverMode);
};

class CPlayer_MovementServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_MovementServices);

	SCHEMA_FIELD(int32_t, m_nImpulse);
	SCHEMA_FIELD(uint64_t, m_nQueuedButtonDownMask);
	SCHEMA_FIELD(uint64_t, m_nQueuedButtonChangeMask);
	SCHEMA_FIELD(uint64_t, m_nButtonDoublePressed);
	SCHEMA_FIELD(int32_t, m_nLastCommandNumberProcessed);
	SCHEMA_FIELD(uint64_t, m_nToggleButtonDownMask);
	SCHEMA_FIELD(float, m_flMaxspeed);
	SCHEMA_FIELD(float, m_flForwardMove);
	SCHEMA_FIELD(float, m_flLeftMove);
	SCHEMA_FIELD(float, m_flUpMove);
	SCHEMA_FIELD(Vector, m_vecLastMovementImpulses);
	SCHEMA_FIELD(QAngle, m_vecOldViewAngles);

	SCHEMA_FIELD_POINTER(CInButtonState, m_nButtons);
	SCHEMA_FIELD_POINTER(uint32_t, m_pButtonPressedCmdNumber);
	SCHEMA_FIELD_POINTER(float, m_arrForceSubtickMoveWhen);
};

class CPlayer_MovementServices_Humanoid : public CPlayer_MovementServices {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_MovementServices_Humanoid);

	SCHEMA_FIELD(bool, m_bDucking);
	SCHEMA_FIELD(bool, m_bDucked);
	SCHEMA_FIELD(float, m_flSurfaceFriction);
};

class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_MovementServices);

	SCHEMA_FIELD(Vector, m_vecLadderNormal);
	SCHEMA_FIELD(bool, m_bOldJumpPressed);
	SCHEMA_FIELD(float, m_flJumpPressedTime);
	SCHEMA_FIELD(float, m_flAccumulatedJumpError);
	SCHEMA_FIELD(float, m_flDuckSpeed);
	SCHEMA_FIELD(float, m_flDuckAmount);
	SCHEMA_FIELD(float, m_flStamina);
};

class CPlayer_UseServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_UseServices);
};

class CPlayer_ViewModelServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_ViewModelServices);
};

class CPlayer_CameraServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_CameraServices);

	SCHEMA_FIELD_POINTER(CHandle<CBaseEntity>, m_hViewEntity);
	SCHEMA_FIELD(float, m_flOldPlayerViewOffsetZ);
	SCHEMA_FIELD(QAngle, m_vecCsViewPunchAngle);
};

class CCSPlayerBase_CameraServices : public CPlayer_CameraServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayerBase_CameraServices);
};

class CPlayer_WeaponServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayer_WeaponServices);
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_WeaponServices);
};

class CCSPlayer_DamageReactServices : public CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_DamageReactServices);
};

class CCSPlayer_ViewModelServices : public CPlayer_ViewModelServices {
public:
	DECLARE_SCHEMA_CLASS(CCSPlayer_ViewModelServices);

	SCHEMA_FIELD_POINTER(CHandle<CBaseViewModel>, m_hViewModel);

	CBaseViewModel* GetViewModel(int iIndex = 0);
	void SetViewModel(int iIndex, CBaseViewModel* pViewModel);
};
