#pragma once

#include <pch.h>
#include <surf/api.h>
#include <surf/surf_bot.h>

struct ReplayFrame_t {
	QAngle ang;
	Vector pos;
	CPlayerButton buttons;
	uint32 flags;
	MoveType_t mt;
};

class CSurfReplayService : CSurfBaseService {
private:
	virtual void OnReset() override;

public:
	using CSurfBaseService::CSurfBaseService;

	void StartRecord();
	void DoRecord(CCSPlayerPawn* pawn, const CPlayerButton& buttons, const QAngle& viewAngles);
	void SaveRecord();

public:
	bool m_bEnabled;
	std::vector<ReplayFrame_t> m_vReplayFrames;
};

class CSurfBotReplayService : CSurfBotBaseService {
public:
	using CSurfBotBaseService::CSurfBotBaseService;

public:
	void DoPlayback(CCSPlayerPawn* pPawn, CInButtonState& buttons);

public:
	i32 m_iCurrentTick;
};

class CSurfReplayPlugin : CSurfForward, CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnEntitySpawned(CEntityInstance* pEntity) override;

	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pPawn, CInButtonState& buttons, float (&vec)[3], QAngle& viewAngles, int& weapon, int& cmdnum, int& tickcount, int& seed, int (&mouse)[2]) override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;

	virtual bool OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) override;
	virtual bool OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) override;

private:
	void HookEvents();

public:
	std::array<std::vector<ReplayFrame_t>, SURF_MAX_TRACK> m_aTrackReplays;
	std::array<std::vector<ReplayFrame_t>, SURF_MAX_STAGE> m_aStageReplays;
};

namespace SURF {
	extern CSurfReplayPlugin* ReplayPlugin();

	namespace REPLAY {
		namespace HOOK {
			bool HookBotTrigger(CBaseEntity* pSelf, CBaseEntity* pOther);
		} // namespace HOOK
	} // namespace REPLAY
} // namespace SURF
