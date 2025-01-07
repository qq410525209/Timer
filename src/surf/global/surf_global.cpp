#include "surf_global.h"

CSurfGlobalAPIPlugin g_GlobalApi;

CSurfGlobalAPIPlugin* SURF::GlobalPlugin() {
	return &g_GlobalApi;
}

void CSurfGlobalAPIPlugin::OnPluginStart() {
	RegisterCommand();
}
