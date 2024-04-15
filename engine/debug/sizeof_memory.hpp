#ifndef _SIZEOF_MEMORY_
#define _SIZEOF_MEMORY_

#include <stdlib.h>
#include <new>
#include <typeinfo>

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, size_t size);
void operator delete[](void* ptr, size_t size);

extern const char* __file__;
extern size_t __line__;
#define new (__file__=__FILE__,__line__=__LINE__) && 0 ? NULL : new

void MEMCHECK_enable(bool enable);
void MEMCHECK_PrintMemeList();

//_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
//inline void* _cdecl operator new(size_t _Size);

/*
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* _cdecl operator new[](size_t _Size);

void operator delete(void* _Block);
void operator delete[](void* _Block);
void operator delete(void* _Block, size_t _Size);
void operator delete[](void* _Block, size_t _Size);
*/
/*
void* new_checkmem(size_t _Size, const char* file, int line)
{
	return malloc(_Size);
}
*/
//#define  operator_new new
//#define operator new(_Size) new_checkmem(_Size, __FILE__, __LINE__)

#endif