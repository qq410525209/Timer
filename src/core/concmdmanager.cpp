#include "concmdmanager.h"
#include "concmdmanager.h"
#include <utils/utils.h>

CONCMD::CConCmdManager g_manager;

CONCMD::CConCmdManager* CONCMD::GetManager() {
	return &g_manager;
}

static void HandleSrvCommand(const CCommand& pCommand, const std::wstring& wCommand) {
	auto manager = CONCMD::GetManager();
	auto it = manager->m_umSrvCmds.find(wCommand);
	if (it == manager->m_umSrvCmds.end()) {
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
	auto manager = CONCMD::GetManager();
	auto it = manager->m_umConCmds.find(wCommand);
	if (it == manager->m_umConCmds.end()) {
		return;
	}

	std::vector<std::string> vArgs;

	if (sayCommand) {
		if (spaceFound) {
			std::string sSayContent(pCommand.Arg(1));
			size_t start = sSayContent.find(' ') + 1;
			size_t contentSize = sSayContent.size();
			while (start < contentSize) {
				if (sSayContent[start] == ' ') {
					++start;
					continue;
				}

				size_t end = sSayContent.find(' ', start);
				if (end == std::string::npos) {
					vArgs.emplace_back(sSayContent.substr(start));
					break;
				}

				vArgs.emplace_back(sSayContent.substr(start, end - start));
				start = end + 1;
			}
		}
	} else {
		for (int i = 1; i < pCommand.ArgC(); i++) {
			vArgs.emplace_back(pCommand.Arg(i));
		}
	}

	auto& vCmdInfo = it->second;
	for (const auto& info : vCmdInfo) {
		if (info.adminFlags != AdminFlag::None) {
			/*if (!AdminManager()->CheckAccess(pController->GetSteamIDAsUInt64(), info.adminFlags)) {
				pController->ClientPrint(sayCommand ? HUD_PRINTTALK : HUD_PRINTCONSOLE, "Access denied");
				continue;
			}*/
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
		if (commandSymbol != L'!' && commandSymbol != 65281 && /*full-width !*/
			commandSymbol != L'.' && commandSymbol != 12290 && /*full-width .*/
			commandSymbol != L'/') {
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
