#pragma once

#include <inetchannel.h>
#include <playerslot.h>
// #include <protocol.h> // @Wend4r: use <netmessages.pb.h> instead.
#include <entity2/entityidentity.h>
#include <steam/steamclientpublic.h>
#include <sdk/engine/clientframe.h>
#include <sdk/tier0/annotations.h>
#include <sdk/tier0/circularbuffer.h>
#include <networksystem/inetworksystem.h>

#include <sdk/netmessages.h>
#include <sdk/protobuf/network_connection.pb.h>

class CHLTVServer;
class INetMessage;
class CNetworkGameServerBase;
class CNetworkGameServer;

struct HltvReplayStats_t {
	enum FailEnum_t {
		FAILURE_ALREADY_IN_REPLAY,
		FAILURE_TOO_FREQUENT,
		FAILURE_NO_FRAME,
		FAILURE_NO_FRAME2,
		FAILURE_CANNOT_MATCH_DELAY,
		FAILURE_FRAME_NOT_READY,
		NUM_FAILURES
	};

	uint nClients;
	uint nStartRequests;
	uint nSuccessfulStarts;
	uint nStopRequests;
	uint nAbortStopRequests;
	uint nUserCancels;
	uint nFullReplays;
	uint nNetAbortReplays;
	uint nFailedReplays[NUM_FAILURES];
}; // sizeof 56

struct Spike_t {
public:
	CUtlString m_szDesc;
	int m_nBits;
}; // sizeof 16

class CNetworkStatTrace {
public:
	CUtlVector<Spike_t> m_Records;
	int m_nMinWarningBytes;
	int m_nStartBit;
	int m_nCurBit;
}; // sizeof 40

// class CServerSideClientBase: CUtlSlot, INetworkChannelNotify, INetworkMessageProcessingPreFilter;
class CServerSideClientBase {
	virtual void UnkDestructor() = 0;

public:
	virtual ~CServerSideClientBase() = 0;

public:
	CPlayerSlot GetPlayerSlot() const {
		return m_nClientSlot;
	}

	CPlayerUserId GetUserID() const {
		return m_UserID;
	}

	CEntityIndex GetEntityIndex() const {
		return m_nEntityIndex;
	}

	CSteamID GetClientSteamID() const {
		return m_SteamID;
	}

	const char* GetClientName() const {
		return m_Name;
	}

	INetChannel* GetNetChannel() const {
		return m_NetChannel;
	}

	const ns_address GetRemoteAddress() const {
		return m_nAddr;
	}

	CNetworkGameServerBase* GetServer() const {
		return m_Server;
	}

	virtual void Connect(int socket, const char* pszName, int nUserID, INetChannel* pNetChannel, bool bFakePlayer, bool bSplitClient, int iClientPlatform) = 0;
	virtual void Inactivate() = 0;
	virtual void Reactivate(CPlayerSlot nSlot) = 0;
	virtual void SetServer(CNetworkGameServer* pNetServer) = 0;
	virtual void Reconnect() = 0;
	virtual void Disconnect(ENetworkDisconnectionReason reason) = 0;
	virtual bool CheckConnect() = 0;

private:
	virtual void unk_10() = 0;

public:
	virtual void SetRate(int nRate) = 0;
	virtual void SetUpdateRate(float fUpdateRate) = 0;
	virtual int GetRate() = 0;

	virtual void Clear() = 0;

	virtual void ExecuteStringCommand(const CNETMsg_StringCmd& msg) = 0;
	virtual void SendNetMessage(const CNetMessage* pData, NetChannelBufType_t bufType) = 0;

#ifdef LINUX

private:
	virtual void unk_17() = 0;
#endif

public:
	virtual void ClientPrintf(PRINTF_FORMAT_STRING const char*, ...) = 0;

	bool IsConnected() const {
		return m_nSignonState >= SIGNONSTATE_CONNECTED;
	}

	bool IsInGame() const {
		return m_nSignonState == SIGNONSTATE_FULL;
	}

	bool IsSpawned() const {
		return m_nSignonState >= SIGNONSTATE_NEW;
	}

	bool IsActive() const {
		return m_nSignonState == SIGNONSTATE_FULL;
	}

	virtual bool IsFakeClient() = 0;

