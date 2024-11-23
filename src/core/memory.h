#pragma once

#include <sdk/common.h>
#include <libmem/libmem_helper.h>
#include <vendor/libmodule/module.h>

class CBasePlayerController;
class CCSPlayerController;
class CCSPlayerPawn;

class GameSessionConfiguration_t {};

namespace MEM {
	namespace CALL {
		void SwitchTeam(CCSPlayerController* controller, int team);
		void SetPawn(CBasePlayerController* controller, CCSPlayerPawn* pawn, bool a3, bool a4, bool a5);
	} // namespace CALL

	namespace MODULE {
		inline std::shared_ptr<libmodule::CModule> engine;
		inline std::shared_ptr<libmodule::CModule> tier0;
		inline std::shared_ptr<libmodule::CModule> server;
		inline std::shared_ptr<libmodule::CModule> schemasystem;
		inline std::shared_ptr<libmodule::CModule> steamnetworkingsockets;

		void Setup();
	} // namespace MODULE

#pragma region trampoline

	inline void* g_fnMovementServicesRunCmds_Trampoline;

#pragma endregion

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

	template<typename T>
	inline bool VmtHookEx(uint32_t uIndex, libmodule::CModule* pModule, const char* className, T pFunc, void*& pOriginFunc,
						  bool allInstance = false) {
		if (!pModule) {
			return false;
		}

		auto extractClassName = [](std::string input) {
			size_t pos = input.find("::");
			if (pos != std::string::npos) {
				return input.substr(0, pos);
			}
			return input;
		};

		auto sClassName = extractClassName(className);

		void* vtable = pModule->GetVirtualTableByName(sClassName);
		if (!vtable) {
			return false;
		}

		libmem::Vmt vmt((libmem::Address*)vtable);
		auto from = vmt.GetOriginal(uIndex);

		if (allInstance) {
			libmem::HookFunc((void*)from, pFunc, pOriginFunc);
		} else {
			vmt.Hook(uIndex, (libmem::Address)pFunc);
			pOriginFunc = (void*)from;
		}

		return true;
	}
} // namespace MEM
