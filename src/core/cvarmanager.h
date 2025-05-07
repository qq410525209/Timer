#pragma once

#include <tier1/convar.h>
#include <optional>

extern void ReplicateConVarValue(const char* name, const char* value, CPlayerSlot slot);

namespace CVAR {
	// Must be on OnPluginStart
	template<typename T>
	ConVarRefAbstract* Register(const char* name, T defaultValue, const char* helpString, typename CConVar<T>::FnChangeCallback_t callback = 0) {
		CConVar<T>* pCvarCreated = new CConVar<T>(name, 0, helpString, defaultValue, callback);
		return pCvarCreated;
	}

	template<typename T>
	ConVarRefAbstract* Register(const char* name, T defaultValue, const char* helpString, T minValue, typename CConVar<T>::FnChangeCallback_t callback = 0) {
		CConVar<T>* pCvarCreated = new CConVar<T>(name, 0, helpString, defaultValue, true, minValue, false, T {}, callback);
		return pCvarCreated;
	}

	template<typename T>
	ConVarRefAbstract* Register(const char* name, T defaultValue, const char* helpString, T minValue, T maxValue, typename CConVar<T>::FnChangeCallback_t callback = 0) {
		CConVar<T>* pCvarCreated = new CConVar<T>(name, 0, helpString, defaultValue, true, minValue, true, maxValue, callback);
		return pCvarCreated;
	}

	// TODO: only maxValue
	// ConVarRefAbstract* Register()

	std::optional<ConVarRefAbstract> Find(const char* name);

	template<typename T>
	bool Set(const char* name, T value) {
		auto targetCvar = Find(name);
		if (!targetCvar) {
			return false;
		}

		auto& targetCvarRef = targetCvar.value();
		targetCvarRef.SetAs(value);

		return true;
	}

	template<typename T>
	bool Replicate(const char* name, T value, CPlayerSlot slot) {
		auto targetCvar = Find(name);
		if (!targetCvar) {
			return false;
		}

		auto& targetCvarRef = targetCvar.value();

		CBufferString buffer;
		targetCvarRef.TypeTraits()->ValueToString(value, buffer);
		ReplicateConVarValue(name, buffer.Get(), slot);

		return true;
	}
} // namespace CVAR
