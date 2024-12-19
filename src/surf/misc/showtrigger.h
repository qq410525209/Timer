#pragma once

#include <core/forwards.h>

class CShowTrigger : CCoreForward {
private:

public:
	void TransmitTriggers(const std::vector<CHandle<CBaseTrigger>>& vTriggers, bool bTransmit);
};

extern CShowTrigger g_ShowTrigger;
