#pragma once

#include <surf/surf_player.h>
#include <core/screentext.h>

class CSurfHudPlugin : CCoreForward, CMovementForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;

public:
	static void OnPlayerSpawn(IGameEvent* pEvent, const char* szName, bool bDontBroadcast);
};

class CSurfHudService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

public:
	CScreenTextPtr m_pSpeedText;
};

namespace SURF {
	extern CSurfHudPlugin* HudPlugin();
} // namespace SURF
