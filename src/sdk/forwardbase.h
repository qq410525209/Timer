#pragma once

template<typename T>
class CBaseForward {
public:
	CBaseForward();
	~CBaseForward();

	static T* m_pFirst;
	T* m_pNext;
};

template<typename T>
CBaseForward<T>::CBaseForward() {
	m_pNext = m_pFirst;
	m_pFirst = static_cast<T*>(this);
}

template<typename T>
CBaseForward<T>::~CBaseForward() {
	if (this == m_pFirst) {
		m_pFirst = static_cast<T*>(m_pNext);
		return;
	}

	T* pPrev = m_pFirst;
	while (pPrev != nullptr && pPrev->m_pNext != this) {
		pPrev = static_cast<T*>(pPrev->m_pNext);
	}

	if (pPrev != nullptr) {
		pPrev->m_pNext = m_pNext;
	}
}
