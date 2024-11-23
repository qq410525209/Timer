#pragma once

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#pragma region GAME_DEF

#define CS_TEAM_NONE      0
#define CS_TEAM_SPECTATOR 1
#define CS_TEAM_T         2
#define CS_TEAM_CT        3

#pragma endregion

#pragma region SURF_DEF

#ifndef SURF_DEBUG_BREAK
#ifdef _WIN32
#define SURF_DEBUG_BREAK() __debugbreak()
#else
#define SURF_DEBUG_BREAK() __builtin_trap()
#endif
#endif

#ifndef SURF_ASSERT
#if defined(_DEBUG) || defined(_OD)
#define SURF_ASSERT(EXPRESSION) static_cast<void>(!!(EXPRESSION) || (SURF_DEBUG_BREAK(), 0))
#else
// disable assertion for release builds
#define SURF_ASSERT(EXPRESSION) static_cast<void>(0)
#endif
#endif

#pragma endregion

#pragma region COMMON_DEF

#ifndef GAME_NAME
#define GAME_NAME "csgo"
#endif

#define ALIGN_VALUE(val, alignment) ((val + alignment - 1) & ~(alignment - 1))
#define MAXPLAYERS                  64

#ifdef _WIN32
#define ROOTBIN       "/bin/win64/"
#define GAMEBIN       "/" GAME_NAME "/bin/win64/"
#define MODULE_PREFIX ""
#define MODULE_EXT    ".dll"
#else
#define ROOTBIN       "/bin/linuxsteamrt64/"
#define GAMEBIN       "/" GAME_NAME "/bin/linuxsteamrt64/"
#define MODULE_PREFIX "lib"
#define MODULE_EXT    ".so"
#endif

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

#define FILLMODULE(name)              MODULE_PREFIX name MODULE_EXT
#define FILLMODULE_FORCE_PREFIX(name) "lib" name MODULE_EXT
#define FILLMODULE_NO_PREFIX(name)    name MODULE_EXT

#if defined _WIN32
#define FORCEINLINE       __forceinline
#define DLL_EXPORT        extern "C" __declspec(dllexport)
#define DLL_IMPORT        extern "C" __declspec(dllimport)
#define DLL_GLOBAL_EXPORT extern __declspec(dllexport)
#define DLL_GLOBAL_IMPORT extern __declspec(dllimport)
#define DLL_CLASS_EXPORT  __declspec(dllexport)
#define DLL_CLASS_IMPORT  __declspec(dllimport)
#define stackalloc(_size) _alloca(ALIGN_VALUE(_size, 16))
#define mallocsize(_p)    (_msize(_p))
#define FMTFUNCTION(a, b)
#define RETURN_ADDRESS _ReturnAddress()
#define FRAME_ADDRESS  _AddressOfReturnAddress()
#elif defined(__GNUC__)
#define FORCEINLINE       inline __attribute__((always_inline))
#define DLL_EXPORT        extern "C" __attribute__((visibility("default")))
#define DLL_IMPORT        extern "C"
#define DLL_GLOBAL_EXPORT extern __attribute__((visibility("default")))
#define DLL_GLOBAL_IMPORT extern
#define DLL_CLASS_EXPORT  __attribute__((visibility("default")))
#define DLL_CLASS_IMPORT
#define stackalloc(_size)                            alloca(ALIGN_VALUE(_size, 16))
#define FMTFUNCTION(fmtargnumber, firstvarargnumber) __attribute__((format(printf, fmtargnumber, firstvarargnumber)))
#define RETURN_ADDRESS                               __builtin_return_address(0)
#define FRAME_ADDRESS                                __builtin_frame_address(0) // @note: it isn't always what we're expecting, compiler dependent
#endif

#if defined(__GNUC__)
#define DECL_ALIGN(x) __attribute__((aligned(x)))

// gnuc has the align decoration at the end
#define ALIGN4
#define ALIGN8
#define ALIGN16
#define ALIGN32
#define ALIGN128

#define ALIGN4_POST   DECL_ALIGN(4)
#define ALIGN8_POST   DECL_ALIGN(8)
#define ALIGN16_POST  DECL_ALIGN(16)
#define ALIGN32_POST  DECL_ALIGN(32)
#define ALIGN128_POST DECL_ALIGN(128)
#else
#define DECL_ALIGN(x) __declspec(align(x))

// MSVC has the align at the start of the struct
#define ALIGN4        DECL_ALIGN(4)
#define ALIGN8        DECL_ALIGN(8)
#define ALIGN16       DECL_ALIGN(16)
#define ALIGN32       DECL_ALIGN(32)
#define ALIGN128      DECL_ALIGN(128)

#define ALIGN4_POST
#define ALIGN8_POST
#define ALIGN16_POST
#define ALIGN32_POST
#define ALIGN128_POST
#endif

#define PLATFORM_OVERLOAD DLL_GLOBAL_IMPORT

// Makes a 4-byte "packed ID" int out of 4 characters
#define MAKEID(d, c, b, a) (((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | ((int)(d)))

