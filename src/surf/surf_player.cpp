#include <surf/timer/surf_timer.h>
#include <surf/zones/surf_zones.h>
#include <surf/hud/surf_hud.h>

CSurfPlayerManager g_SurfPlayerManager;

CSurfPlayerManager* SURF::GetPlayerManager() {
	return &g_SurfPlayerManager;
}

CMovementPlayerManager* MOVEMENT::GetPlayerManager() {
	return static_cast<CMovementPlayerManager*>(&g_SurfPlayerManager);
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CServerSideClientBase* pClient) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(pClient));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CPlayerPawnComponent* component) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(component));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CBasePlayerController* controller) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(controller));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CBasePlayerPawn* pawn) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(pawn));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CPlayerSlot slot) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(slot));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CEntityIndex entIndex) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(entIndex));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CPlayerUserId userID) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(userID));
}

CSurfPlayer* CSurfPlayerManager::ToPlayer(CSteamID steamid, bool validate) const {
	return static_cast<CSurfPlayer*>(CPlayerManager::ToPlayer(steamid, validate));
}

void CSurfPlayer::Init(int iSlot) {
	CMovementPlayer::Init(iSlot);

	InitService(m_pTimerService);
	InitService(m_pZoneService);
	InitService(m_pHudService);
}

void CSurfPlayer::EnableGodMode() {
	CCSPlayerPawn* pawn = this->GetPlayerPawn();
	if (!pawn) {
		return;
	}
	if (pawn->m_bTakesDamage()) {
		pawn->m_bTakesDamage(false);
	}
}

void CSurfPlayer::HideLegs() {
	CCSPlayerPawn* pawn = this->GetPlayerPawn();
	if (!pawn) {
		return;
	}

	Color& ogColor = pawn->m_clrRender();
	if (this->m_bHideLegs && ogColor.a() == 255) {
		pawn->m_clrRender(Color(255, 255, 255, 254));
	} else if (!this->m_bHideLegs && ogColor.a() != 255) {
		pawn->m_clrRender(Color(255, 255, 255, 255));
	}
}

void SURF::FormatTime(f64 time, char* output, u32 length, bool precise) {
	int roundedTime = RoundFloatToInt(time * 1000); // Time rounded to number of ms

	int milliseconds = roundedTime % 1000;
	roundedTime = (roundedTime - milliseconds) / 1000;
	int seconds = roundedTime % 60;
	roundedTime = (roundedTime - seconds) / 60;
	int minutes = roundedTime % 60;
	roundedTime = (roundedTime - minutes) / 60;
	int hours = roundedTime;

	if (hours == 0) {
		if (precise) {
			snprintf(output, length, "%02i:%02i.%03i", minutes, seconds, milliseconds);
		} else {
			snprintf(output, length, "%i:%02i", minutes, seconds);
		}
	} else {
		if (precise) {
			snprintf(output, length, "%i:%02i:%02i.%03i", hours, minutes, seconds, milliseconds);
		} else {
			snprintf(output, length, "%i:%02i:%02i", hours, minutes, seconds);
		}
	}
}

CUtlString SURF::FormatTime(f64 time, bool precise) {
	char temp[32];
	FormatTime(time, temp, sizeof(temp), precise);
	return CUtlString(temp);
}

void SURF::FormatDiffTime(f64 time, char* output, u32 length, bool precise) {
	char temp[32];
	if (time > 0) {
		FormatTime(time, temp, sizeof(temp), precise);
		V_snprintf(output, length, "+%s", temp);
	} else {
		FormatTime(-time, temp, sizeof(temp), precise);
		V_snprintf(output, length, "-%s", temp);
	}
}

CUtlString SURF::FormatDiffTime(f64 time, bool precise) {
	char temp[32];
	FormatDiffTime(time, temp, sizeof(temp), precise);
	return CUtlString(temp);
}
