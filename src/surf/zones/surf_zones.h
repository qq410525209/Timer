#pragma once

#include <surf/surf_player.h>
#include <surf/zones/edit.h>

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

struct ZoneCache_t {
	ZoneCache_t() {
		m_aBeams.fill(CEntityHandle());
		m_iTrack = (ZoneTrack)-1;
		m_iType = (ZoneType)-1;
		m_iData = -1;
	}

	std::array<CHandle<CBeam>, 12> m_aBeams;
	ZoneTrack m_iTrack;
	ZoneType m_iType;
	int m_iData;
};

class CZoneHandle : public CHandle<CBaseTrigger> {
	using CHandle::CHandle;
};

template<>
struct std::hash<CZoneHandle> {
	size_t operator()(const CZoneHandle& handle) const {
		return std::hash<uint32>()(handle.ToInt());
	}
};

class CSurfZoneService : public CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	CZoneEditProperty m_ZoneEdit;

	virtual void Reset() override;

public:
	void AddZone(const Vector& vecMin, const Vector& vecMax);
	void EditZone(CCSPlayerPawnBase* pawn, const CPlayerButton* buttons);
	void CreateZone(const Vector& vecMin, const Vector& vecMax, std::array<CHandle<CBeam>, 12>& out);
	CBaseEntity* CreateNormalZone(const Vector& vecMins, const Vector& vecMaxs);

public:
	static void CreatePoints2D(const Vector& vecMin, const Vector& vecMax, Vector out[4]);
	static void CreatePoints3D(const Vector& vecMin, const Vector& vecMax, Vector out[8]);
	static void FillBoxMinMax(Vector& vecMin, Vector& vecMax, bool resize = false);
};

class CSurfZonePlugin : CMovementForward, CCoreForward {
public:
	std::unordered_map<CZoneHandle, ZoneCache_t> m_hZones;

public:
	std::optional<ZoneCache_t> FindZone(CBaseEntity* pEnt);

private:
	virtual void OnPluginStart() override;
	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CPlayerButton* buttons, const float (&vec)[3], const QAngle& viewAngles,
									const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) override;
};

CSurfZonePlugin* SurfZonePlugin();
