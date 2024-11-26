#pragma once

#include <surf/surf_player.h>

class CSurfHudService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	virtual void OnPhysicsSimulatePost() override;
};
