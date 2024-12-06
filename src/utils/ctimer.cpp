#include "ctimer.h"
#include <utils/utils.h>
#include <queue>
#include <core/forwards.h>

CUtlVector<CTimerBase*> g_NonPersistentTimers;
CUtlVector<CTimerBase*> g_PersistentTimers;

std::mutex g_FrameMutex;
std::queue<std::unique_ptr<IFrameAction>> g_FrameQueue;
std::queue<std::unique_ptr<IFrameAction>> g_FrameActions;

static void ProcessFrameActions() {
	if (g_FrameQueue.size()) {
		g_FrameMutex.lock();
		std::swap(g_FrameQueue, g_FrameActions);
		g_FrameMutex.unlock();

		while (!g_FrameActions.empty()) {
			g_FrameActions.front()->Execute();
			g_FrameActions.pop();
		}
	}
}

static void ProcessTimerList(CUtlVector<CTimerBase*>& timers) {
	for (int i = timers.Count() - 1; i >= 0; i--) {
		auto timer = timers[i];
		f64 currentTime = timer->useRealTime ? UTIL::GetGlobals()->realtime : UTIL::GetGlobals()->curtime;
		if (timer->lastExecute == -1) {
			timer->lastExecute = currentTime;
		}

		if (timer->lastExecute + timer->interval <= currentTime) {
			if (!timer->Execute()) {
				delete timer;
				timers.Remove(i);
			} else {
				timer->lastExecute = currentTime;
			}
		}
	}
}

void RemoveNonPersistentTimers() {
	g_NonPersistentTimers.PurgeAndDeleteElements();
}

void UTIL::TIMER::AddTimer(CTimerBase* timer, bool preserveMapChange) {
	if (preserveMapChange) {
		g_PersistentTimers.AddToTail(timer);
	} else {
		g_NonPersistentTimers.AddToTail(timer);
	}
}

void UTIL::TIMER::RemoveTimer(CTimerBase* timer) {
	FOR_EACH_VEC(g_PersistentTimers, i) {
		if (g_PersistentTimers.Element(i) == timer) {
			g_PersistentTimers.Remove(i);
			return;
		}
	}

	FOR_EACH_VEC(g_NonPersistentTimers, i) {
		if (g_NonPersistentTimers.Element(i) == timer) {
			g_NonPersistentTimers.Remove(i);
			return;
		}
	}
}

void UTIL::TIMER::AddFrameAction(std::unique_ptr<IFrameAction> action) {
	g_FrameMutex.lock();
	g_FrameQueue.push(std::move(action));
	g_FrameMutex.unlock();
}

class CProcessTimer : CCoreForward {
private:
	virtual void OnServerGamePostSimulate(IGameSystem* pGameEvent) override {
		::ProcessFrameActions();
		::ProcessTimerList(g_PersistentTimers);
		::ProcessTimerList(g_NonPersistentTimers);
	}
};

CProcessTimer g_ProcessTimer;
