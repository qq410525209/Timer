#include "surf_bot.h"
#include <surf/replay/surf_replay.h>
#include <utils/utils.h>

CSurfBotManager g_SurfBotManager;

CSurfBotManager* SURF::GetBotManager() {
	return &g_SurfBotManager;
}

CSurfBot* CSurfBotManager::ToPlayer(CServerSideClientBase* pClient) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(pClient));
}

CSurfBot* CSurfBotManager::ToPlayer(CPlayerPawnComponent* component) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(component));
}

CSurfBot* CSurfBotManager::ToPlayer(CBasePlayerController* controller) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(controller));
}

CSurfBot* CSurfBotManager::ToPlayer(CBasePlayerPawn* pawn) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(pawn));
}

CSurfBot* CSurfBotManager::ToPlayer(CPlayerSlot slot) const {
	auto pPlayer = static_cast<CSurfBot*>(CPlayerManager::ToPlayer(slot));
	return pPlayer->m_bFakeClient ? pPlayer : nullptr;
}

CSurfBot* CSurfBotManager::ToPlayer(CEntityIndex entIndex) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(entIndex));
}

CSurfBot* CSurfBotManager::ToPlayer(CPlayerUserId userID) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(userID));
}

CSurfBot* CSurfBotManager::ToPlayer(CSteamID steamid, bool validate) const {
	return static_cast<CSurfBot*>(CPlayerManager::ToPlayer(steamid, validate));
}

std::vector<CPlayer*> CSurfBotManager::GetOnlinePlayers() const {
	std::vector<CPlayer*> bots;
	for (auto& player : m_pPlayers) {
		if (player->m_bFakeClient && UTIL::IsPlayerSlot(player->GetPlayerSlot())) {
			bots.emplace_back(player.get());
		}
	}

	return bots;
}

void CSurfBot::Init(int iSlot) {
	CPlayer::Init(iSlot);

	InitService(m_pReplayService);
}

void CSurfBot::Reset() {
	CPlayer::Reset();

	ResetService(m_pReplayService);
}
