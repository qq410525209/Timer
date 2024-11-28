#pragma once

#include <movement/movement.h>
#include <list>

class CSurfPlayer;
class CSurfTimerService;
class CSurfZoneService;
class CSurfHudService;

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

	CSurfPlayer* GetPlayer() const {
		return m_pPlayer;
	}

public:
	// forwards
	// clang-format off
	virtual void Reset() {}
	virtual void OnServiceStartup() {}

	// clang-format on

private:
	CSurfPlayer* m_pPlayer;
};

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
	struct ServiceDeleter {
		void operator()(void* ptr) const {
			g_pMemAlloc->Free(ptr);
		}
	};

	template<typename T>
	void InitService(std::unique_ptr<T, ServiceDeleter>& service) {
		static_assert(std::is_base_of<CSurfBaseService, T>::value, "T must be derived from CSurfBaseService");

		if (!service) {
			service.reset(new T(this));
			reinterpret_cast<CSurfBaseService*>(service.get())->OnServiceStartup();
		}
	}

public:
	std::unique_ptr<CSurfTimerService, ServiceDeleter> m_pTimerService;
	std::unique_ptr<CSurfZoneService, ServiceDeleter> m_pZoneService;
	std::unique_ptr<CSurfHudService, ServiceDeleter> m_pHudService;
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

namespace SURF {
	extern CSurfPlayerManager* GetPlayerManager();

	void FormatTime(f64 time, char* output, u32 length, bool precise = true);
	CUtlString FormatTime(f64 time, bool precise = true);
	void FormatDiffTime(f64 time, char* output, u32 length, bool precise = true);
	CUtlString FormatDiffTime(f64 time, bool precise = true);
} // namespace SURF
