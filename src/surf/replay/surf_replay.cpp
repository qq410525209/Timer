#include "surf_replay.h"

CSurfReplayPlugin g_SurfReplay;

bool CSurfReplayPlugin::OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButton* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
									   int& tickcount, int& seed, int (&mouse)[2]) {
	if (!pawn->IsAlive()) {
		return true;
	}

	auto bBot = pawn->IsBot();
	if (bBot) {
	}

	return true;
}

void CSurfReplayPlugin::OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3],
										   const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {
	if (!pawn->IsAlive()) {
		return;
	}
}

void CSurfReplayService::Reset() {}
