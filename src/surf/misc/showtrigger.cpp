#include "showtrigger.h"

CShowTrigger g_ShowTrigger;

enum S2EdictState {
	FL_FULL_EDICT_CHANGED_S2 = 0x2,
	FL_EDICT_ALWAYS_S2 = 0x4,
	FL_EDICT_DONTSEND_S2 = 0x8,
	FL_EDICT_PVSCHECK_S2 = 0x10,
};

void CShowTrigger::TransmitTriggers(const std::vector<CHandle<CBaseTrigger>>& vTriggers, bool bTransmit) {
	for (const auto& hTrigger : vTriggers) {
		CBaseTrigger* pTrigger = hTrigger.Get();
		if (!pTrigger) {
			continue;
		}

		auto iEffect = pTrigger->m_fEffects();
		auto iStateFlags = pTrigger->m_NetworkTransmitComponent()->m_iStateFlags();

		if (bTransmit) {
			iEffect &= ~EF_NODRAW;
			iStateFlags &= ~FL_EDICT_DONTSEND_S2;
		} else {
			iEffect |= EF_NODRAW;
			iStateFlags |= FL_EDICT_DONTSEND_S2;
		}

		pTrigger->m_fEffects(iEffect);

		auto target = (uint*)((char*)pTrigger + 0x1c0);
		_InterlockedExchange(target, iStateFlags);
		// pTrigger->m_NetworkTransmitComponent()->m_iStateFlags(iStateFlags);
		// pTrigger->NetworkStateChanged();
	}
}
