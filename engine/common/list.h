#ifndef _CLIST_H_
#define _CLIST_H_

#include <functional>
#include <stdio.h>

template <class T>
T _zero()
{
	return T(); // or T(0);
}

template <typename T>
struct _item {
	_item* next;

	T value;
	_item(){
		next = 0;
	}
	~_item(){
		if(next){
			delete next;
			next = 0;
		}
	}
};

template <typename T>
class cList
{
public:
	_item<T>* root;
	_item<T>* last;
	_item<T>* enum_cur;
	int count;
	bool as_array;
	//если использован allocate работаем как с массивом, идексированный доступ аботает как обычный массив
	//любой push или pop сбрасывает этот флаг, работаем как со списком. идексированный доступ будет выполнять перебор списка от начала

	cList(){
		root = last = enum_cur = new _item<T>();
		count = 0;
		as_array = false;
	}

	~cList(){
		if(root){
			delete root;
			root = 0;
		}
	}

	void allocate(int items_count){		
		count = items_count;

		_item<T>* items_array = new _item<T>[count + 1];

		root = &items_array[0];
		enum_cur = &items_array[0];
		last = &items_array[count];

		_item<T>* cur = root;
		for(int i = 0; i < count; ++i)
		{
			cur->value = 0;
			cur->next = &items_array[i + 1];
			cur = cur->next;
		}

		cur->value = 0;
		cur->next = 0;

		as_array = true;
	}

	unsigned int push(const T& value){
		last->value = value;
		last->next = new _item<T>();
		last = last->next;

		as_array = false;

		return count++;
	}

	unsigned int push(T& value) {
		last->value = value;
		last->next = new _item<T>();
		last = last->next;

		as_array = false;

		return count++;
	}

	T pop(){
		if(root && root->next){
			T retval = root->value;
			_item<T>* first = root;

			root = root->next;

			first->next = 0;
			delete first;

			--count;

			as_array = false;

			return retval;
		}

		return _zero<T>();
	}

	T pop_last(){
		if(!root || !root->next)return 0;

		_item<T>* c = root;

		while(c->next)
		{
			c = c->next;
		}

		T v = c->value;
		c->next = 0;
		delete c;

		--count;

		as_array = false;

		return v;
	}

	void free()
	{
		delete root->next;
		root->next = 0;
		last = root;
		count = 0;
	}

	//for breaking return true
	typedef std::function<bool(T)> _enumerate_callback;
	int enumerate(_enumerate_callback enumerate_callback){

		_item<T>* c = root;
		int i = 0;
		while(c->next)
		{
			if(enumerate_callback(c->value))
			{
				return i;
			}
			c = c->next;
			++i;
		}

		return -1;
	}

	typedef bool _compaer_callback(T,T);
	int compare(T value, _compaer_callback compaer_callback){
		_item<T>* c = root;
		int i = 0;
		while(c->next)
		{
			if(compaer_callback(c->value, value))
			{
				return i;
			}
			c = c->next;
			++i;
		}

		return -1;
	}

	void sort(_compaer_callback compaer_callback)
	{
		if(!root || !root->next)return;

		_item<T>* a = root;
		while(a->next)
		{
			_item<T>* min_item = a;

			_item<T>* b = a;
			while(b->next)
			{
				if(b != a)
				{
					if(compaer_callback(b->value, min_item->value)){
						min_item = b;
					}
				}

				b = b->next;
			}

			if(min_item != a)
			{
				T t = a->value;
				a->value = min_item->value;
				min_item->value = t;
			}

			a = a->next;
		}

		as_array = false;
	}

	inline void reset_enumerator()
	{
		enum_cur = root;
	}

	T next(){

		if(enum_cur && enum_cur->next){
			T retval = enum_cur->value;
			enum_cur = enum_cur->next;
			//printf(">>%x\n", retval);
			return retval;
		}
		enum_cur = root;

		return _zero<T>();
	}

	T* next_ptr(){
		//printf("cur:%x && cur->next:%x\n",cur ,cur->next);
		if(enum_cur && enum_cur->next){
			T* retval = &enum_cur->value;
			enum_cur = enum_cur->next;
			return retval;
		}
		enum_cur = root;
		return 0;
	}

	T pop_index(int i){
		if(!root || !root->next || i >= count)return _zero<T>();

		if(as_array){
			return root[i].value;
		}

		_item<T>* c = root;
		_item<T>* prev = root;
		int iterator = 0;
		bool ftr = false;

		while(c->next)
		{
			if(iterator++ == i){
				ftr = true;
				break;
			}
			prev = c;
			c = c->next;
		}

		if(!ftr)return _zero<T>();

		T v = c->value;

		if(c == root)
		{
			root = root->next;
		}
		else{
			prev->next = c->next;
		}

		c->next = 0;

		delete c;

		--count;

		as_array = false;

		return v;
	}

	T* get_index_ptr(int i){
		if(!root || !root->next || i >= count)return 0;

		if(as_array){
			return &root[i].value;
		}

		_item<T>* c = root;
		int iterator = 0;
		bool ftr = false;

		while(c->next)
		{
			if(iterator++ == i){
				ftr = true;
				break;
			}

			c = c->next;
		}

		if(!ftr)return 0;

		return &c->value;
	}

	T operator[] (int i){
		if(!root || !root->next || i >= count)return _zero<T>();

		if(as_array){
			return root[i].value;
		}

		_item<T>* c = root;
		int iterator = 0;

		while(c->next)
		{
			if(iterator++ == i){
				return c->value;
			}
			c = c->next;
		}

		return _zero<T>();
	}

	int find(T value)
	{
		if(!root || !root->next)return -1;

		_item<T>* c = root;
		int iterator = 0;

		while(c->next)
		{
			if(c->value==value){
				return iterator;
			}
			c = c->next;
			++iterator;
		}

		return -1;
	}

	void dbg_print_links()
	{
		_item<T>* c = root;

		while(c->next)
		{
			printf("\t%d\n", c->next->value);
			c = c->next;
		}

	}
};

#endif