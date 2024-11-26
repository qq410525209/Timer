#pragma once

#include <surf/surf_player.h>

class CSurfTimerPlugin : CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPhysicsSimulatePost(CCSPlayerController* pController) override;
};

class CSurfTimerService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	bool m_bTimerRunning {};
	f64 m_fCurrentTime {};
	f64 m_fLastEndTime {};
	f64 m_fLastFalseEndTime {};
	f64 m_fLastStartSoundTime {};
	f64 m_fLastMissedTimeSoundTime {};
	bool m_bValidTime {};

	i32 m_iCurrentStage {};

	bool m_bPaused {};

public:
	void DoTimerStart(bool playSound = true);
	void DoTimerEnd();
};
