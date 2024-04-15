#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <engine/common/base_types.h>

#include <cstdio>

#define _ASTAR_MAXPATH 1024

class _asnode_base
{
public:
	int links_count = 0;
	_asnode_base** link_nodes = 0;
	int id = -1;
	uint r;
	uint c;
	int flag = 0;

	_asnode_base()
	{
		links_count = 0;
		link_nodes = 0;
		id = -1;
		r = c = 0;
		flag = 0;
	}

	~_asnode_base()
	{
		if(link_nodes){
			delete[] link_nodes;
		}
	}
};

class cAsSortList
{
public:
#define _COLLISION_SORT_INIT_SIZE 2048*8

	struct _asitem
	{
		_asnode_base* node;
		_asnode_base* parent;

		uint steps;
		float rating;
		_asitem* back;
		_asitem* next;

		_asitem(_asnode_base* innode = 0, _asnode_base* inparent = 0, uint insteps = -1, float inrating = 0);
		~_asitem();

		void operator = (_asitem* i);
	};

	_asitem* Root;
	_asitem* Last;

	_asitem* _create_list(uint count);

	cAsSortList();
	~cAsSortList();

	void _reset_list();
	_asitem* GetItem(_asnode_base* node);
	void PushItem(_asitem* initem);
	void PushItem(_asnode_base* node, _asnode_base* parent_node, uint steps);
	bool PopMinItem(_asitem* pitem);
	void DeleteItem(_asitem* item);

};

struct _ASPOINT
{
	int as_id = -1;
	int x = 0;
	int y = 0;

	_ASPOINT()
	{
		as_id = -1;
		x = 0;
		y = 0;
	}

	_ASPOINT(int X, int Y, int Id)
	{
		as_id = Id;
		x = X;
		y = Y;
	}
};

struct _PATH_TRACK
{
	//num количество шагов+1. points[0]==старт целл
	uint num;
	_ASPOINT* points;

	_PATH_TRACK();
	_PATH_TRACK(uint count);
	~_PATH_TRACK();
};

class cAStar
{
public:

	/*enum class _LINK : int
	{
		ERR = -1,
		NS,
		EW,
		SN,
		WE
	};*/

	cAsSortList* Open;
	cAsSortList* Close;

	cAStar();
	~cAStar();

	_PATH_TRACK* build_path();
	_PATH_TRACK* FindPath(uint startX, uint startY, uint targetX, uint targetY, _asnode_base*** rootnode);
	_PATH_TRACK* FindPath(_asnode_base* start_node, _asnode_base* target_node);
};

#endif