#pragma once

#include <pch.h>
#include <movement/movement.h>

class CSurfReplay : CMovementForward {
	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButton* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
								int& tickcount, int& seed, int (&mouse)[2]) override;
};
