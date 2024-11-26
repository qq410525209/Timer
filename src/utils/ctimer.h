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

	virtual ~CTimerBase() {}

	virtual bool Execute() = 0;

	f64 interval {};
	f64 lastExecute = -1;
	bool useRealTime {};
};

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

class IFrameAction {
public:
	virtual ~IFrameAction() {}

	virtual void Execute() = 0;
};

template<typename... Args>
class CFrameAction : public IFrameAction {
public:
	using Fn = void(__cdecl*)(Args... args);

	Fn m_fn;
	std::tuple<Args...> m_args;

	explicit CFrameAction(Fn fn, Args... args) : m_fn(fn), m_args(std::make_tuple(std::move(args)...)) {}

	virtual void Execute() override {
		std::apply(m_fn, m_args);
	}
};

namespace UTIL {
	namespace TIMER {
		void AddTimer(CTimerBase* timer, bool preserveMapChange = true);
		void RemoveTimer(CTimerBase* timer);
		void AddFrameAction(std::unique_ptr<IFrameAction> action);
	} // namespace TIMER

	/* Creates a timer for the given function, the function must return a f64 that represents the interval in seconds; 0 or less to stop the timer */
	template<typename... Args>
	CTimer<Args...>* StartTimer(typename CTimer<Args...>::Fn fn, Args... args, f64 initialDelay, bool preserveMapChange = true,
								bool useRealTime = false) {
		auto timer = new CTimer<Args...>(useRealTime, initialDelay, fn, args...);
		TIMER::AddTimer(timer, preserveMapChange);
		return timer;
	}

	/* Request a frame action for the given function. */
	template<typename... Args>
	void RequestFrame(typename CFrameAction<Args...>::Fn fn, Args... args) {
		auto pFrameAction = std::make_unique<CFrameAction<Args...>>(fn, args...);
		TIMER::AddFrameAction(std::move(pFrameAction));
	}
} // namespace UTIL
