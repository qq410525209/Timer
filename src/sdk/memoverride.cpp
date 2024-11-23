#include <memalloc.h>

inline void* AllocUnattributed(size_t nSize) {
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
	return MemAlloc_Alloc(nSize);
#else
	return MemAlloc_Alloc(nSize, g_pszModule, 0);
#endif
}

#ifdef HAVE_MEMOVERRIDE_NEW_DELETE
void* __cdecl operator new(size_t nSize) {
	return AllocUnattributed(nSize);
}

void* __cdecl operator new(size_t nSize, int nBlockUse, const char* pFileName, int nLine) {
	return MemAlloc_Alloc(nSize, pFileName, nLine);
}

void __cdecl operator delete(void* pMem) {
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
	g_pMemAlloc->Free(pMem);
#else
	g_pMemAlloc->Free(pMem, g_pszModule, 0);
#endif
}

void* __cdecl operator new[](size_t nSize) {
	return AllocUnattributed(nSize);
}

void* __cdecl operator new[](size_t nSize, int nBlockUse, const char* pFileName, int nLine) {
	return MemAlloc_Alloc(nSize, pFileName, nLine);
}

void __cdecl operator delete[](void* pMem) {
#if !defined(USE_LIGHT_MEM_DEBUG) && !defined(USE_MEM_DEBUG)
	g_pMemAlloc->Free(pMem);
#else
	g_pMemAlloc->Free(pMem, g_pszModule, 0);
#endif
}
#endif
