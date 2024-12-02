#pragma once

#include <pch.h>
#include <surf/surf_player.h>

class CSurfReplayPlugin : CMovementForward {
	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButton* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
								int& tickcount, int& seed, int (&mouse)[2]) override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3], const int& weapon,
									const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]);
};

class CSurfReplayService : CSurfBaseService {
	using CSurfBaseService::CSurfBaseService;

	virtual void Reset() override;
};
