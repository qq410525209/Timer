#pragma once

#include <tuple>
#include <type_traits>

template<typename T>
struct FunctionTraits;

template<typename Ret, typename... Args>
struct FunctionTraits<Ret (*)(Args...)> {
	using ArgsTuple = std::tuple<Args...>;
	using ReturnType = Ret;
	static constexpr size_t ArgCount = sizeof...(Args);
};

template<typename Ret, typename Class, typename... Args>
struct FunctionTraits<Ret (Class::*)(Args...)> {
	using type = Class;
	using ReturnType = Ret;
};

template<typename Ret, typename Class, typename... Args>
struct FunctionTraits<Ret (Class::*)(Args...) const> {
	using type = Class;
	using ReturnType = Ret;
};

template<typename FuncPtr, typename NewReturnType>
struct RebindFunction;

template<typename Ret, typename... Args, typename NewReturnType>
struct RebindFunction<Ret (*)(Args...), NewReturnType> {
	using Type = NewReturnType(Args...);
};

template<typename FuncPtr, typename NewReturnType>
using RebindFunction_t = typename RebindFunction<FuncPtr, NewReturnType>::Type;
