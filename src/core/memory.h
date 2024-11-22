#pragma once

#include <sdk/common.h>

class CBasePlayerController;
class CCSPlayerController;
class CCSPlayerPawn;

namespace MEM {
	namespace CALL {
		void SwitchTeam(CCSPlayerController* controller, int team);
		void SetPawn(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool a3, bool a4, bool a5);
	} // namespace CALL

	void SetupHooks();

	template<typename T, typename... Args>
	T SDKCall(void* pAddress, Args... args) {
		auto pFn = reinterpret_cast<T (*)(Args...)>(pAddress);
		SURF_ASSERT((uintptr_t)pFn);
		return pFn(args...);
	}

	template<typename T, typename... Args>
	auto GetFn(void* pAddress, Args... args) {
		return reinterpret_cast<T (*)(Args...)>(pAddress);
	}
} // namespace MEM
