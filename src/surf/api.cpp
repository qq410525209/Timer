#include "api.h"
#include <fmt/format.h>

FORWARD_INIT(CSurfForward);
FORWARD_INIT(CSurfGlobalAPIForward);

// TODO: Style
std::string SURF::GetStyleName(i8 style) {
	return "正常模式";
}

std::string SURF::GetStyleShortName(i8 style) {
	return "NM";
}

std::string SURF::GetTrackName(TimerTrack track) {
	switch (track) {
		case EZoneTrack::Track_Main:
			return "主线";
		default:
			return fmt::format("奖励 {}", (u8)track);
	}
}
