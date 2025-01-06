#pragma once

#include <core/forwards.h>

class CShowTrigger : CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnActivateServer(CNetworkGameServerBase* pGameServer) override;

public:
	void TransmitTriggers(const std::vector<CHandle<CBaseTrigger>>& vTriggers, bool bTransmit);
};

extern CShowTrigger g_ShowTrigger;
