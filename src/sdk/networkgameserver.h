#pragma once

#include <pch.h>
#include <sdk/engine/framesnapshot.h>
#include <sdk/protobuf/netmessages.pb.h>

class CNetworkStringTableContainer;
class CNetworkServerSpawnGroupCreatePrerequisites;

template<typename T>
struct CBaseCmdKeyValues : T {
	KeyValues* m_pKeyValues;
};

class GameSessionConfigurationS2_t : public CBaseCmdKeyValues<CSVCMsg_GameSessionConfiguration> {};

struct SplitDisconnect_t {
	CServerSideClientBase* m_pUser;
	CServerSideClientBase* m_pSplit;
};

class CNetworkGameServer : public CNetworkGameServerBase {
public:
	// @boeing666: offsets only for linux
	char pad16[16];                                                        // 16
	IGameSpawnGroupMgr* m_pSpawnGroupMgr;                                  // 32
	char pad40[8];                                                         // 40
	int m_State;                                                           // 48
	int m_nRefCount;                                                       // 52
	int m_Socket;                                                          // 56
	int m_nTickCount;                                                      // 60
	void (*UnkFunc)();                                                     // 64
	void (*UnkFunc2)();                                                    // 72
	CGlobalVars m_globals;                                                 // 80
	GameSessionConfigurationS2_t m_GameConfig;                             // 200
	CUtlString m_szMapname;                                                // 320
	CUtlString m_szAddons;                                                 // 328
	CUtlString m_szSkyname;                                                // 336
	CUtlString m_szUnk344;                                                 // 344
	KeyValues* m_KV352;                                                    // 352
	CNetworkStringTableContainer* m_StringTables;                          // 360
	CNetworkStringTableContainer* m_StringTables2;                         // 368
	INetworkStringTable* m_pInstanceBaselineTable;                         // 376
	INetworkStringTable* m_pLightStyleTable;                               // 384
	INetworkStringTable* m_pUserInfoTable;                                 // 392
	INetworkStringTable* m_pServerStartupTable;                            // 400
	CFrameSnapshotManager m_snapshotManager;                               // 408
	char pad528[16];                                                       // 528
	char pad544[30];                                                       // 544
	bf_write m_Signon;                                                     // 576
	CUtlMemory<byte> m_SignonBuffer;                                       // 616
	bool m_bIsBackgroundMap;                                               // 632
	CUtlVector<CServerSideClientBase*> m_Clients;                          // 640
	char pad664[20];                                                       // 664
	CUtlMemory<byte> m_unk688;                                             // 688
	char pad704[16];                                                       // 704
	SpawnGroupHandle_t m_hActiveSpawnGroup;                                // 720
	int m_nMaxClients;                                                     // 724
	int m_nSpawnCount;                                                     // 728
	int serverclasses;                                                     // 732
	int serverclassbits;                                                   // 736
	uint64 m_nCreateTime;                                                  // 744
	bool m_bPendingChangeLevel;                                            // 752
	bool m_bUnk753;                                                        // 753
	bool m_bPreserveSteamID;                                               // 754
	bool m_bHibernating;                                                   // 755
	CUtlVector<netadr_t*> m_unk760;                                        // 760
	CUtlLinkedList<CNetworkServerSpawnGroupCreatePrerequisites*> m_unk784; // 784
	CUtlMap<uint32, void*> m_unk816;                                       // 816 something with spawngroups
	CUtlMap<uint32, void*> m_unk848;                                       // 848 something with spawngroups
	CUtlVector<char> m_GameData;                                           // 880
	CUtlString m_GameType;                                                 // 904
	char pad912[8];                                                        // 912
	CUtlVector<SplitDisconnect_t> m_QueuedForDisconnect;                   // 920
	CServerSideClientBase* m_pHostClient;                                  // 944
	IGameSpawnGroupMgr* m_pSpawnGroupMgr2;
	void* m_unk960;                               // 960
	HGameResourceManifest m_pGameSessionManifest; // 968
	char pad976[8];                               // 976
	uint64 m_nReservationCookie;                  // 984
	float m_flTimeLastClientLeft;                 // 992
	char pad996[12];                              // 996
	float m_flReservationExpiryTime;              // 1008
	char pad1012[140];                            // 1012
	int m_nFinalSimulationTickThisFrame;          // 1152
	int m_GameDataVersion;                        // 1156
	int m_numGameSlots;                           // 1160
	float m_fLastCPUCheckTime;                    // 1164
	float m_fCPUPercent;                          // 1168
	float m_flTimescale;                          // 1172
	bool m_bUnk1176;                              // 1176
	bool m_bUnk1177;                              // 1177
	uint64 m_nMatchId;                            // 1184
	void* m_pUnk1192;                             // 1192

	// public:
	//	CUtlString m_szStartspot;                      // 1200
	//	bf_write m_FullSendTables;                     // 1208
	//	CUtlMemory<byte> m_FullSendTablesBuffer;       // 1248
	//	CPrecacheItem generic_precache[MAX_GENERIC];   // 1264
	//	char pad9456[8];                               // 9456
	//	CPrecacheItem decal_precache[MAX_BASE_DECALS]; // 9464
	//	char pad17656[8];                              // 17656
	//	INetworkStringTable* m_pGenericPrecacheTable;  // 17664
	//	INetworkStringTable* m_pDecalPrecacheTable;    // 17672
	//	CPureServerWhitelist* m_pPureServerWhitelist;  // 17680
	//	bool allowsignonwrites;                        // 17688
}; // sizeof 17696
