#include "concmdmanager.h"
#include <utils/utils.h>

CONCMD::CConCmdManager g_manager;

CONCMD::CConCmdManager* CONCMD::GetManager() {
	return &g_manager;
}

void CONCMD::RegServerCmd(std::string cmd, SrvCmd_Callback cb, std::string description, int64_t cmdFlag) {
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

	if (!cmd.starts_with("sm_")) {
		cmd = "sm_" + cmd;
	}

	CConCmdManager::CSrvCmdInfo info;
	info.callback = cb;
	info.description = description;
	info.cmdFlags = cmdFlag;
	g_manager.m_umSrvCmds[UTIL::ToWideString(cmd.c_str())].emplace_back(info);

	static FnCommandCallback_t nullcb;
	ConCommandRefAbstract ref;
	ConCommand command = ConCommand(&ref, cmd.c_str(), nullcb, description.c_str(), cmdFlag);
	g_pCVar->RegisterConCommand(&command, 4);
}

void CONCMD::RegConsoleCmd(std::string cmd, ConCmd_Callback cb, std::string description, int64_t cmdFlag) {
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

	if (!cmd.starts_with("sm_")) {
		cmd = "sm_" + cmd;
	}

	CConCmdManager::CConCmdInfo info;
	info.callback = cb;
	info.description = description;
	info.adminFlags = AdminFlag::None;
	info.cmdFlags = cmdFlag;
	g_manager.m_umConCmds[UTIL::ToWideString(cmd.c_str())].emplace_back(info);

	static FnCommandCallback_t nullcb;
	ConCommandRefAbstract ref;
	ConCommand command = ConCommand(&ref, cmd.c_str(), nullcb, description.c_str(), cmdFlag | FCVAR_CLIENT_CAN_EXECUTE);
	g_pCVar->RegisterConCommand(&command, 4);
}

void CONCMD::RegAdminCmd(std::string cmd, ConCmd_Callback cb, AdminFlag adminFlags, std::string description, int64_t cmdFlag) {
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

	if (!cmd.starts_with("sm_")) {
		cmd = "sm_" + cmd;
	}

	CConCmdManager::CConCmdInfo info;
	info.callback = cb;
	info.description = description;
	info.adminFlags = adminFlags;
	info.cmdFlags = cmdFlag;
	g_manager.m_umConCmds[UTIL::ToWideString(cmd.c_str())].emplace_back(info);

	static FnCommandCallback_t nullcb;
	ConCommandRefAbstract ref;
	ConCommand command = ConCommand(&ref, cmd.c_str(), nullcb, description.c_str(), cmdFlag | FCVAR_CLIENT_CAN_EXECUTE);
	g_pCVar->RegisterConCommand(&command, 4);
}

static void ParseCommandArgs(const std::string& sRaw, std::vector<std::string>& vArgs) {
	std::string currentArg;
	bool inQuotes = false;

	size_t start = sRaw.find(' ');
	if (start == std::string::npos) {
		return;
	}

	++start;

	while (start < sRaw.size() && sRaw[start] == ' ') {
		++start;
	}

	size_t rawSize = sRaw.size();
	for (size_t i = start; i < rawSize; ++i) {
		char c = sRaw[i];

		if (c == '"') {
			inQuotes = !inQuotes;

			if (!inQuotes) {
				vArgs.emplace_back(std::move(currentArg));
				currentArg = "";
			}
		} else if (c == ' ' && !inQuotes) {
			if (!currentArg.empty()) {
				vArgs.emplace_back(std::move(currentArg));
				currentArg = "";
			}
		} else {
			currentArg += c;
		}
	}

	if (!currentArg.empty()) {
		vArgs.emplace_back(std::move(currentArg));
	}
}

static void HandleSrvCommand(const CCommand& pCommand, const std::wstring& wCommand) {
	auto it = g_manager.m_umSrvCmds.find(wCommand);
	if (it == g_manager.m_umSrvCmds.end()) {
		return;
	}

	std::vector<std::string> vArgs;
	for (int i = 1; i < pCommand.ArgC(); i++) {
		vArgs.emplace_back(pCommand.Arg(i));
	}

	auto& vCmdInfo = it->second;
	for (const auto& info : vCmdInfo) {
		info.callback(vArgs);
	}
}

static void HandleConCommand(CCSPlayerController* pController, const CCommand& pCommand, const std::wstring& wCommand, bool sayCommand,
							 bool spaceFound) {
	auto it = g_manager.m_umConCmds.find(wCommand);
	if (it == g_manager.m_umConCmds.end()) {
		return;
	}

	std::vector<std::string> vArgs;

	if (sayCommand) {
		if (spaceFound) {
			std::string sRawContent(pCommand.ArgS());
			ParseCommandArgs(sRawContent, vArgs);
		}
	} else {
		for (int i = 1; i < pCommand.ArgC(); i++) {
			vArgs.emplace_back(pCommand.Arg(i));
		}
	}

	auto& vCmdInfo = it->second;
	for (const auto& info : vCmdInfo) {
		if (info.adminFlags != AdminFlag::None) {
			if (!ADMIN::CheckAccess(pController->m_steamID(), info.adminFlags)) {
				sayCommand ? UTIL::PrintChat(pController, "Access denied") : UTIL::PrintConsole(pController, "Access denied");
				continue;
			}
		}

		info.callback(pController, vArgs);
	}
}

void CONCMD::CConCmdManager::OnDispatchConCommand(ICvar* pCvar, ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args) {
	auto pszCommand = args.Arg(0);
	if (!pszCommand || !pszCommand[0]) {
		return;
	}

	auto pController = (CCSPlayerController*)UTIL::GetController(ctx.GetPlayerSlot());
	if (!pController) {
		HandleSrvCommand(args, UTIL::ToWideString(pszCommand));
		return;
	}

	if (!strcmp(pszCommand, "say") || !strcmp(pszCommand, "say_team")) {
		auto pszSayContent = args.Arg(1);
		if (!pszSayContent || !pszSayContent[0]) {
			return;
		}

		auto wSayContent = UTIL::ToWideString(pszSayContent);
		wchar_t commandSymbol = wSayContent[0];
		if (commandSymbol != L'!' && commandSymbol != 65281 && commandSymbol != L'.' && commandSymbol != 12290 && commandSymbol != L'/') {
			return;
		}

		size_t spacePos = wSayContent.find(L' ');
		bool bSpaceFound = (spacePos != std::wstring::npos);
		std::wstring wCommand = bSpaceFound ? wSayContent.substr(1, spacePos - 1) : wSayContent.substr(1);
		if (wCommand.empty()) {
			return;
		}

		if (bSpaceFound) {
			wCommand.append(1, L'\0');
		}

		wCommand = L"sm_" + wCommand;

		HandleConCommand(pController, args, wCommand, true, bSpaceFound);

		if (commandSymbol == L'/') {
			return;
		}
	} else {
		HandleConCommand(pController, args, UTIL::ToWideString(pszCommand), false, false);
	}
}
