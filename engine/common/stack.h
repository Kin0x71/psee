#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>

template <typename T>
class cStack {
public:
	struct _item {
		_item* next;
		_item* prev;
		T v;
		_item() {
			next = 0;
			prev = 0;
		}
		~_item() {
			if(v) {
				//delete v;
				v = 0;
			}

			if(next) {
				delete next;
				next = 0;
			}
		}
	};
	
	_item* Buff;
	_item* LastBuffItem;
	_item* Last;
	_item* Root;

	cStack() {
		Buff = LastBuffItem = new _item();
		Root = Last = 0;
	}

	~cStack() {

		if(Root) {
			delete Root;
			Root = 0;
		}
	}

	void Allocate(int size)
	{
		Buff = new _item[size];

		for(int i = 0; i < size - 1; ++i)
		{
			Buff[i]->next = &Buff[i + 1];
		}
	}

	int SizeOfList()
	{
		if(!Root)return 0;
		int size = 0;
		_item* cur = Root;
		do
		{
			++size;
		} while(cur = cur->next);
		return size;
	}

	int SizeOfBuff()
	{
		if(!Buff)return 0;
		int size = 0;
		_item* cur = Buff;
		do
		{
			++size;
		} while(cur = cur->next);
		return size;
	}

	void Swap()
	{
		if(Root && Buff) {

			LastBuffItem->next = Root;
			Root->prev = LastBuffItem;

			LastBuffItem = Last;
		}

		Root = Last = 0;
	}
	
	_item* PopFreeSlot()
	{
		_item* ret = Buff;

		if(!Buff->next) {
			LastBuffItem = Buff->next = new _item();
		}

		Buff = Buff->next;

		return ret;
	}

	void PushFreeSlot(_item * item)
	{
		item->prev = 0;
		item->next = Buff;
		Buff = item;
	}

	_item* Push(T item)
	{
		_item* i = PopFreeSlot();

		PushItem(i);

		i->v = item;

		return i;
	}

	void PushItem(_item * i)
	{
		i->next = Root;
		i->prev = 0;

		if(Root) {
			Root->prev = i;
		}
		else {
			Last = i;
		}

		Root = i;
	}

	T Pop()
	{
		if(!Root)return (T)-1;

		_item* i = Root;
		T v = i->v;

		Root = Root->next;

		PushFreeSlot(i);

		return v;
	}

	T Get(T v)
	{
		if(!Root)return (T)-1;

		_item* cur = Root;
		do
		{
			if(cur->v == v) {
				return Free(cur);
			}

		} while(cur = cur->next);

		return (T)-1;
	}

	bool Find(T v)
	{
		_item* cur = Root;
		do
		{
			if(cur->v == v) {
				return true;
			}

		} while(cur = cur->next);

		return false;
	}

	T Free(_item * item)
	{
		if(item->prev && item->next)
		{
			item->next->prev = item->prev;
			item->prev->next = item->next;
		}
		else if(!item->prev && item->next)
		{
			Root = item->next;
			Root->prev = 0;
		}
		else if(item->prev && !item->next)
		{
			item->prev->next = 0;
		}
		else {
			Root = 0;
			item->prev = 0;
			item->next = 0;
		}

		T v = item->v;

		PushFreeSlot(item);

		return v;
	}

	void Unlink(_item * item)
	{
		if(item->prev && item->next)
		{
			item->next->prev = item->prev;
			item->prev->next = item->next;
		}
		else if(!item->prev && item->next)
		{
			Root = item->next;
			Root->prev = 0;
		}
		else if(item->prev && !item->next)
		{
			item->prev->next = 0;
		}

		item->prev = 0;
		item->next = 0;
	}
};

#endif