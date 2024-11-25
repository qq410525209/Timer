#pragma once

#include <pch.h>

class CSurfMisc : CCoreForward {
	virtual void OnPluginStart() override;
	virtual void OnActivateServer(CNetworkGameServerBase* pGameServer) override;
};
