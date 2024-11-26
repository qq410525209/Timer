#pragma once

#include <surf/surf_player.h>

class CSurfZoneService : CSurfBaseService, CCoreForward {
public:
	using CSurfBaseService::CSurfBaseService;

private:
	virtual void OnPluginStart() override;
};
