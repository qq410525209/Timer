#pragma once

#include <movement/movement.h>

class CSurfTimerService;
class CSurfZoneService;
class CSurfHudService;
class CSurfReplayService;
class CSurfMiscService;
class CSurfGlobalAPIService;
class CSurfCheckpointService;

class CSurfPlayer : public CMovementPlayer {
public:
	using CMovementPlayer::CMovementPlayer;

	virtual void Init(int iSlot) override;
	virtual void Reset() override;

public:
	PlayerServicePtr<CSurfTimerService> m_pTimerService;
	PlayerServicePtr<CSurfZoneService> m_pZoneService;
	PlayerServicePtr<CSurfHudService> m_pHudService;
	PlayerServicePtr<CSurfReplayService> m_pReplayService;
	PlayerServicePtr<CSurfMiscService> m_pMiscService;
	PlayerServicePtr<CSurfGlobalAPIService> m_pGlobalAPIService;
	PlayerServicePtr<CSurfCheckpointService> m_pCheckpointService;

public:
	bool m_bJustTeleported;
};

class CSurfPlayerService : public CPlayerService {
public:
	using CPlayerService::CPlayerService;

	virtual CSurfPlayer* GetPlayer() const override {
		return static_cast<CSurfPlayer*>(m_pPlayer);
	}

public:
	virtual void PlayErrorSound() const;
};

class CSurfPlayerManager : public CMovementPlayerManager {
public:
	CSurfPlayerManager() {
		for (int i = 0; i < MAXPLAYERS; i++) {
			m_pPlayers[i] = std::make_unique<CSurfPlayer>(i);
		}
	}

	virtual CSurfPlayer* ToPlayer(CServerSideClientBase* pClient) const override;
	virtual CSurfPlayer* ToPlayer(CPlayerPawnComponent* component) const override;
	virtual CSurfPlayer* ToPlayer(CBasePlayerController* controller) const override;
	virtual CSurfPlayer* ToPlayer(CBasePlayerPawn* pawn) const override;
	virtual CSurfPlayer* ToPlayer(CPlayerSlot slot) const override;
	virtual CSurfPlayer* ToPlayer(CEntityIndex entIndex) const override;
	virtual CSurfPlayer* ToPlayer(CPlayerUserId userID) const override;
	virtual CSurfPlayer* ToPlayer(CSteamID steamid, bool validate = false) const override;

	// fake players are excluded
	virtual std::vector<CPlayer*> GetOnlinePlayers() const override;

private:
	virtual void OnEntitySpawned(CEntityInstance* pEntity) override;
};

namespace SURF {
	extern CSurfPlayerManager* GetPlayerManager();

	inline const Vector ZERO_VEC = {0.0f, 0.0f, 0.0f};
	inline const QAngle ZERO_ANG = {0.0f, 0.0f, 0.0f};
} // namespace SURF
