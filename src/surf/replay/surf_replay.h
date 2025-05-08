#pragma once

#include <pch.h>
#include <surf/api.h>
#include <surf/surf_bot.h>
#include <utils/ctimer.h>

using ReplayArray_t = std::vector<replay_frame_t>;

struct replay_run_info_t {
	std::time_t timestamp;
	uint64 steamid;
	f64 time;
	i8 style;
	ZoneTrack track;
	i32 stage;
	i32 preframes;
	i32 framelength;
	i32 postframes;
};

struct replay_file_header_t {
	std::string format = SURF_REPLAY_FORMAT;
	u8 version = SURF_REPLAY_VERSION;
	std::string map;
	f32 tickrate;
	replay_run_info_t info;

	void ReadFromStream(std::ifstream& in);
	void WriteToStream(std::ofstream& out) const;
};

class CSurfReplayService : CSurfPlayerService {
private:
	virtual void OnReset() override;

public:
	using CSurfPlayerService::CSurfPlayerService;

	void OnStart_Recording();
	void OnTimerFinishPost_SaveRecording();
	void FinishGrabbingTrackPostFrames();

	void StartRecord();
	void DoRecord(CCSPlayerPawn* pawn, const CPlayerButton& buttons, const QAngle& viewAngles);
	void SaveRecord();
	void ClearFrames();

public:
	bool m_bEnabled;
	ReplayArray_t m_vCurrentFrames;
	i32 m_iCurrentFrame;
	i32 m_iTrackPrerunFrame;
	i32 m_iStagePrerunFrame;
	i32 m_iTrackFinishFrame;
	i32 m_iStageFinishFrame;
	bool m_bGrabbingTrackPostFrame;
	bool m_bGrabbingStagePostFrame;

	CTimerHandle m_hPostFrameTimer;
};

class CSurfBotReplayService : CSurfBotService {
public:
	using CSurfBotService::CSurfBotService;

	virtual void OnInit() override;
	virtual void OnReset() override;

public:
	void Init();
	void DoPlayback(CCSPlayerPawn* pPawn, CInButtonState& buttons);

	bool IsReplayBot() const {
		return m_bReplayBot;
	}

	bool IsStageBot() const {
		return m_iCurrentStage != -1;
	}

	bool IsTrackBot() const {
		return m_iCurrentTrack != -1;
	}

public:
	bool m_bReplayBot;
	i32 m_iCurrentTick;
	i32 m_iCurrentStage;
	ZoneTrack m_iCurrentTrack;
};

class CSurfReplayPlugin : CSurfForward, CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnEntitySpawned(CEntityInstance* pEntity) override;

	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pPawn, CInButtonState& buttons, float (&vec)[3], QAngle& viewAngles, int& weapon, int& cmdnum, int& tickcount, int& seed, int (&mouse)[2]) override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pPawn, const CInButtonState& buttons, const float (&vec)[3], const QAngle& viewAngles, const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;

	virtual bool OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) override;
	virtual bool OnStayZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) override;
	virtual bool OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) override;
	virtual void OnTimerFinishPost(CSurfPlayer* pPlayer) override;

public:
	void AsyncWriteReplayFile(const replay_run_info_t& info, const ReplayArray_t& vFrames);
	bool ReadReplayFile(const std::string_view path, ReplayArray_t& out);

private:
	void HookEvents();

public:
	ConVarRefAbstract* m_cvarTrackPreRunTime;
	ConVarRefAbstract* m_cvarTrackPostRunTime;
	ConVarRefAbstract* m_cvarStagePreRunTime;
	ConVarRefAbstract* m_cvarStagePostRunTime;
	ConVarRefAbstract* m_cvarPreRunAlways;

	std::array<ReplayArray_t, SURF_MAX_TRACK> m_aTrackReplays;
	std::array<ReplayArray_t, SURF_MAX_STAGE> m_aStageReplays;
};

namespace SURF {
	extern CSurfReplayPlugin* ReplayPlugin();

	namespace REPLAY {
		namespace HOOK {
			bool OnBotTrigger(CBaseEntity* pSelf, CBaseEntity* pOther);
		} // namespace HOOK
	} // namespace REPLAY
} // namespace SURF
