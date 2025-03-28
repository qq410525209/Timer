#pragma once

#include <surf/api.h>

struct timer_snapshot_t {
	f64 m_fCurrentTime {};
	bool m_bTimerRunning {};
	bool m_bPracticeMode {};
	bool m_bPaused {};

	ZoneTrack m_iTrack;

	i32 m_iCurrentStage {};
	i32 m_iCurrentCP {};
	i32 m_iLastStage {};
	i32 m_iLastCP {};
};

class CSurfTimerPlugin : CSurfForward, CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPhysicsSimulatePost(CCSPlayerController* pController) override;
	virtual bool OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* player) override;
	virtual bool OnStayZone(const ZoneCache_t& zone, CSurfPlayer* player) override;

private:
	void RegisterCommand();
};

class CSurfTimerService : CSurfBaseService, public timer_snapshot_t {
public:
	using CSurfBaseService::CSurfBaseService;

	void DoTimerStart(bool playSound = true);
	void DoTimerStop();
	void DoTimerFinish();
	void DoTimerPause();
	void DoTimerResume();

	void BuildSnapshot(timer_snapshot_t& buffer) const;
	void FromSnapshot(const timer_snapshot_t& snapshot);

public:
	f64 m_fLastEndTime {};
	f64 m_fLastFalseEndTime {};
	f64 m_fLastStartSoundTime {};
	f64 m_fLastMissedTimeSoundTime {};
	bool m_bValidTime {};

	Vector m_vecPausePos = Vector(0.0f, 0.0f, 0.0f);
	QAngle m_vecPauseAng = QAngle(0.0f, 0.0f, 0.0f);
	Vector m_vecPauseVel = Vector(0.0f, 0.0f, 0.0f);
};

namespace SURF {
	extern CSurfTimerPlugin* TimerPlugin();

	void FormatTime(f64 time, char* output, u32 length, bool precise = true);
	CUtlString FormatTime(f64 time, bool precise = true);
	void FormatDiffTime(f64 time, char* output, u32 length, bool precise = true);
	CUtlString FormatDiffTime(f64 time, bool precise = true);
} // namespace SURF
