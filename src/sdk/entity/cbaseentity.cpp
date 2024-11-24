#include "cbaseentity.h"

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
