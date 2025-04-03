#pragma once

constexpr auto SURF_WORKSHOP_ADDONS_ID = "3377250885";
constexpr auto SURF_WORKSHOP_ADDONS_SNDEVENT_FILE = "soundevents/surf.vsndevts";

constexpr auto SURF_SND_ERROR = "Buttons.snd8";
constexpr auto SURF_SND_SAVE_CP = "surf.savecp";
constexpr auto SURF_SND_TELE_CP = "Buttons.snd9";

#include <surf/zones/surf_zones.h>

class CSurfForward : public CBaseForward<CSurfForward> {
public:
	virtual bool OnTimerStart(CSurfPlayer* pPlayer) {
		return true;
	}

	virtual bool OnTimerPause(CSurfPlayer* pPlayer) {
		return true;
	}

	virtual bool OnTimerResume(CSurfPlayer* pPlayer) {
		return true;
	}

	virtual bool OnTimerStop(CSurfPlayer* pPlayer) {
		return true;
	}

	virtual void OnTimerStopPost(CSurfPlayer* pPlayer) {}

	virtual bool OnTimerFinish(CSurfPlayer* pPlayer) {
		return true;
	}

	virtual void OnTimerFinishPost(CSurfPlayer* pPlayer) {}

	virtual bool OnEnterZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
		return true;
	}

	virtual bool OnStayZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
		return true;
	}

	virtual bool OnLeaveZone(const ZoneCache_t& zone, CSurfPlayer* pPlayer) {
		return true;
	}
};

class CSurfGlobalAPIForward : public CBaseForward<CSurfGlobalAPIForward> {
public:
	virtual void OnGlobalInit() {}

	virtual void OnGlobalZoneHelperInit() {}

	virtual void OnGlobalMapValidated() {}
};
