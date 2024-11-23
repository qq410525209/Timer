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

	std::string GetWorkingDirectory();
	std::string GetPublicIP();
	json LoadJsonc(std::string sFilePath);
} // namespace UTIL
