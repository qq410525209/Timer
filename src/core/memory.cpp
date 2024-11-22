#include "memory.h"
#include <sdk/common.h>

static bool SetupDetours() {
	return true;
}

static bool SetupVMTHooks() {
	return true;
}

void MEM::SetupHooks() {
	SURF_ASSERT(SetupDetours());
	SURF_ASSERT(SetupVMTHooks());
}
