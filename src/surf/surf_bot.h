#pragma once

#include <core/playermanager.h>

class CSurfBotReplayService;

class CSurfBot : public CPlayer {
public:
	using CPlayer::CPlayer;

	virtual void Init(int iSlot) override;
	virtual void Reset() override;

public:
	PlayerServicePtr<CSurfBotReplayService> m_pReplayService;
};

class CSurfBotBaseService : public CPlayerService {
public:
	using CPlayerService::CPlayerService;

	virtual CSurfBot* GetPlayer() const override {
		return static_cast<CSurfBot*>(m_pPlayer);
	}
};

class CSurfBotManager : public CPlayerManager {
public:
	CSurfBotManager() {
		for (int i = 0; i < MAXPLAYERS; i++) {
			m_pPlayers[i] = std::make_unique<CSurfBot>(i);
		}
	}

	virtual CSurfBot* ToPlayer(CServerSideClientBase* pClient) const override;
	virtual CSurfBot* ToPlayer(CPlayerPawnComponent* component) const override;
	virtual CSurfBot* ToPlayer(CBasePlayerController* controller) const override;
	virtual CSurfBot* ToPlayer(CBasePlayerPawn* pawn) const override;
	virtual CSurfBot* ToPlayer(CPlayerSlot slot) const override;
	virtual CSurfBot* ToPlayer(CEntityIndex entIndex) const override;
	virtual CSurfBot* ToPlayer(CPlayerUserId userID) const override;
	virtual CSurfBot* ToPlayer(CSteamID steamid, bool validate = false) const override;

	// only fake players
	virtual std::vector<CPlayer*> GetOnlinePlayers() const override;
};

namespace SURF {
	extern CSurfBotManager* GetBotManager();
};
