#pragma once

#include <string>
#include <unordered_map>

enum EventHookMode {
	EventHookMode_Pre,       //< Hook callback fired before event is fired */
	EventHookMode_Post,      //< Hook callback fired after event is fired */
	EventHookMode_PostNoCopy //< Hook callback fired after event is fired, but event data won't be copied */
};

#define EVENT_CALLBACK_PRE_ARGS   IGameEvent *pEvent, const char *szName, bool &bDontBroadcast
#define EVENT_CALLBACK_PRE(fn)    static bool fn(EVENT_CALLBACK_PRE_ARGS)
#define EVENT_CALLBACK_PRE_L(...) [__VA_ARGS__](EVENT_CALLBACK_PRE_ARGS) -> bool
using FnFireEventCallbackPre_t = std::function<bool(EVENT_CALLBACK_PRE_ARGS)>;

#define EVENT_CALLBACK_POST_ARGS   IGameEvent *pEvent, const char *szName, bool bDontBroadcast
#define EVENT_CALLBACK_POST(fn)    static void fn(EVENT_CALLBACK_POST_ARGS)
#define EVENT_CALLBACK_POST_L(...) [__VA_ARGS__](EVENT_CALLBACK_POST_ARGS) -> void
using FnFireEventCallbackPost_t = std::function<void(EVENT_CALLBACK_POST_ARGS)>;

#define EVENT_CALLBACK_POSTNOCOPY_ARGS   const char *szName, bool bDontBroadcast
#define EVENT_CALLBACK_POSTNOCOPY(fn)    static void fn(EVENT_CALLBACK_POSTNOCOPY_ARGS)
#define EVENT_CALLBACK_POSTNOCOPY_L(...) [__VA_ARGS__](EVENT_CALLBACK_POSTNOCOPY_ARGS) -> void
using FnFireEventCallbackPostNoCopy_t = std::function<void(EVENT_CALLBACK_POSTNOCOPY_ARGS)>;

struct event_t {
	const char* m_szName;
	FnFireEventCallbackPre_t m_pCallbackPre;
	FnFireEventCallbackPost_t m_pCallbackPost;
	FnFireEventCallbackPostNoCopy_t m_pCallbackPostNoCopy;
};

namespace EVENT {
	void HookEvent(const char* name, FnFireEventCallbackPre_t callback);
	void HookEvent(const char* name, FnFireEventCallbackPost_t callback);
	void HookEvent(const char* name, FnFireEventCallbackPostNoCopy_t callback);
	void UnhookEvent(const char* name);

	inline std::unordered_map<std::string, std::vector<event_t>> m_plist;
} // namespace EVENT
