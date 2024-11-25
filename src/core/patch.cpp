#include "patch.h"
#include <core/memory.h>

void PATCH::DoMovementUnlocker() {
	static auto fn = GAMEDATA::GetMemSig("ServerMovementUnlock");
	SURF_ASSERT(fn);

	WIN_LINUX(libmem::StoreToAddress(fn, 0xEB), libmem::StoreToAddress(fn, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90));
}
