#pragma once

#include <string>
#include <unordered_map>

#include <sdk/common.h>

class CGameConfig {
public:
	CGameConfig(std::string sFilePath);

	virtual int GetOffset(std::string name);
	virtual void* GetMemSig(std::string name);
	virtual void* GetAddress(std::string name);

	bool IsValid() const {
		return !m_Json.is_null() && !m_Json.empty();
	}

private:
	json m_Json;
	std::string m_sFilePath;
	std::unordered_map<std::string, void*> m_pMemSig;
};

extern CGameConfig* g_pGameConfig;
