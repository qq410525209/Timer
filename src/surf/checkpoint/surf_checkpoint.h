#pragma once

#include <surf/api.h>
#include <surf/timer/surf_timer.h>

struct cp_cache_t : timer_snapshot_t {
	Vector m_vecPos = Vector(0.0f, 0.0f, 0.0f);
	QAngle m_vecAng = QAngle(0.0f, 0.0f, 0.0f);
	Vector m_vecVel = Vector(0.0f, 0.0f, 0.0f);

	Vector m_vecLadderNormal = Vector(0.0f, 0.0f, 0.0f);

	MoveType_t m_nMoveType {};
	MoveType_t m_nActualMoveType {};

	bool m_bDucked {};
	bool m_bDucking {};
	f32 m_fDuckTime {};
	f32 m_fDuckSpeed {};

	f32 m_fGravity {};
	f32 m_fSpeed {};
	f32 m_fStamina {};

	i32 m_iFlags {};
	CEntityHandle m_hGroundEntity;

	std::string m_sTargetName;
	std::string m_sDesignerName;
};

class CSurfCheckpointPlugin : CSurfForward, CMovementForward, CCoreForward {
private:
	virtual void OnPluginStart() override;
	virtual void OnClientPutInServer(ISource2GameClients* pClient, CPlayerSlot slot, char const* pszName, int type, uint64 xuid) override;
	virtual void OnClientDisconnect(ISource2GameClients* pClient, CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkID) override;

public:
	void RegisterCommands();

public:
	static void OnPlayerSpawn(IGameEvent* pEvent, const char* szName, bool bDontBroadcast);
};

class CSurfCheckpointService : CSurfBaseService {
public:
	using CSurfBaseService::CSurfBaseService;

	virtual void OnReset() override;

public:
	void OpenCheckpointsMenu();
	void SaveCheckpoint();
	void LoadCheckpoint(const cp_cache_t& cache);

public:
	std::vector<cp_cache_t> m_vCheckpoints;
	i32 m_iCurrentCP {};
};

namespace SURF {
	extern CSurfCheckpointPlugin* CheckpointPlugin();
} // namespace SURF
