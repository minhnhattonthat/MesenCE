#include "pch.h"
#include "VirtualMemory.h"

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <sys/mman.h>
#endif

void* VirtualMemory::AllocZeroed(size_t size)
{
	if(size == 0) {
		return nullptr;
	}
#ifdef _WIN32
	//Committed pages are demand-zero: no physical page is assigned until first access
	void* ptr = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(!ptr) {
		throw std::bad_alloc();
	}
#else
	void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED) {
		throw std::bad_alloc();
	}
#endif
	return ptr;
}

void VirtualMemory::Free(void* ptr, size_t size)
{
	if(!ptr) {
		return;
	}
#ifdef _WIN32
	(void)size;
	VirtualFree(ptr, 0, MEM_RELEASE);
#else
	munmap(ptr, size);
#endif
}
