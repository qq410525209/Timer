#pragma once

#include <pch.h>
#include <core/adminmanager.h>

#include <optional>

#define SCMD_CALLBACK_ARGS   const std::vector<std::string>& vArgs
#define SCMD_CALLBACK(fn)    static void fn(SCMD_CALLBACK_ARGS)
#define SCMD_CALLBACK_L(...) [__VA_ARGS__](SCMD_CALLBACK_ARGS) -> void
using SrvCmd_Callback = std::function<void(SCMD_CALLBACK_ARGS)>;

#define CCMD_CALLBACK_ARGS   CCSPlayerController *pController, const std::vector<std::string>&vArgs
#define CCMD_CALLBACK(fn)    static void fn(CCMD_CALLBACK_ARGS)
#define CCMD_CALLBACK_L(...) [__VA_ARGS__](CCMD_CALLBACK_ARGS) -> void
using ConCmd_Callback = std::function<void(CCMD_CALLBACK_ARGS)>;

#define CCMDLISTENER_CALLBACK(fn)    static bool fn(CCMD_CALLBACK_ARGS)
#define CCMDLISTENER_CALLBACK_L(...) [__VA_ARGS__](CCMD_CALLBACK_ARGS) -> bool
using ConCmdListener_Callback = std::function<bool(CCMD_CALLBACK_ARGS)>;

namespace CONCMD {
	class CConCmdManager : CCoreForward {
		virtual bool OnClientCommand(ISource2GameClients* pClient, CPlayerSlot slot, const CCommand& args) override;
		virtual bool OnDispatchConCommand(ICvar* pCvar, ConCommandRef cmd, const CCommandContext& ctx, const CCommand& args) override;

	public:
		struct CConCmdInfo {
			ConCmd_Callback callback;
			AdminFlag adminFlags = AdminFlag::None;
			std::string description;
			uint64 cmdFlags = FCVAR_NONE;
		};

		struct CSrvCmdInfo {
			SrvCmd_Callback callback;
			std::string description;
			uint64 cmdFlags = FCVAR_NONE;
		};

		std::unordered_map<std::wstring, std::vector<CConCmdInfo>> m_umConCmds;
		std::unordered_map<std::wstring, std::vector<CSrvCmdInfo>> m_umSrvCmds;
		std::unordered_map<std::wstring, std::vector<ConCmdListener_Callback>> m_umConCmdListeners;
	};

	void RegServerCmd(std::string cmd, SrvCmd_Callback cb, std::string description = std::string(), uint64 cmdFlag = FCVAR_NONE);
	void RegConsoleCmd(std::string cmd, ConCmd_Callback cb, std::string description = std::string(), uint64 cmdFlag = FCVAR_NONE);
	void RegAdminCmd(std::string cmd, ConCmd_Callback cb, AdminFlag adminFlags, std::string description = std::string(), uint64 cmdFlag = FCVAR_NONE);
	void AddCommandListener(std::string cmd, ConCmdListener_Callback cb);

	std::optional<ConCommandRef> Find(const char* name);

	extern CConCmdManager* GetManager();
} // namespace CONCMD