	virtual bool IsHLTV() = 0;

	// Is an actual human player or splitscreen player (not a bot and not a HLTV slot)
	virtual bool IsHumanPlayer() = 0;

	virtual bool IsHearingClient(CPlayerSlot nSlot) = 0;

public: // Message Handlers
	virtual bool ProcessTick(const CNETMsg_Tick_t& msg) = 0;
	virtual bool ProcessStringCmd(const CNETMsg_StringCmd_t& msg) = 0;

private:
	virtual bool unk_27() = 0;
	virtual bool unk_28() = 0;

public:
	virtual bool ProcessSpawnGroup_LoadCompleted(const CNETMsg_SpawnGroup_LoadCompleted_t& msg) = 0;
	virtual bool ProcessClientInfo(const CCLCMsg_ClientInfo_t& msg) = 0;
	virtual bool ProcessBaselineAck(const CCLCMsg_BaselineAck_t& msg) = 0;
	virtual bool ProcessLoadingProgress(const CCLCMsg_LoadingProgress_t& msg) = 0;
	virtual bool ProcessSplitPlayerConnect(const CCLCMsg_SplitPlayerConnect_t& msg) = 0;
	virtual bool ProcessSplitPlayerDisconnect(const CCLCMsg_SplitPlayerDisconnect_t& msg) = 0;
	virtual bool ProcessCmdKeyValues(const CCLCMsg_CmdKeyValues_t& msg) = 0;

private:
	virtual bool unk_36() = 0;
	virtual bool unk_37() = 0;

public:
	virtual bool ProcessMove(const CCLCMsg_Move_t& msg) = 0;
	virtual bool ProcessVoiceData(const CCLCMsg_VoiceData_t& msg) = 0;
	virtual bool ProcessFileCRCCheck(const CCLCMsg_FileCRCCheck_t& msg) = 0;
	virtual bool ProcessRespondCvarValue(const CCLCMsg_RespondCvarValue_t& msg) = 0;

	virtual bool ProcessPacketStart(const NetMessagePacketStart_t& msg) = 0;
	virtual bool ProcessPacketEnd(const NetMessagePacketEnd_t& msg) = 0;
	virtual bool ProcessConnectionClosed(const NetMessageConnectionClosed_t& msg) = 0;
	virtual bool ProcessConnectionCrashed(const NetMessageConnectionCrashed_t& msg) = 0;

public:
	virtual bool ProcessChangeSplitscreenUser(const NetMessageSplitscreenUserChanged_t& msg) = 0;

private:
	virtual bool unk_47() = 0;
	virtual bool unk_48() = 0;
	virtual bool unk_49() = 0;

public:
	virtual void ConnectionStart(INetChannel* pNetChannel) = 0;

private: // SpawnGroup something.
	virtual void unk_51() = 0;
	virtual void unk_52() = 0;

public:
	virtual void ExecuteDelayedCall(void*) = 0;

	virtual bool UpdateAcknowledgedFramecount(int tick) = 0;

	void ForceFullUpdate() {
		m_nForceWaitForTick = -1;
	}

	virtual bool ShouldSendMessages() = 0;
	virtual void UpdateSendState() = 0;

	virtual CMsgPlayerInfo& GetPlayerInfo() = 0;

	virtual void UpdateUserSettings() = 0;
	virtual void ResetUserSettings() = 0;

private:
	virtual void unk_60() = 0;

public:
	virtual void SendSignonData() = 0;
	virtual void SpawnPlayer() = 0;
	virtual void ActivatePlayer() = 0;

	virtual void SetName(const char* name) = 0;
	virtual void SetUserCVar(const char* cvar, const char* value) = 0;

	int GetSignonState() const {
		return m_nSignonState;
	}

	virtual void FreeBaselines() = 0;

	bool IsFullyAuthenticated(void) {
		return m_bFullyAuthenticated;
	}

	void SetFullyAuthenticated(void) {
		m_bFullyAuthenticated = true;
	}

	virtual CServerSideClientBase* GetSplitScreenOwner() = 0;

	virtual int GetNumPlayers() = 0;

