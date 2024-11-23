#pragma once

#include <sdk/schema.h>
#include <sdk/usercmd.h>

class CPlayerPawnComponent {
public:
	DECLARE_SCHEMA_CLASS(CPlayerPawnComponent);

	SCHEMA_FIELD_POINTER(CNetworkVarChainer, __m_pChainEntity);

public:
	CCSPlayerPawn* GetPawn() {
		return (CCSPlayerPawn*)(__m_pChainEntity()->GetObj());
	}
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
