#pragma once

#include <pch.h>

#include <sdk/forwardbase.h>
#include <sdk/usercmd.h>
#include <sdk/entity/ccsplayerpawn.h>

class CCoreForward : public CForwardBase<CCoreForward> {
public:
	virtual bool OnPlayerRunCmd(CCSPlayerPawn* pawn, CInButtonState* buttons, float (&vec)[3], float (&angles)[3], int& weapon, int& cmdnum,
								int& tickcount, int& seed, int (&mouse)[2]) {
		return true;
	}

	virtual void OnPlayerRunCmdPost(CCSPlayerPawn* pawn, const CInButtonState* buttons, const float (&vec)[3], const float (&angles)[3],
									const int& weapon, const int& cmdnum, const int& tickcount, const int& seed, const int (&mouse)[2]) {}

	virtual void OnGameFrame(ISource2Server* pServer, bool simulating, bool bFirstTick, bool bLastTick) {}

	virtual void OnClientConnected(ISource2GameClients* pClient, CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID,
								   const char* pszAddress, bool bFakePlayer) {}

	virtual void OnClientFullyConnect(ISource2GameClients* pClient, CPlayerSlot slot) {}

	virtual void OnClientPutInServer(ISource2GameClients* pClient, CPlayerSlot slot, char const* pszName, int type, uint64 xuid) {}

	virtual void OnClientActive(ISource2GameClients* pClient, CPlayerSlot slot, bool bLoadGame, const char* pszName, uint64 xuid) {}

	virtual void OnClientDisconnect(ISource2GameClients* pClient, CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName,
									uint64 xuid, const char* pszNetworkID) {}

	virtual void OnClientVoice(ISource2GameClients* pClient, CPlayerSlot slot) {}

	virtual void OnClientCommand(ISource2GameClients* pClient, CPlayerSlot slot, const CCommand& args) {}

	virtual void OnStartupServer(INetworkServerService* pServerService, const GameSessionConfiguration_t& config) {}

	virtual void OnFireEvent(IGameEventManager2* pEventManager, IGameEvent* pEvent, bool bDontBroadcast) {}

	virtual void OnDispatchConCommand(ICvar* pCvar, ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args) {}

	virtual void OnPostEventAbstract(IGameEventSystem* pEventSystem, CSplitScreenSlot nSlot, int nClientCount, const uint64* clients,
									 INetworkMessageInternal* pEvent, const CNetMessage* pData) {}
};
