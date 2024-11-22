#include "cs2surf.h"
#include "version.h"

#include <core/interfaces.h>
#include <core/memory.h>

CSurfPlugin g_SurfPlugin;

PLUGIN_EXPOSE(CSurfPlugin, g_SurfPlugin);

CSurfPlugin* SurfPlugin() {
	return &g_SurfPlugin;
}

bool CSurfPlugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) {
	IFACE::Setup(ismm, error, maxlen);
	MEM::SetupHooks();

	return true;
}

const char* CSurfPlugin::GetAuthor() {
	return "Nyano1337";
}

const char* CSurfPlugin::GetName() {
	return "CS2-SurfTimer";
}

const char* CSurfPlugin::GetDescription() {
	return "";
}

const char* CSurfPlugin::GetURL() {
	return " https://cs2surf.com";
}

const char* CSurfPlugin::GetLicense() {
	return "MIT License";
}

const char* CSurfPlugin::GetVersion() {
	return "dev"; // VERSION_STRING
}

const char* CSurfPlugin::GetDate() {
	return "";
}

const char* CSurfPlugin::GetLogTag() {
	return "";
}
