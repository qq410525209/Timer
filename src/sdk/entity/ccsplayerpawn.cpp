#include "ccsplayerpawn.h"
#include <sdk/entity/services.h>
#include <core/memory.h>

Vector CCSPlayerPawnBase::GetEyePosition() {
	Vector absorigin = GetAbsOrigin();
	CPlayer_CameraServices* cameraService = m_pCameraServices();
	if (!cameraService) {
		return absorigin;
	}
	return Vector(absorigin.x, absorigin.y, absorigin.z + cameraService->m_flOldPlayerViewOffsetZ());
}

CBaseViewModel* CCSPlayerPawnBase::GetCustomViewModel() {
	CBaseViewModel* pViewModel = m_pViewModelServices()->GetViewModel(1);
	if (!pViewModel) {
		pViewModel = (CBaseViewModel*)MEM::CALL::CreateEntityByName("predicted_viewmodel");
		pViewModel->DispatchSpawn();
		m_pViewModelServices()->SetViewModel(1, pViewModel);
		pViewModel->m_hOwnerEntity(this->GetRefEHandle());
	}

	return pViewModel;
}
