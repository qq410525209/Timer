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
		struct CConCmdInfo {
			ConCmd_Callback callback;
			AdminFlag adminFlags = AdminFlag::None;
			std::string description;
			int64_t cmdFlags = FCVAR_NONE;
		};

		struct CSrvCmdInfo {
			SrvCmd_Callback callback;
			std::string description;
			int64_t cmdFlags = FCVAR_NONE;
		};

		std::unordered_map<std::wstring, std::vector<CConCmdInfo>> m_umConCmds;
		std::unordered_map<std::wstring, std::vector<CSrvCmdInfo>> m_umSrvCmds;
	};

	void RegServerCmd(std::string cmd, SrvCmd_Callback cb, std::string description = std::string(), int64_t cmdFlag = FCVAR_NONE);
	void RegConsoleCmd(std::string cmd, ConCmd_Callback cb, std::string description = std::string(), int64_t cmdFlag = FCVAR_NONE);
	void RegAdminCmd(std::string cmd, ConCmd_Callback cb, AdminFlag adminFlags, std::string description = std::string(),
					 int64_t cmdFlag = FCVAR_NONE);

	extern CConCmdManager* GetManager();
} // namespace CONCMD
