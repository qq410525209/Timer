#pragma once

#include <surf/surf_player.h>

enum ZoneEditStep {
	EditStep_None,   // 0 - nothing
	EditStep_First,  // 1 - wait for E tap to setup first coord
	EditStep_Second, // 2 - wait for E tap to setup second coord
	EditStep_Third,  // 3 - wait for E tap to setup height
	EditStep_Final   // 4 - confirm
};

class CSurfZoneService;

struct CZoneEditProperty {
	void Init(CSurfZoneService* outer);
	void Reset();

	CSurfZoneService* m_pOuter;
	bool m_bEnabled;
	ZoneEditStep m_iStep;
	Vector m_vecMins;
	Vector m_vecMaxs;
	std::vector<CHandle<CBeam>> m_vBeam;

	static constexpr const int m_iZonePairs2D[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
	static constexpr const int m_iZonePairs3D[][2] = {{0, 2}, {2, 6}, {6, 4}, {4, 0}, {0, 1}, {3, 1}, {3, 2}, {3, 7}, {5, 1}, {5, 4}, {6, 7}, {7, 5}};

	void StartEditZone();
	void CreateEditZone(const Vector& playerAim);
	void UpdateZone(const Vector& playerAim);
	void CreateZone2D(const Vector points[4], std::vector<CHandle<CBeam>>& out);
	void CreateZone3D(const Vector points[8], std::vector<CHandle<CBeam>>& out);
	void UpdateZone2D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax);
	void UpdateZone3D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax);
	void ClearBeams();
};

struct ZoneCache_t {
	CHandle<CBaseTrigger> m_hZone {};
	std::array<CHandle<CBeam>, 12> m_aBeams {};
};

class CSurfZoneService : public CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	CZoneEditProperty m_ZoneEdit;
	std::vector<ZoneCache_t> m_hZones;

	virtual void Reset() override;

public:
	void AddZone(const Vector& vecMin, const Vector& vecMax);
	void EditZone(CCSPlayerPawnBase* pawn, const CInButton* buttons);
	void CreateZone(const Vector& vecMin, const Vector& vecMax, std::array<CHandle<CBeam>, 12>& out);
	CBaseEntity* CreateNormalZone(const Vector& vecMins, const Vector& vecMaxs);

public:
	static void CreatePoints2D(const Vector& vecMin, const Vector& vecMax, Vector out[4]);
	static void CreatePoints3D(const Vector& vecMin, const Vector& vecMax, Vector out[8]);
	static void FillBoxMinMax(Vector& vecMin, Vector& vecMax, bool resize = false);
};

class CSurfZonePlugin : CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButton* buttons, const float (&vec)[3], const float (&angles)[3], const int& weapon,
									const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;
};

CSurfZonePlugin* SurfZonePlugin();
