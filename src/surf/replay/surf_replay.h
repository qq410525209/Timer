#pragma once

#include <pch.h>
#include <surf/api.h>

struct ReplayFrame_t {
	QAngle ang;
	Vector pos;
	uint64 buttons;
	uint32 flags;
	MoveType_t mt;
};

class CSurfReplayService : CSurfBaseService {
	using CSurfBaseService::CSurfBaseService;

	virtual void Reset() override;

public:
	bool m_bEnabled;
	std::vector<ReplayFrame_t> m_vReplayFrames;

public:
	void StartRecord();
	void DoRecord(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const QAngle& viewAngles);
	void SaveRecord();
};

class CSurfReplayPlugin : CSurfForward, CMovementForward, CCoreForward {
public:
	std::unordered_map<uint32_t, std::vector<ReplayFrame_t>> m_umTrackReplays;
	std::unordered_map<uint32_t, std::vector<ReplayFrame_t>> m_umStageReplays;
	std::array<uint32_t, MAXPLAYERS> m_iCurrentTick;

private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const float (&vec)[3], const QAngle& viewAngles,
									const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;

	virtual bool OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* player) override;
	virtual bool OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* player) override;
};

extern CSurfReplayPlugin* GetReplayPlugin();
