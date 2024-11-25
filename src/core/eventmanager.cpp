#include "eventmanager.h"

void EVENT::HookEvent(const char* name, FnFireEventCallbackPre_t callback) {
	event_t event {.m_szName = name, .m_pCallbackPre = callback};
	if (!m_plist.count(name)) {
		m_plist[name] = std::vector<event_t>();
	}
	m_plist[name].push_back(event);
}

void EVENT::HookEvent(const char* name, FnFireEventCallbackPost_t callback) {
	event_t event {.m_szName = name, .m_pCallbackPost = callback};
	if (!m_plist.count(name)) {
		m_plist[name] = std::vector<event_t>();
	}
	m_plist[name].push_back(event);
}

void EVENT::HookEvent(const char* name, FnFireEventCallbackPostNoCopy_t callback) {
	event_t event {.m_szName = name, .m_pCallbackPostNoCopy = callback};
	if (!m_plist.count(name)) {
		m_plist[name] = std::vector<event_t>();
	}
	m_plist[name].push_back(event);
}

void EVENT::UnhookEvent(const char* name) {
	auto search = m_plist.find(name);
	if (search == m_plist.end()) {
		return;
	}
	m_plist.erase(search);
}
