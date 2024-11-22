#include "cs2surf.h"

CSurfPlugin g_SurfPlugin;

CSurfPlugin* SurfPlugin() {
	return &g_SurfPlugin;
}

bool CSurfPlugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) {
	return true;
}

const char* CSurfPlugin::GetAuthor() {
	return nullptr;
}

const char* CSurfPlugin::GetName() {
	return nullptr;
}

const char* CSurfPlugin::GetDescription() {
	return nullptr;
}

const char* CSurfPlugin::GetURL() {
	return nullptr;
}

const char* CSurfPlugin::GetLicense() {
	return nullptr;
}

const char* CSurfPlugin::GetVersion() {
	return nullptr;
}

const char* CSurfPlugin::GetDate() {
	return nullptr;
}

const char* CSurfPlugin::GetLogTag() {
	return nullptr;
}
