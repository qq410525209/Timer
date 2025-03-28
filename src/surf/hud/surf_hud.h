#pragma once

#include <surf/surf_player.h>
#include <core/screentext.h>

class CSurfHudPlugin : CCoreForward, CMovementForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;

private:
	void UpdateHudData(CSurfPlayer* pSurfPlayer);
	void UpdateHudDataPost(CSurfPlayer* pSurfPlayer);
	void UpdateHTML(CSurfPlayer* pSurfPlayer);
	void UpdateScreenText(CSurfPlayer* pSurfPlayer);

public:
	static void OnPlayerSpawn(IGameEvent* pEvent, const char* szName, bool bDontBroadcast);
};

class CSurfHudService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	virtual void OnReset() override;

public:
	int m_iCurrentSpeed;
	int m_iPrevousSpeed;
	std::weak_ptr<CScreenText> m_wpSpeedText;
};

namespace SURF {
	extern CSurfHudPlugin* HudPlugin();
} // namespace SURF
