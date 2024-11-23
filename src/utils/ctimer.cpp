#include "ctimer.h"
#include <utils/utils.h>

CUtlVector<CTimerBase*> g_NonPersistentTimers;
CUtlVector<CTimerBase*> g_PersistentTimers;

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

void UTIL::ProcessTimers() {
	::ProcessTimerList(g_PersistentTimers);
	::ProcessTimerList(g_NonPersistentTimers);
}

void UTIL::RemoveNonPersistentTimers() {
	g_NonPersistentTimers.PurgeAndDeleteElements();
}

void UTIL::AddTimer(CTimerBase* timer, bool preserveMapChange) {
	if (preserveMapChange) {
		g_PersistentTimers.AddToTail(timer);
	} else {
		g_NonPersistentTimers.AddToTail(timer);
	}
}

void UTIL::RemoveTimer(CTimerBase* timer) {
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
