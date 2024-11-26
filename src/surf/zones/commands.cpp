#include <core/concmdmanager.h>

CCMD_CALLBACK(Command_Zones) {}

void RegisterCommand() {
	CONCMD::RegConsoleCmd("sm_zones", Command_Zones);
}
