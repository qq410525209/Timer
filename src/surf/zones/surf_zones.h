#pragma once

#include <surf/surf_player.h>

enum ZoneEditStep {
	EditStep_None,   // 0 - nothing
	EditStep_First,  // 1 - wait for E tap to setup first coord
	EditStep_Second, // 2 - wait for E tap to setup second coord
	EditStep_Third,  // 3 - wait for E tap to setup height
	EditStep_Final   // 4 - confirm
};

class CSurfZoneService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	ZoneEditStep m_iEditStep = EditStep_None;

	virtual void Reset() override;
};

class CSurfZonePlugin : CMovementForward, CCoreForward {
private:
	void DrawEditZone(CCSPlayerPawnBase* pawn);
	void CreateZonePoints(const Vector& vecMin, const Vector& vecMax, Vector out[8]);
	void DrawZone(const Vector points[8], bool flat = false);

private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3], const int& weapon,
									const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;
};
