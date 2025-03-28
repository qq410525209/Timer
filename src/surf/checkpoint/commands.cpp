#include <surf/checkpoint/surf_checkpoint.h>
#include <surf/surf_player.h>
#include <core/concmdmanager.h>

CCMD_CALLBACK(Command_Checkpoints) {
	CSurfPlayer* pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pSurfPlayer) {
		return;
	}

	auto& pCPService = pSurfPlayer->m_pCheckpointService;
}

CCMD_CALLBACK(Command_Save) {
	CSurfPlayer* pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pSurfPlayer) {
		return;
	}

	auto& pCPService = pSurfPlayer->m_pCheckpointService;
}

CCMD_CALLBACK(Command_Tele) {
	CSurfPlayer* pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pSurfPlayer) {
		return;
	}

	auto& pCPService = pSurfPlayer->m_pCheckpointService;
}

CCMDLISTENER_CALLBACK(Command_Jointeam) {
	CSurfPlayer* pSurfPlayer = SURF::GetPlayerManager()->ToPlayer(pController);
	if (!pSurfPlayer) {
		return true;
	}

	auto& pCPService = pSurfPlayer->m_pCheckpointService;

	return true;
}

void CSurfCheckpointPlugin::RegisterCommands() {
	CONCMD::RegConsoleCmd("sm_cpmenu", Command_Checkpoints, "Opens the checkpoints menu.");
	CONCMD::RegConsoleCmd("sm_cps", Command_Checkpoints, "Opens the checkpoints menu. Alias for sm_cpmenu.");
	CONCMD::RegConsoleCmd("sm_cpcaidan", Command_Checkpoints, "Opens the checkpoints menu. Alias for sm_cpmenu.");
	CONCMD::RegConsoleCmd("sm_checkpoint", Command_Checkpoints, "Opens the checkpoints menu. Alias for sm_cpmenu.");
	CONCMD::RegConsoleCmd("sm_checkpoints", Command_Checkpoints, "Opens the checkpoints menu. Alias for sm_cpmenu.");
	CONCMD::RegConsoleCmd("sm_save", Command_Save, "Saves checkpoint.");
	CONCMD::RegConsoleCmd("sm_saveloc", Command_Save, "Saves checkpoint.");
	CONCMD::RegConsoleCmd("sm_cp", Command_Save, "Saves checkpoint. Alias for sm_save.");
	CONCMD::RegConsoleCmd("sm_tele", Command_Tele, "Teleports to checkpoint. Usage: sm_tele [number]");
	CONCMD::RegConsoleCmd("sm_prac", Command_Tele, "Teleports to checkpoint. Usage: sm_tele [number]. Alias of sm_tele.");
	CONCMD::RegConsoleCmd("sm_practice", Command_Tele, "Teleports to checkpoint. Usage: sm_tele [number]. Alias of sm_tele.");

	CONCMD::AddCommandListener("jointeam", Command_Jointeam);
}
