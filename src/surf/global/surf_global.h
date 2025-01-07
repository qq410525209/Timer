#pragma once

#include <core/forwards.h>

class CSurfGlobalAPIPlugin : CCoreForward {
private:
	virtual void OnPluginStart() override;

private:
	void RegisterCommand();
};

namespace SURF {
	extern CSurfGlobalAPIPlugin* GlobalPlugin();

	namespace GLOBAL {
		inline constexpr const char* BaseUrl = "";
	} // namespace GLOBAL
} // namespace SURF
