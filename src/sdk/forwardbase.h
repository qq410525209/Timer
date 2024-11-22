#pragma once

template<typename T>
class CForwardBase {
public:
	CForwardBase();
	~CForwardBase();

	static T* m_pFirst;
	T* m_pNext;
};

template<typename T>
CForwardBase<T>::CForwardBase() {
	m_pNext = m_pFirst;
	m_pFirst = static_cast<T*>(this);
}

template<typename T>
CForwardBase<T>::~CForwardBase() {
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
