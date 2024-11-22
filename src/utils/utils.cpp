#include "utils.h"
#include <fstream>

std::string UTIL::GetWorkingDirectory() {
	return PATH::Join(std::filesystem::current_path().string(), "..", "..", GAME_NAME, "addons", "cs2surf");
}

json UTIL::LoadJsonc(std::string sFilePath) {
	std::ifstream file(sFilePath);
	if (!file.good()) {
		return json();
	}

	return json::parse(file, nullptr, true, true);
};
