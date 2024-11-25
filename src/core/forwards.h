#pragma once

#include <pch.h>

#include <sdk/forwardbase.h>
#include <sdk/usercmd.h>
#include <sdk/entity/ccsplayerpawn.h>
#include <sdk/entity/services.h>

#define FORWARD_POST(forwardClass, forwardFn, ...) \
	for (auto p = forwardClass::m_pFirst; p; p = p->m_pNext) { \
		p->forwardFn(__VA_ARGS__); \
	}

class CCoreForward : public CForwardBase<CCoreForward> {
public:
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
