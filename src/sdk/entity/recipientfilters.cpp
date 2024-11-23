#include "recipientfilters.h"
#include <utils/utils.h>

void CRecipientFilter::AddRecipient(CPlayerSlot slot) {
	// Don't add if it already exists
	if (m_Recipients.Find(slot) != m_Recipients.InvalidIndex()) {
		return;
	}

	m_Recipients.AddToTail(slot);
}

void CRecipientFilter::AddAllPlayers() {
	m_Recipients.RemoveAll();
	if (!GameEntitySystem()) {
		return;
	}

	for (int i = 0; i <= UTIL::GetServerGlobals()->maxClients; i++) {
		CBaseEntity* ent = static_cast<CBaseEntity*>(GameEntitySystem()->GetEntityInstance(CEntityIndex(i)));
		if (ent) {
			AddRecipient(i);
		}
	}
}

CCopyRecipientFilter::CCopyRecipientFilter(IRecipientFilter* source, int iExcept) {
	for (int i = 0; i < source->GetRecipientCount(); i++) {
		if (source->GetRecipientIndex(i).Get() != iExcept) {
			this->AddRecipient(source->GetRecipientIndex(i));
		}
	}
}
