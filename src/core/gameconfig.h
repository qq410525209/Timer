#pragma once

#include <string>
#include <unordered_map>

#include <sdk/common.h>

class CGameConfig {
public:
	CGameConfig(std::string pszPath);

	json m_Json;
	std::string m_pszFile;
	std::unordered_map<std::string, void*> m_pMemSig;

	template<typename T = int>
	T GetOffset(std::string name) {
		if (m_Json.is_null()) {
			SURF_ASSERT(false);
			return -1;
		}

		if (m_Json.find("Offset") == m_Json.end()) {
			SURF_ASSERT(false);
			return -1;
		}

		auto& offset = m_Json["Offset"];
		if (offset.is_null() || offset.empty()) {
			SURF_ASSERT(false);
			return -1;
		}

		auto& element = offset[name];
		if (element.is_null() || element.empty()) {
			SURF_ASSERT(false);
			return -1;
		}

		return element[WIN_LINUX("windows", "linux")].get<T>();
	}

public:
	virtual void* GetMemSig(std::string name);
	virtual void* GetAddress(std::string name);

	bool IsValid() {
		return !m_Json.is_null();
	}
};
