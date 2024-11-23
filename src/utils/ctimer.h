#pragma once

#include <functional>
#include <tuple>

#include <sdk/common.h>
#include <utlvector.h>

/*
 * Credit to cs2kz
 */

class CTimerBase {
public:
	CTimerBase(f64 initialInterval, bool useRealTime) : interval(initialInterval), useRealTime(useRealTime) {};

	virtual bool Execute() = 0;

	f64 interval {};
	f64 lastExecute = -1;
	bool useRealTime {};
};

extern CUtlVector<CTimerBase*> g_NonPersistentTimers;
extern CUtlVector<CTimerBase*> g_PersistentTimers;

template<typename... Args>
class CTimer : public CTimerBase {
public:
	using Fn = f64(__cdecl*)(Args... args);

	Fn m_fn;
	std::tuple<Args...> m_args;

	explicit CTimer(bool useRealTime, f32 initialDelay, Fn fn, Args... args)
		: CTimerBase(initialDelay, useRealTime), m_fn(fn), m_args(std::make_tuple(std::move(args)...)) {}

	bool Execute() override {
		interval = std::apply(m_fn, m_args);
		return interval > 0;
	}
};

namespace UTIL {
	void ProcessTimers();
	void RemoveNonPersistentTimers();
	void AddTimer(CTimerBase* timer, bool preserveMapChange = true);
	void RemoveTimer(CTimerBase* timer);
} // namespace UTIL
