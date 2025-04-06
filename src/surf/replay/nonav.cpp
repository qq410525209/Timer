#include <pch.h>
#include <core/memory.h>
#include <libmem/libmem_helper.h>

class CNoNavPlugin : CCoreForward {
	virtual void OnPluginStart() override;
};

CNoNavPlugin g_NoNav;

void CNoNavPlugin::OnPluginStart() {
	static auto fn = libmem::SignScan("80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 80 3D ? ? ? ? ? 74", LIB::server);
	SDK_ASSERT(fn);

	MEM::PatchNOP(fn, 13);
}
