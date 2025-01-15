#pragma once

#include <sdk/convars2.h>

extern void ReplicateConVarValue(const char* name, const char* value, CPlayerSlot slot);

namespace CVAR {
	template<typename T>
	BaseConVar* Register(const char* name, T defaultValue, const char* helpString = "", typename ConVarS2<T>::FnChangeCallback_t callback = 0) {
		ConVarS2<T>* pCvarCreated = new ConVarS2<T>(name, 0, helpString, defaultValue, callback);
		return pCvarCreated;
	}

	CConVarBaseData* Find(const char* name);

	template<typename T>
	bool Set(const char* name, T value) {
		CConVarBaseData* pTargetCvar = Find(name);
		if (!pTargetCvar) {
			return false;
		}

		CConVarData<T>* pCastCvar = pTargetCvar->Cast<T>();
		if (!pCastCvar) {
			Plat_FatalErrorFunc("Failed to match Cvar type! Target type: %d, T type: %d\n", pTargetCvar->GetType(), TranslateConVarType<T>());
			DebuggerBreak();
			return false;
		}

		pCastCvar->SetValue(value);
		return true;
	}

	template<typename T>
	bool Replicate(const char* name, T value, CPlayerSlot slot) {
		CConVarBaseData* pTargetCvar = Find(name);
		if (!pTargetCvar) {
			return false;
		}

		CConVarData<T>* pCastCvar = pTargetCvar->Cast<T>();
		if (!pCastCvar) {
			Plat_FatalErrorFunc("Failed to match Cvar type! Target type: %d, T type: %d\n", pTargetCvar->GetType(), TranslateConVarType<T>());
			DebuggerBreak();
			return false;
		}

		char szNewValue[256];
		CConVarData<T>::ValueToString(value, szNewValue, sizeof(szNewValue));
		ReplicateConVarValue(name, szNewValue, slot);

		return true;
	}
} // namespace CVAR
