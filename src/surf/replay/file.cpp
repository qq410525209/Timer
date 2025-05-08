#include "surf_replay.h"
#include <thread>
#include <utils/utils.h>

constexpr auto REPLAY_ARRAY_HEAD = "===ARRAY_HEAD===";
constexpr auto REPLAY_ARRAY_TAIL = "===ARRAY_TAIL===";

static void ReadStreamString(std::ifstream& file, std::string& outStr) {
	uint32_t len = 0;
	file.read(reinterpret_cast<char*>(&len), sizeof(len));
	outStr.resize(len);
	file.read(&outStr[0], len);
}

static void WriteStreamString(std::ofstream& file, const std::string& str) {
	uint32_t length = static_cast<uint32_t>(str.size());
	file.write(reinterpret_cast<const char*>(&length), sizeof(length));
	file.write(str.data(), length);
}

void replay_file_header_t::ReadFromStream(std::ifstream& in) {
	ReadStreamString(in, format);
	in.read(reinterpret_cast<char*>(&version), sizeof(version));
	ReadStreamString(in, map);
	in.read(reinterpret_cast<char*>(&tickrate), sizeof(tickrate));
	in.read(reinterpret_cast<char*>(&info), sizeof(info));
}

void replay_file_header_t::WriteToStream(std::ofstream& out) const {
	WriteStreamString(out, format);
	out.write(reinterpret_cast<const char*>(&version), sizeof(version));
	WriteStreamString(out, map);
	out.write(reinterpret_cast<const char*>(&tickrate), sizeof(tickrate));
	out.write(reinterpret_cast<const char*>(&info), sizeof(info));
}

void CSurfReplayPlugin::AsyncWriteReplayFile(const replay_run_info_t& info, const ReplayArray_t& vFrames) {
	std::thread([info, vFrames]() {
		std::string path = "../../csgo/addons/cs2surf/FIXME";
		std::ofstream file(path, std::ios::out | std::ios::binary);
		if (!file.good()) {
			LOG::Error("Failed to WriteReplayFile: %s", path.data());
			SDK_ASSERT(false);
			return;
		}

		replay_file_header_t header;
		header.map = UTIL::GetGlobals()->mapname.ToCStr();
		header.tickrate = SURF_TICKRATE;
		header.info = info;
		header.WriteToStream(file);

		file.write(REPLAY_ARRAY_HEAD, std::strlen(REPLAY_ARRAY_HEAD));

		auto iFrameLen = vFrames.size();
		file.write(reinterpret_cast<const char*>(&iFrameLen), sizeof(iFrameLen));
		for (const auto& frame : vFrames) {
			file.write(reinterpret_cast<const char*>(&frame), sizeof(frame));
		}

		file.write(REPLAY_ARRAY_TAIL, std::strlen(REPLAY_ARRAY_TAIL));

		file.close();
	}).detach();
}

bool CSurfReplayPlugin::ReadReplayFile(const std::string_view path, ReplayArray_t& out) {
	std::ifstream file(path.data(), std::ios::in | std::ios::binary);
	if (!file.good()) {
		LOG::Error("Failed to read replay file: %s, Reason: file not found!", path.data());
		return false;
	}

	replay_file_header_t header;
	header.ReadFromStream(file);

	std::string sHead;
	ReadStreamString(file, sHead);

	if (sHead != std::string(REPLAY_ARRAY_HEAD)) {
		LOG::Error("Failed to read replay file: %s, Reason: array head not match!", path.data());
		file.close();
		return false;
	}

	size_t iFrameLen = 0;
	file.read(reinterpret_cast<char*>(&iFrameLen), sizeof(iFrameLen));
	out.reserve(iFrameLen);

	for (size_t i = 0; i < iFrameLen; i++) {
		replay_frame_t frame;
		file.read(reinterpret_cast<char*>(&frame), sizeof(frame));
		out.emplace_back(frame);
	}

	std::string sTail;
	ReadStreamString(file, sTail);

	file.close();

	return true;
}
