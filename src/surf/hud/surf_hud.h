#pragma once

#include <surf/surf_player.h>

class CSurfHudPlugin : CMovementForward {
private:
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3], const int& weapon,
									const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;
};

class CSurfHudService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;
};