	virtual void ShouldReceiveStringTableUserData() = 0;

private:
	virtual void unk_70(CPlayerSlot nSlot) = 0;
	virtual void unk_71() = 0;
	virtual void unk_72() = 0;

public:
	virtual int GetHltvLastSendTick() = 0;

private:
	virtual void unk_74() = 0;
	virtual void unk_75() = 0;
	virtual void unk_76() = 0;

public:
	virtual void Await() = 0;

	virtual void MarkToKick() = 0;
	virtual void UnmarkToKick() = 0;

	virtual bool ProcessSignonStateMsg(int state) = 0;
	virtual void PerformDisconnection(ENetworkDisconnectionReason reason) = 0;

public:
	void* m_pVT1; // INetworkMessageProcessingPreFilter

private:
	MEM_PAD(0x20);
#ifdef __linux__
	MEM_PAD(0x10);
#endif

public:
	void (*RebroadcastSource)(int msgID); // 48
	CUtlString m_UserIDString;            // 56
	CUtlString m_Name;                    // 64
	CPlayerSlot m_nClientSlot;            // 72
	CEntityIndex m_nEntityIndex;          // 76
	CNetworkGameServerBase* m_Server;     // 80
	INetChannel* m_NetChannel;            // 88
	uint8 m_nUnkVariable;                 // 96
	bool m_bMarkedToKick;                 // 97
	int32 m_nSignonState;                 // 100
	MEM_PAD(0x38);
	bool m_bFakePlayer; // 160
	MEM_PAD(0x7);
	CPlayerUserId m_UserID; // 168
	CSteamID m_SteamID;     // 170
	CSteamID m_UnkSteamID;  // 195
	CSteamID m_UnkSteamID2; // 203 from auth ticket
	CSteamID m_nFriendsID;  // 211
	ns_address m_nAddr;     // 220
	ns_address m_nAddr2;    // 252
	KeyValues* m_ConVars;   // 288
	bool m_bConVarsChanged; // 296
	bool m_bConVarsInited;  // 297
	bool m_bIsHLTV;         // 298
	bool m_bIsReplay;       // 299

private:
	MEM_PAD(0x15);

public:
	int m_nForceWaitForTick; // 308
	MEM_PAD(0x882);
	bool m_bFullyAuthenticated = false; // 2490
	MEM_PAD(0x1C2);
};

// constexpr auto dsa = offsetof(CServerSideClientBase, m_nSignonState);

#ifdef _WIN32
COMPILE_TIME_ASSERT(sizeof(CServerSideClientBase) == 2944);
#endif

class CServerSideClient : public CServerSideClientBase {
public:
	virtual ~CServerSideClient() = 0;

public:
	CPlayerBitVec m_VoiceStreams;   // 2944
	CPlayerBitVec m_VoiceProximity; // 2952
	CCheckTransmitInfo m_PackInfo;  // 2960
	MEM_PAD(0x240);
	CClientFrameManager m_FrameManager; // 3552
	MEM_PAD(0x80);
};

#ifdef _WIN32
COMPILE_TIME_ASSERT(offsetof(CServerSideClient, m_PackInfo) == 2960);
COMPILE_TIME_ASSERT(offsetof(CServerSideClient, m_FrameManager) == 3552);
COMPILE_TIME_ASSERT(sizeof(CServerSideClient) == 3968);
#endif

// not full class reversed
class CHLTVClient : public CServerSideClientBase {
public:
	virtual ~CHLTVClient() = 0;

public:
	CNetworkGameServerBase* m_pHLTV; // 2952
	CUtlString m_szPassword;         // 2960
	CUtlString m_szChatGroup;        // 2968 // "all" or "group%d"
	double m_fLastSendTime = 0.0;    // 2976
	double m_flLastChatTime = 0.0;   // 2984
	int m_nLastSendTick = 0;         // 2992
	int m_unknown2 = 0;              // 2996
	int m_nFullFrameTime = 0;        // 3000
	int m_unknown3 = 0;              // 3004

private:
	[[maybe_unused]] char pad2960[0x4]; // 3008

public:
	bool m_bNoChat = false;   // 3012
	bool m_bUnkBool = false;  // 3013
	bool m_bUnkBool2 = false; // 3014
	bool m_bUnkBool3 = false; // 3015

	[[maybe_unused]] char pad3976[0x28]; // 2984
}; // sizeof 3056