// Compares a string with a 4-byte packed ID constant
#define STRING_MATCHES_ID(p, id) ((*((int*)(p)) == (id)) ? true : false)
#define ID_TO_STRING(id, p) \
	((p)[3] = (((id) >> 24) & 0xFF), (p)[2] = (((id) >> 16) & 0xFF), (p)[1] = (((id) >> 8) & 0xFF), (p)[0] = (((id) >> 0) & 0xFF))

#define Q_ARRAYSIZE(p) (sizeof(p) / sizeof(p[0]))

#define SETBITS(iBitVector, bits)   ((iBitVector) |= (bits))
#define CLEARBITS(iBitVector, bits) ((iBitVector) &= ~(bits))
#define FBitSet(iBitVector, bits)   ((iBitVector) & (bits))

#ifndef REFERENCE
#define REFERENCE(arg) ((void)arg)
#endif

#define CONST_INTEGER_AS_STRING(x) #x // Wraps the integer in quotes, allowing us to form constant strings with it
#define __HACK_LINE_AS_STRING__(x) \
	CONST_INTEGER_AS_STRING( \
		x) //__LINE__ can only be converted to an actual number by going through this, otherwise the output is literally "__LINE__"
#define __LINE__AS_STRING __HACK_LINE_AS_STRING__(__LINE__) // Gives you the line number in constant string form

// Using ARRAYSIZE implementation from winnt.h:
#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif

// Return the number of elements in a statically sized array.
//   DWORD Buffer[100];
//   RTL_NUMBER_OF(Buffer) == 100
// This is also popularly known as: NUMBER_OF, ARRSIZE, _countof, NELEM, etc.
//
#define RTL_NUMBER_OF_V1(A) (sizeof(A) / sizeof((A)[0]))

#if defined(__cplusplus) && !defined(MIDL_PASS) && !defined(RC_INVOKED) && !defined(_PREFAST_) && (_MSC_FULL_VER >= 13009466) && !defined(SORTPP_PASS)

// From crtdefs.h
#if !defined(UNALIGNED)
#if defined(_M_IA64) || defined(_M_AMD64)
#define UNALIGNED __unaligned
#else
#define UNALIGNED
#endif
#endif

// RtlpNumberOf is a function that takes a reference to an array of N Ts.
//
// typedef T array_of_T[N];
// typedef array_of_T &reference_to_array_of_T;
//
// RtlpNumberOf returns a pointer to an array of N chars.
// We could return a reference instead of a pointer but older compilers do not accept that.
//
// typedef char array_of_char[N];
// typedef array_of_char *pointer_to_array_of_char;
//
// sizeof(array_of_char) == N
// sizeof(*pointer_to_array_of_char) == N
//
// pointer_to_array_of_char RtlpNumberOf(reference_to_array_of_T);
//
// We never even call RtlpNumberOf, we just take the size of dereferencing its return type.
// We do not even implement RtlpNumberOf, we just decare it.
//
// Attempts to pass pointers instead of arrays to this macro result in compile time errors.
// That is the point.
extern "C++" // templates cannot be declared to have 'C' linkage
	template<typename T, size_t N>
	char (*RtlpNumberOf(UNALIGNED T (&)[N]))[N];

#define RTL_NUMBER_OF_V2(A) (sizeof(*RtlpNumberOf(A)))

#else
#define RTL_NUMBER_OF_V2(A) RTL_NUMBER_OF_V1(A)
#endif

// ARRAYSIZE is more readable version of RTL_NUMBER_OF_V2
// _ARRAYSIZE is a version useful for anonymous types
#define ARRAYSIZE(A)  RTL_NUMBER_OF_V2(A)
#define _ARRAYSIZE(A) RTL_NUMBER_OF_V1(A)

void* Plat_LoadLibrary(const char* path, int mode = -1);
bool Plat_FreeLibrary(void* lib);
void* Plat_GetProcAddress(void* lib, const char* symbol);
void* Plat_GetModuleHandle(const char* path);
bool Plat_GetCommandArgument(const char* argName, char* buffer, size_t maxlength);

namespace LIB {
	constexpr inline const char* gamebin = GAMEBIN;
	constexpr inline const char* addons = GAME_NAME "/addons/cs2surf";
	constexpr inline const char* engine2 = FILLMODULE("engine2");
	constexpr inline const char* server = FILLMODULE("server");
	constexpr inline const char* tier0 = FILLMODULE("tier0");
	constexpr inline const char* networksystem = FILLMODULE("networksystem");
	constexpr inline const char* filesystem_stdio = FILLMODULE("filesystem_stdio");
	constexpr inline const char* schemasystem = FILLMODULE("schemasystem");
	constexpr inline const char* matchmaking = FILLMODULE("matchmaking");
	constexpr inline const char* soundsystem = FILLMODULE("soundsystem");
	constexpr inline const char* resourcesystem = FILLMODULE("resourcesystem");
	constexpr inline const char* steamnetworkingsockets = FILLMODULE("steamnetworkingsockets");
} // namespace LIB

#pragma endregion
