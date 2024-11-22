#pragma once

#include <sdk/common.h>

#include <string>
#include <filesystem>

namespace UTIL {
	namespace PATH {
		template<typename... Args>
		std::string Join(const Args&... args) {
			std::filesystem::path result;
			((result /= std::filesystem::path(args)), ...);
			return result.string();
		}
	} // namespace PATH

	inline std::string GetWorkingDirectory() {
		return PATH::Join(std::filesystem::current_path().string(), "..", "..", GAME_NAME, "addons", "cs2surf");
	}
} // namespace UTIL
