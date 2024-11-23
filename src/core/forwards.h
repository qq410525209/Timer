#pragma once

#include <sdk/forwardbase.h>
#include <sdk/usercmd.h>
#include <sdk/entity/ccsplayerpawn.h>

class CSurfForward : public CForwardBase<CSurfForward> {
public:
	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButtonState* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
								int& tickcount, int& seed, int (&mouse)[2]) {
		return true;
	}

	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButtonState* buttons, const float (&vec)[3], const float (&angles)[3],
									const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {}
};
