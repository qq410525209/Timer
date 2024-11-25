#include <movement/movement.h>
#include <pch.h>

class CRampfix : CMovementForward {
	virtual bool OnTryPlayerMove(CCSPlayer_MovementServices* ms, CMoveData* mv, Vector* pFirstDest, trace_t* pFirstTrace) override;
	virtual void OnTryPlayerMovePost(CCSPlayer_MovementServices* ms, const CMoveData* mv, const Vector* pFirstDest,
									 const trace_t* pFirstTrace) override;
};

CRampfix g_Rampfix;

bool CRampfix::OnTryPlayerMove(CCSPlayer_MovementServices* ms, CMoveData* mv, Vector* pFirstDest, trace_t* pFirstTrace) {
	return false;
}

void CRampfix::OnTryPlayerMovePost(CCSPlayer_MovementServices* ms, const CMoveData* mv, const Vector* pFirstDest, const trace_t* pFirstTrace) {}
