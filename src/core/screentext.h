#pragma once

#include <pch.h>
#include <core/playermanager.h>

struct ScreenTextManifest_t {
	Vector2D m_vecPos = {0.0f, 0.0f};
	Color m_Color = {0, 222, 101, 255};
	std::string m_sFont = "Arial";
	float m_fFontSize = 20.0f;
};

class CScreenText {
public:
	CScreenText(const ScreenTextManifest_t& manifest);
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
	static Vector GetRelativeOrigin(const Vector& eyePosition, float distanceToTarget = 6.7f);

public:
	Vector2D m_vecPos;

private:
	CHandle<CPointWorldText> m_hScreenEnt;
	CHandle<CBasePlayerPawn> m_hOwner;
};

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

	[[nodiscard]] std::weak_ptr<CScreenText> CreateScreenText(CBasePlayerController* pController, std::optional<ScreenTextManifest_t> manifest = std::nullopt);
	void Render(CBasePlayerController* pController, const std::weak_ptr<CScreenText>& hText);
	void Unrender(CBasePlayerController* pController, const std::weak_ptr<CScreenText>& hText);
	void Cleanup(CBasePlayerController* pController);
} // namespace VGUI
