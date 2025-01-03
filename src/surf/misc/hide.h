#pragma once

#include <core/forwards.h>

using CEdictBitVec = CBitVec<MAX_EDICTS>;

class CHidePlugin : CCoreForward {
public:
	void Set(CBasePlayerController* pOwner, CBaseEntity* pTarget, bool bShouldHide);
	void Remove(CBasePlayerController* pOwner, CBaseEntity* pTarget);
	void Reset(CBasePlayerController* pOwner);

private:
	void ClearEntityTransmitInfo(CEdictBitVec* pTransmitEntity, CEdictBitVec* pTransmitAlways, CBaseEntity* pEntity);
	void ClearEntityChildEntities(CEdictBitVec* pTransmitEntity, CEdictBitVec* pTransmitAlways, CBaseEntity* pEntity);

private:
	virtual void OnClientSendSnapshotBefore(CServerSideClient* pClient) override;

private:
	std::array<CEdictBitVec, MAXPLAYERS> m_vBlockTransmit;
};

extern CHidePlugin* HidePlugin();
