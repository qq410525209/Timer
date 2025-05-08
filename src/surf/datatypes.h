#pragma once

namespace SURF {
	inline const Vector ZERO_VEC = {0.0f, 0.0f, 0.0f};
	inline const QAngle ZERO_ANG = {0.0f, 0.0f, 0.0f};
} // namespace SURF

enum ZoneTrack : i8 {
	Track_Main,
	Track_Bonus,
	Track_MaxBonus = 16,
	TRACKS_SIZE
};

enum ZoneType : i8 {
	Zone_Start,
	Zone_End,
	Zone_Stage,
	Zone_Checkpoint,
	Zone_Stop,     // stops the player's timer
	Zone_Teleport, // teleports to a defined point
	Zone_Mark,     // do nothing, mainly used for marking trigger;
	ZONETYPES_SIZE
};

struct timer_snapshot_t {
	timer_snapshot_t() = default;
	timer_snapshot_t(const timer_snapshot_t&) = default;

	timer_snapshot_t& operator=(const timer_snapshot_t& other) = default;

	f64 m_fCurrentTime {};
	bool m_bTimerRunning {};
	bool m_bStageTimer {};
	bool m_bPracticeMode {};
	bool m_bPaused {};

	ZoneTrack m_iCurrentTrack = ZoneTrack::Track_Main;
	i8 m_iCurrentStyle {};

	i32 m_iCurrentStage {};
	i32 m_iLastStage {};
};

struct timer_pausemovement_t {
	Vector m_vecPos = SURF::ZERO_VEC;
	QAngle m_vecAng = SURF::ZERO_ANG;
	Vector m_vecVel = SURF::ZERO_VEC;

	Vector m_vecLadderNormal = SURF::ZERO_VEC;

	MoveType_t m_nMoveType {};
	MoveType_t m_nActualMoveType {};

	bool m_bDucked {};
	bool m_bDucking {};
	f32 m_fDuckTime {};
	f32 m_fDuckSpeed {};

	f32 m_fGravity {};
	f32 m_fSpeed {};
	f32 m_fStamina {};

	i32 m_iFlags {};
	CEntityHandle m_hGroundEntity;

	std::string m_sTargetName;
};

struct hudsettings_t {
	bool m_bMaster = true;

	struct {
		bool m_bEnabled = true;
		bool m_bTimer = true;
		bool m_bSpeed = true;
		bool m_bRank = true;
		bool m_bMapinfo = true;
		bool m_bZone = true;
	} m_Center;

	bool m_bSpectators = true;
	bool m_bKeyOverlay = true;
	bool m_bSpeedText = true;
	bool m_bHidePlayers = false;
};

struct replay_frame_t {
	QAngle ang;
	Vector pos;
	CPlayerButton buttons;
	uint32 flags;
	MoveType_t mt;
};
