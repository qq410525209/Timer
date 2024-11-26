#pragma once

#include <surf/surf_player.h>

class CSurfHudPlugin : CMovementForward {
private:
	virtual void OnPhysicsSimulatePost(CCSPlayerController* pController) override;
};

class CSurfHudService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;
};
