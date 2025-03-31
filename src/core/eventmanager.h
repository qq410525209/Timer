#pragma once

#include <string>
#include <unordered_map>

enum EventHookAction {
	EventHookAction_Continue,
	EventHookAction_Block,
};

enum EventHookMode {
	EventHookMode_Pre,       //< Hook callback fired before event is fired */
	EventHookMode_Post,      //< Hook callback fired after event is fired */
	EventHookMode_PostNoCopy //< Hook callback fired after event is fired, but event data won't be copied */
};

#define EVENT_PRE_CALLBACK_ARGS   IGameEvent *pEvent, const char *szName, bool &bDontBroadcast
#define EVENT_PRE_CALLBACK(fn)    static EventHookAction fn(EVENT_PRE_CALLBACK_ARGS)
#define EVENT_PRE_CALLBACK_L(...) [__VA_ARGS__](EVENT_PRE_CALLBACK_ARGS) -> EventHookAction
using FnFireEventCallbackPre_t = std::function<EventHookAction(EVENT_PRE_CALLBACK_ARGS)>;

#define EVENT_POST_CALLBACK_ARGS   IGameEvent *pEvent, const char *szName, bool bDontBroadcast
#define EVENT_POST_CALLBACK(fn)    static void fn(EVENT_POST_CALLBACK_ARGS)
#define EVENT_POST_CALLBACK_L(...) [__VA_ARGS__](EVENT_POST_CALLBACK_ARGS) -> void
using FnFireEventCallbackPost_t = std::function<void(EVENT_POST_CALLBACK_ARGS)>;

#define EVENT_POSTNOCOPY_CALLBACK_ARGS   const char *szName, bool bDontBroadcast
#define EVENT_POSTNOCOPY_CALLBACK(fn)    static void fn(EVENT_POSTNOCOPY_CALLBACK_ARGS)
#define EVENT_POSTNOCOPY_CALLBACK_L(...) [__VA_ARGS__](EVENT_POSTNOCOPY_CALLBACK_ARGS) -> void
using FnFireEventCallbackPostNoCopy_t = std::function<void(EVENT_POSTNOCOPY_CALLBACK_ARGS)>;

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
