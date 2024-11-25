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

using FnFireEventCallbackPostNoCopy_t = std::function<void(const char* szName, bool bDontBroadcast)>;
using FnFireEventCallbackPost_t = std::function<void(IGameEvent* pEvent, const char* szName, bool bDontBroadcast)>;
using FnFireEventCallbackPre_t = std::function<EventHookAction(IGameEvent* pEvent, const char* szName, bool& bDontBroadcast)>;

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
