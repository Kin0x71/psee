//#include "sizeof_memory.hpp"
#include <stdio.h>
#include  <typeinfo>
#include <intrin.h>

//#include "memlist.h"
//size_t dbg_memsize::total_size = 0;
const char* nullstr = "/0";

bool enable_memcheck = false;

struct _mem_item{
	_mem_item* prev;
	_mem_item* next;

	void* ptr;
	const char* file;
	size_t line;
	unsigned int size;
	bool is_array;
	void* retaddr;

	void constructor()
	{
		prev = 0;
		next = 0;

		ptr = 0;
		file = nullstr;
		line = 0;
		size = 0;
		is_array = false;
		retaddr = 0;
	}
};

class _mem_stack{
public:

	_mem_item* root = 0;

	void push(_mem_item* mem_item)
	{
		if(!root){
			root = mem_item;
			return;
		}

		root->prev = mem_item;
		mem_item->next = root;

		//printf("push:%p next:%p\n", mem_item, mem_item->next);

		root = mem_item;
	}

	_mem_item* find(void* ptr)
	{
		_mem_item* cur = root;
		while(cur)
		{
			//printf("cur:%p next:%p\n", cur, cur->next);
			if(cur->ptr == ptr)return cur;
			cur = cur->next;
		}

		return 0;
	}

	void remove(_mem_item* mem_item)
	{
		if(mem_item->prev){
			if(mem_item->next){
				mem_item->prev->next = mem_item->next;
				mem_item->next->prev = mem_item->prev;
			}
			else{
				mem_item->prev->next = 0;
			}
		}
		else{
			root = root->next;
			root->prev = 0;
		}

		mem_item->next = 0;

		free(mem_item);
	}
};

_mem_stack mem_stack;

const char* __file__ = "\0";
size_t __line__ = 0;

void* __cdecl get_own_retaddr()
{
	__asm
	{
		MOV ECX, DWORD PTR SS : [EBP + 0]
		MOV EAX, DWORD PTR SS : [ECX + 4]
	}
}

void* _new(size_t size, bool is_array, void* retaddr)
{
	void* ptr = malloc(size);

	if(enable_memcheck)
	{
		_mem_item* mem_item = (_mem_item*)malloc(sizeof(_mem_item));

		//unsigned int* uint_ptr = (unsigned int*)ptr;
		//*uint_ptr = (unsigned int)mem_item;
		//++uint_ptr;

		//ptr = uint_ptr;

		mem_item->constructor();
		mem_item->file = __file__;
		mem_item->line = __line__;
		mem_item->ptr = ptr;
		mem_item->size = size;
		mem_item->is_array = is_array;
		mem_item->retaddr = retaddr;

		mem_stack.push(mem_item);
	}

	__file__ = "\0";
	__line__ = 0;

	return ptr;
}

void _delete(void* ptr)
{
	_mem_item* mem_item = mem_stack.find(ptr);

	if(mem_item){
		mem_stack.remove(mem_item);

		//unsigned int* uint_ptr = (unsigned int*)ptr;
		//--uint_ptr;

		//ptr = uint_ptr;
	}
	else if(enable_memcheck){
		printf("delete ptr:%p not found\n", ptr);
	}

	free(ptr);
}

void* operator new(size_t size)
{
	return _new(size, false, get_own_retaddr());
}

void* operator new[](size_t size)
{
	return _new(size, true, get_own_retaddr());
}

void operator delete(void* ptr)
{
	_delete(ptr);
}

void operator delete[](void* ptr)
{
	_delete(ptr);
}

void operator delete(void* ptr, size_t size)
{
	_delete(ptr);
}

void operator delete[](void* ptr, size_t size)
{
	_delete(ptr);
}

void MEMCHECK_enable(bool enable)
{
	enable_memcheck = enable;
}

void MEMCHECK_PrintMemeList()
{
	int total_mem_size = 0;
	int total_items_count = 0;
	_mem_item* cur = mem_stack.root;
	while(cur)
	{
		printf("%p %d %d %s %d [%p]\n",cur->ptr , cur->size,cur->is_array, cur->file, cur->line, cur->retaddr);

		total_mem_size += cur->size;
		++total_items_count;

		cur = cur->next;
	}

	printf("total_mem_size:%d total_items_count:%d\n", total_mem_size, total_items_count);
}

/*

/*
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
inline void* _cdecl operator new(size_t _Size)
{
	dbg_memsize::total_size += _Size;
	//printf("operator new &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	//printf("operator new %p %p\n", __FILE__, __LINE__);
	int* ptr = (int*)malloc(_Size + 4);

	//*ptr = _Size;
	//++ptr;

	return ptr;
}*/
/*
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* _cdecl operator new[](size_t _Size){
	dbg_memsize::total_size += _Size;
	//printf("operator new &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	int* ptr = (int*)malloc(_Size + 4);

	*ptr = _Size;
	++ptr;

	return ptr;
}

void operator delete(void* _Block)
{
	int* ptr = (int*)_Block;
	--ptr;
	size_t size = *ptr;

	dbg_memsize::total_size -= size;

	free(ptr);
}

void operator delete[](void* _Block){
	int* ptr = (int*)_Block;
	--ptr;
	size_t size = *ptr;

	dbg_memsize::total_size -= size;

	free(ptr);
}

void operator delete(void* _Block, size_t _Size)
{
	int* ptr = (int*)_Block;
	--ptr;
	size_t size = *ptr;

	dbg_memsize::total_size -= size;

	free(ptr);
}

void operator delete[](void* _Block, size_t _Size)
{
	int* ptr = (int*)_Block;
	--ptr;
	size_t size = *ptr;

	dbg_memsize::total_size -= size;

	free(ptr);
}*/

/*
void* operator new(size_t size)
{
	dbg_memsize::total_size += size;
	//printf("operator new &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	int* ptr = (int*)malloc(size + 4);

	*ptr = size;
	++ptr;

	return ptr;
}

void* operator new[](size_t size)
{
	dbg_memsize::total_size += size;
	//printf("operator new[] &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	int* ptr = (int*)malloc(size + 4);

	*ptr = size;
	++ptr;

	return ptr;
}

void operator delete(void* ptr, size_t size)
{
	dbg_memsize::total_size -= size;
	printf("operator delete &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	free(ptr);
}

void operator delete[](void* ptr, size_t size)
{
	dbg_memsize::total_size -= size;
	printf("operator delete[] &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	free(ptr);
}

void operator delete[](void* ptr)
{
	//dbg_memsize::total_size -= size;
	printf("operator delete[] &:%x(%d)\n", &dbg_memsize::total_size, dbg_memsize::total_size);
	free(ptr);
}*/