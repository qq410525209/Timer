#pragma once

class CSurfZoneService;

enum ZoneTrack {
	Track_Main,
	Track_Bonus,
	TRACKS_SIZE
};

enum ZoneType {
	Zone_Start,
	Zone_End,
	Zone_Stage,
	Zone_Checkpoint,
	Zone_Stop,     // stops the player's timer
	Zone_Teleport, // teleports to a defined point
	Zone_Mark,     // do nothing, mainly used for marking trigger;
	ZONETYPES_SIZE
};

enum ZoneEditStep {
	EditStep_None,   // 0 - nothing
	EditStep_First,  // 1 - wait for E tap to setup first coord
	EditStep_Second, // 2 - wait for E tap to setup second coord
	EditStep_Third,  // 3 - wait for E tap to setup height
	EditStep_Final   // 4 - confirm
};

struct ZoneData_t {
	ZoneData_t() {
		Reset();
	}

	ZoneData_t(const ZoneData_t& other) {
		m_iTrack = other.m_iTrack;
		m_iType = other.m_iType;
		m_iValue = other.m_iValue;
		m_vecMins = other.m_vecMins;
		m_vecMaxs = other.m_vecMaxs;
		m_vecDestination = other.m_vecDestination;
		m_iFlag = other.m_iFlag;
		m_iHookHammerid = other.m_iHookHammerid;
		m_sHookName = other.m_sHookName;
		m_fLimitSpeed = other.m_fLimitSpeed;
	}

	void Reset() {
		m_iTrack = (ZoneTrack)-1;
		m_iType = (ZoneType)-1;
		m_iValue = -1;
		m_vecMins = Vector(0.0f, 0.0f, 0.0f);
		m_vecMaxs = Vector(0.0f, 0.0f, 0.0f);
		m_vecDestination = Vector(0.0f, 0.0f, 0.0f);
		m_iFlag = -1;
		m_iHookHammerid = -1;
		m_sHookName = std::string();
		m_fLimitSpeed = -1.0f;
	}

	ZoneTrack m_iTrack;
	ZoneType m_iType;
	int m_iValue;
	Vector m_vecMins;
	Vector m_vecMaxs;
	Vector m_vecDestination;
	int m_iFlag;
	int m_iHookHammerid;
	std::string m_sHookName;
	float m_fLimitSpeed;
};

struct CZoneEditProperty : ZoneData_t {
	using ZoneData_t::ZoneData_t;

	void Init(CSurfZoneService* outer);
	void Reset();

	void StartEditZone();
	void CreateEditZone(const Vector& playerAim);
	void UpdateZone(const Vector& playerAim);
	void CreateZone2D(const Vector points[4], std::vector<CHandle<CBeam>>& out);
	void CreateZone3D(const Vector points[8], std::vector<CHandle<CBeam>>& out);
	void UpdateZone2D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax);
	void UpdateZone3D(const std::vector<CHandle<CBeam>>& vBeams, const Vector& vecMin, const Vector& vecMax);
	void ClearBeams();

	CSurfZoneService* m_pOuter;
	bool m_bEnabled;
	ZoneEditStep m_iStep;
	std::vector<CHandle<CBeam>> m_vBeam;

	static constexpr const int m_iZonePairs2D[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
	static constexpr const int m_iZonePairs3D[][2] = {{0, 2}, {2, 6}, {6, 4}, {4, 0}, {0, 1}, {3, 1}, {3, 2}, {3, 7}, {5, 1}, {5, 4}, {6, 7}, {7, 5}};
};
