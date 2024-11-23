#include "surf_replay.h"

CSurfReplay g_SurfReplay;

bool CSurfReplay::OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButtonState* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
								 int& tickcount, int& seed, int (&mouse)[2]) {
	auto bBot = pawn->IsBot();
	if (bBot) {
		SURF_ASSERT(false);
	}

	return true;
}
