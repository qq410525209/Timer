#pragma once

#include <pch.h>
#include <sdk/serversideclient.h>
#include <sdk/entity/observer.h>
#include <sdk/entity/ccsplayercontroller.h>
#include <core/forwards.h>

class CPlayer {
public:
	CPlayer(int slot) : m_iSlot(slot) {}

	virtual ~CPlayer() {}

	virtual void Reset() {
		unauthenticatedSteamID = k_steamIDNil;
	}

	virtual CCSPlayerController* GetController() const;
	virtual CBasePlayerPawn* GetCurrentPawn() const;
	virtual CCSPlayerPawn* GetPlayerPawn() const;
	virtual CCSObserverPawn* GetObserverPawn() const;
	virtual CServerSideClient* GetClient() const;

	// Returns true if this client has been fully authenticated by Steam
	virtual bool IsAuthenticated() const;
	virtual bool IsConnected() const;
	virtual bool IsInGame() const;
	virtual bool IsFakeClient() const;
	virtual bool IsSourceTV() const;
	virtual bool IsValidClient() const;
	virtual const char* GetName() const;
	virtual const char* GetIpAddress() const;
	virtual CSteamID GetSteamId(bool validated = false) const;

	virtual void Kick(const char* internalReason, ENetworkDisconnectionReason reason);

	uint64 GetSteamId64(bool validated = false) const {
		return GetSteamId(validated).ConvertToUint64();
	}

	void SetUnauthenticatedSteamID(uint64 xuid) {
		unauthenticatedSteamID = CSteamID(xuid);
	}

	CPlayerSlot GetPlayerSlot() const {
		return m_iSlot;
	}

	CEntityIndex GetEntityIndex() const {
		return m_iSlot + 1;
	}

private:
	const int m_iSlot;
	CSteamID unauthenticatedSteamID = k_steamIDNil;
};

class CPlayerManager : CCoreForward {
public:
	virtual CPlayer* ToPlayer(CServerSideClientBase* pClient) const;
	virtual CPlayer* ToPlayer(CPlayerPawnComponent* component) const;
	virtual CPlayer* ToPlayer(CBasePlayerController* controller) const;
	virtual CPlayer* ToPlayer(CBasePlayerPawn* pawn) const;
	virtual CPlayer* ToPlayer(CPlayerSlot slot) const;
	virtual CPlayer* ToPlayer(CEntityIndex entIndex) const;
	virtual CPlayer* ToPlayer(CPlayerUserId userID) const;
	virtual CPlayer* ToPlayer(CSteamID steamid, bool validate = false) const;

	virtual std::vector<CPlayer*> GetOnlinePlayers() const;

private:
	virtual void OnClientConnected(ISource2GameClients* pClient, CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID,
								   const char* pszAddress, bool bFakePlayer) override;

	virtual void OnClientDisconnect(ISource2GameClients* pClient, CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName,
									uint64 xuid, const char* pszNetworkID) override;

	virtual void OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) override;

private:
	std::array<std::unique_ptr<CPlayer>, MAXPLAYERS> m_pPlayers;
};

extern CPlayerManager* GetPlayerManager();
