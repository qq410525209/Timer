#pragma once

#include <surf/surf_player.h>

class CSurfForward : public CBaseForward<CSurfForward> {
public:
};

class CSurfBaseService : public CBaseForward<CSurfBaseService> {
public:
	virtual void Init(CSurfPlayer* player) {
		this->m_pPlayer = player;
	}

private:
	CSurfPlayer* m_pPlayer;
};
