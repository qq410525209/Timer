#include "cbaseentity.h"
#include <core/memory.h>

void CBaseEntity::AcceptInput(const char* pInputName, variant_t value, CEntityInstance* pActivator, CEntityInstance* pCaller) {
	MEM::CALL::CEntityInstance_AcceptInput(this, pInputName, pActivator, pCaller, &value, 0);
}

void CBaseEntity::DispatchSpawn(CEntityKeyValues* pInitKeyValue) {
	MEM::CALL::DispatchSpawn(this, pInitKeyValue);
}

Vector CBaseEntity::GetAbsOrigin() {
	auto pBodyComponent = m_CBodyComponent();
	if (!pBodyComponent) {
		return Vector(0.0f, 0.0f, 0.0f);
	}

	auto pNode = pBodyComponent->m_pSceneNode();
	if (!pNode) {
		return Vector(0.0f, 0.0f, 0.0f);
	}

	return pNode->m_vecAbsOrigin();
}
