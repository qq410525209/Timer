#pragma once

#include <movement/movement.h>

class CSurfTimerService;
class CSurfZoneService;

class CSurfPlayer : public CMovementPlayer {
public:
	using CMovementPlayer::CMovementPlayer;

	virtual void Init(int iSlot);

public:
#pragma region rampfix
	bool didTPM {};
	bool overrideTPM {};
	Vector tpmVelocity = vec3_invalid;
	Vector tpmOrigin = vec3_invalid;
	Vector lastValidPlane = vec3_origin;
#pragma endregion

	bool m_bHideLegs {};

#pragma region service

private:
	template<typename T>
	struct CSurfServiceDeleter {
		void operator()(T* ptr) const {
			g_pMemAlloc->Free(ptr);
		}
	};

	template<typename T>
	void InitService(std::unique_ptr<T, CSurfServiceDeleter<T>>& service) {
		if (!service) {
			service.reset(new T(this));
		}
	}

public:
	std::unique_ptr<CSurfTimerService, CSurfServiceDeleter<CSurfTimerService>> m_pTimerService;
	std::unique_ptr<CSurfZoneService, CSurfServiceDeleter<CSurfZoneService>> m_pZoneService;
#pragma endregion

public:
	void EnableGodMode();
	void HideLegs();
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

	// Safe
	CSurfPlayer* ToSurfPlayer(CMovementPlayer* player) {
		return static_cast<CSurfPlayer*>(player);
	}

	// Dont pass by global playermanager
	CSurfPlayer* ToSurfPlayer(CPlayer* player) {
		return static_cast<CSurfPlayer*>(player);
	}
};

class CSurfForward : public CBaseForward<CSurfForward> {
public:
	bool OnTimerStart(CSurfPlayer* player) {
		return true;
	}

	bool OnTimerEnd(CSurfPlayer* player) {
		return true;
	}
};

class CSurfBaseService {
public:
	CSurfBaseService(CSurfPlayer* player) : m_pPlayer(player) {}

	CSurfPlayer* GetPlayer() {
		return m_pPlayer;
	}

private:
	CSurfPlayer* m_pPlayer;
};

namespace SURF {
	extern CSurfPlayerManager* GetPlayerManager();
}
