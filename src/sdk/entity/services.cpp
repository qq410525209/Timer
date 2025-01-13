#include "services.h"

CBaseViewModel* CCSPlayer_ViewModelServices::GetViewModel(int iIndex) {
	return m_hViewModel()[iIndex].Get();
}

void CCSPlayer_ViewModelServices::SetViewModel(int iIndex, CBaseViewModel* pViewModel) {
	static uint uOffset = schema::GetOffset("CCSPlayerPawnBase", "m_pViewModelServices").offset;
	this->GetPawn()->NetworkStateChanged(uOffset);
	m_hViewModel()[iIndex].Set(pViewModel);
	pViewModel->m_nViewModelIndex(iIndex);
}
