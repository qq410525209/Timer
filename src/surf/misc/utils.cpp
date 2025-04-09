#include "surf_misc.h"
#include <utils/print.h>

void SURF::Print(CSurfPlayer* pSurfPlayer, const char* fmt, ...) {
	auto pController = pSurfPlayer->GetController();
	if (pController) {
		CUtlString buffer;
		va_list args;
		va_start(args, fmt);
		buffer.FormatV(fmt, args);

		UTIL::CPrintChat(pController, "{green}[Surf] {grey}%s", buffer.Get());
	}
}

void SURF::PrintWarning(CSurfPlayer* pSurfPlayer, const char* fmt, ...) {
	auto pController = pSurfPlayer->GetController();
	if (pController) {
		CUtlString buffer;
		va_list args;
		va_start(args, fmt);
		buffer.FormatV(fmt, args);

		UTIL::CPrintChat(pController, "{green}[Surf] {darkred}%s", buffer.Get());
	}
}
