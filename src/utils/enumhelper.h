#pragma once

#include <cstdint>

// clang-format off

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator+=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs + rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator-=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs - rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator*=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs * rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator/=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs / rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator%=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs % rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator++(T& lhs) noexcept {
	lhs = static_cast<T>(lhs + 1);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator--(T& lhs) noexcept {
	lhs = static_cast<T>(lhs - 1);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator|=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs | rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator&=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs & rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator^=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs ^ rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator<<=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs << rhs);
	return lhs;
}

template<typename T, typename U>
requires std::is_enum_v<T>
constexpr T& operator>>=(T& lhs, U rhs) noexcept {
	lhs = static_cast<T>(lhs >> rhs);
	return lhs;
}

// clang-format on
