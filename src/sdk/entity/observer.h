#pragma once

#include <sdk/entity/ccsplayerpawn.h>
#include <sdk/entity/services.h>

enum {
	OBS_MODE_NONE = 0x0,    // not in spectator mode
	OBS_MODE_FIXED = 0x1,   // view from a fixed camera position
	OBS_MODE_IN_EYE = 0x2,  // follow a player in first person view
	OBS_MODE_CHASE = 0x3,   // follow a player in third person view
	OBS_MODE_ROAMING = 0x4, // free roaming
	OBS_MODE_DIRECTED = 0x5,
	NUM_OBSERVER_MODES = 0x6,
};

class CCSObserver_ObserverServices : public CPlayer_ObserverServices {};

class CCSObserver_MovementServices : public CPlayer_MovementServices {};

class CCSObserver_UseServices : public CPlayer_UseServices {};

class CCSObserver_ViewModelServices : public CPlayer_ViewModelServices {};

class CCSObserver_CameraServices : public CCSPlayerBase_CameraServices {};

class CCSObserverPawn : public CCSPlayerPawnBase {
public:
	CCSObserver_ObserverServices* GetObserverServices() {
		static auto iOffset = schema::GetOffset("CCSObserverPawn", "m_pObserverServices");
		return *(CCSObserver_ObserverServices**)((uint8_t*)this + iOffset.offset);
	};

	CCSObserver_MovementServices* GetMovementServices() {
		static auto iOffset = schema::GetOffset("CCSObserverPawn", "m_pMovementServices");
		return *(CCSObserver_MovementServices**)((uint8_t*)this + iOffset.offset);
	};

	CCSObserver_UseServices* GetUseServices() {
		static auto iOffset = schema::GetOffset("CCSObserverPawn", "m_pUseServices");
		return *(CCSObserver_UseServices**)((uint8_t*)this + iOffset.offset);
	};

	CCSObserver_ViewModelServices* GetViewModelServices() {
		static auto iOffset = schema::GetOffset("CCSObserverPawn", "m_pViewModelServices");
		return *(CCSObserver_ViewModelServices**)((uint8_t*)this + iOffset.offset);
	};

	CCSObserver_CameraServices* GetCameraServices() {
		static auto iOffset = schema::GetOffset("CCSObserverPawn", "m_pCameraServices");
		return *(CCSObserver_CameraServices**)((uint8_t*)this + iOffset.offset);
	};
};
