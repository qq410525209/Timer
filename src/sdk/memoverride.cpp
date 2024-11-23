#include <memalloc.h>

inline void* AllocUnattributed(size_t nSize) {
	return MemAlloc_Alloc(nSize);
}

#ifdef HAVE_MEMOVERRIDE_NEW_DELETE
void* __cdecl operator new(size_t nSize) {
	return AllocUnattributed(nSize);
}

void* __cdecl operator new(size_t nSize, int nBlockUse, const char* pFileName, int nLine) {
	return MemAlloc_Alloc(nSize, pFileName, nLine);
}

void __cdecl operator delete(void* pMem) {
	g_pMemAlloc->Free(pMem);
}

void* __cdecl operator new[](size_t nSize) {
	return AllocUnattributed(nSize);
}

void* __cdecl operator new[](size_t nSize, int nBlockUse, const char* pFileName, int nLine) {
	return MemAlloc_Alloc(nSize, pFileName, nLine);
}

void __cdecl operator delete[](void* pMem) {
	g_pMemAlloc->Free(pMem);
}
#endif
