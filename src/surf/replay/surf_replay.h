#pragma once

#include <pch.h>

class CSurfReplay : CCoreForward {
	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButtonState* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
								int& tickcount, int& seed, int (&mouse)[2]) override;
};
