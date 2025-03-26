#pragma once

#include <pch.h>
#include <core/playermanager.h>

class CScreenText {
public:
	CScreenText(float x, float y, Color color, const std::string_view& font, float fontsize);
	~CScreenText();

	void SetText(const std::string_view& text);

	// plane coordinate
	//      ^
	//      |
	//  <- 0.0f ->
	//      |
	//      v
	void SetPos(float x, float y);

	void SetColor(Color color);
	void SetFont(const std::string_view& font);
	void SetFontSize(float fontsize);

	bool IsRendering();

	// FIXME: dead, spectator not supported yet.
	void Display(CBasePlayerController* pController);

	void UpdatePos();

public:
	static Vector GetRelativeOrigin(const Vector& eyePosition, float distanceToTarget = 7.0f);

public:
	Vector2D m_vecPos;

private:
	CHandle<CPointWorldText> m_hScreenEnt;
	CHandle<CBasePlayerPawn> m_hOwner;
};

using CScreenTextPtr = std::shared_ptr<CScreenText>;

class CScreenTextController : public CPlayer {
public:
	using CPlayer::CPlayer;

	virtual void Reset() override {
		CPlayer::Reset();

		m_vScreenTexts.clear();
	}

public:
	std::vector<std::shared_ptr<CScreenText>> m_vScreenTexts;
};

class CScreenTextControllerManager : CPlayerManager {
public:
	CScreenTextControllerManager() {
		for (int i = 0; i < MAXPLAYERS; i++) {
			m_pPlayers[i] = std::make_unique<CScreenTextController>(i);
		}
	}

	virtual CScreenTextController* ToPlayer(CBasePlayerController* controller) const override {
		return static_cast<CScreenTextController*>(CPlayerManager::ToPlayer(controller));
	}

	virtual CScreenTextController* ToPlayer(CBasePlayerPawn* pawn) const override {
		return static_cast<CScreenTextController*>(CPlayerManager::ToPlayer(pawn));
	}
};

namespace VGUI {
	CScreenTextControllerManager* GetScreenTextManager();

	[[nodiscard]] inline std::shared_ptr<CScreenText> CreateScreenText(float x = 0.0f, float y = 0.0f, Color color = Color(0, 222, 101, 255), std::string_view font = "Arial", float fontsize = 20.0f) {
		return std::make_shared<CScreenText>(x, y, color, font, fontsize);
	}

	void Render(CBasePlayerController* pController, const std::shared_ptr<CScreenText>& pText);
	void Unrender(CBasePlayerController* pController, const std::shared_ptr<CScreenText>& pText);
	void Cleanup(CBasePlayerController* pController);
} // namespace VGUI
