#pragma once

#include <core/forwards.h>
#include <core/adminmanager.h>

#include <sdk/entity/ccsplayercontroller.h>

using ConCmd_Callback = std::function<void(CCSPlayerController* pController, const std::vector<std::string>& vArgs)>;
using SrvCmd_Callback = std::function<void(const std::vector<std::string>& vArgs)>;

namespace CONCMD {
	class CConCmdManager : CCoreForward {
		virtual void OnDispatchConCommand(ICvar* pCvar, ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args) override;

	public:
		struct CSmConCmdInfo {
			ConCmd_Callback callback;
			AdminFlag adminFlags = AdminFlag::None;
			std::string description;
			int64_t cmdFlags = FCVAR_NONE;
		};

		struct CSmSrvCmdInfo {
			SrvCmd_Callback callback;
			std::string description;
			int64_t cmdFlags = FCVAR_NONE;
		};

		std::unordered_map<std::wstring, std::vector<CSmConCmdInfo>> m_umConCmds;
		std::unordered_map<std::wstring, std::vector<CSmSrvCmdInfo>> m_umSrvCmds;
	};

	extern CConCmdManager* GetManager();
} // namespace CONCMD
