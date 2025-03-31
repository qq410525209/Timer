#pragma once

#include <gametrace.h>

struct touchlist_t {
	Vector deltavelocity;
	trace_t trace;
};

class CTraceFilterPlayerMovementCS : public CTraceFilter {};

// Doesn't really match with source2sdk
template<typename T>
class CWeakHandle {
public:
	CWeakHandle(const std::shared_ptr<T>& pData) : m_wpData(pData) {}

	virtual ~CWeakHandle() {}

	virtual bool IsValid() const {
		return this->m_wpData.expired();
	}

	virtual bool Close() {
		COMPILE_TIME_ASSERT("Implement this!");
		return false;
	}

	T* Data() {
		return m_wpData.lock().get();
	}

protected:
	std::weak_ptr<T> m_wpData;
};
