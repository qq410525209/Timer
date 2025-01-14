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
	CBaseViewModel* pCustomViewModel = m_pViewModelServices()->GetViewModel(1);
	if (!pCustomViewModel) {
		pCustomViewModel = (CBaseViewModel*)MEM::CALL::CreateEntityByName("predicted_viewmodel");
		pCustomViewModel->DispatchSpawn();
		m_pViewModelServices()->SetViewModel(1, pCustomViewModel);
		pCustomViewModel->m_hOwnerEntity(this->GetRefEHandle());
	} else {
		static QAngle nullAng;
		pCustomViewModel->Teleport(nullptr, &nullAng, nullptr);
	}

	return pCustomViewModel;
}
