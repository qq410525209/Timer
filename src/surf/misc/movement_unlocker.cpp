#include <surf/surf_player.h>
#include <core/patch.h>

class CMovementUnlocker : CCoreForward {
private:
	virtual void OnPluginStart() override;
};

CMovementUnlocker g_MovementUnlocker;

void CMovementUnlocker::OnPluginStart() {
	PATCH::DoMovementUnlocker();
}
