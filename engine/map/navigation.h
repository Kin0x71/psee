#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include "astar.h"
#include <engine/common/list.h>
#include <engine/object/objects.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

using namespace glm;

class _zone_asnode :public _asnode_base{
public:
	vec3 local_pos;
	float plane_angle = 0.0f;
};

struct _map_asnode :public _asnode_base{
	int id = -1;

	int nodes_count = 0;
	_zone_asnode* nodes = 0;
	_zone_asnode*** field = 0;
	int field_rows = 0;
	int field_cells = 0;
	float min_height = 99999.0f;
	float max_height = -99999.0f;

	void AllockField(int NodesCount, int ZoneRows, int ZoneCells);
	void BuildLinks(cList<_zone_asnode*>& cells_list);

	~_map_asnode()
	{
		if(nodes){
			delete[] nodes;
		}

		if(field){

			for(int ri = 0; ri < field_rows; ++ri){
				delete[] field[ri];
			}

			delete[] field;
		}
	}
};

class cNavigation{
public:

	struct _tracking_item{
		cObjectBase* object;
		vec3 last_pos;

		_tracking_item(){
			object = 0;
			last_pos = vec3(0.0f, 0.0f, 0.0f);
		}

		_tracking_item(cObjectBase* Obj, int ZoneRow, int ZoneCell, int Row, int Cell, int FieldCellId){
			object = Obj;
			last_pos = vec3(object->position.x, object->position.y, object->position.z);
		}
	};

	cAStar AStar;

	//_asnode* as_zones = 0;

	uint MapRows = 0;
	uint MapCells = 0;
	uint ZoneRows = 0;
	uint ZoneCells = 0;
	_map_asnode*** NavZones = 0;
	int map_nodes_count = 0;
	_map_asnode* map_nodes = 0;
	//cList<_map_asnode*> NavZonesList;

	cList<_tracking_item*> tracking_objects;

	void convert_pos_to_loop(int* r, int* c, int rows, int cells);

	int BuildZonesLinks();

	void AddTrackingObject(cObjectBase* object);
	void RemoveTrackingObject(cObjectBase* object);

	void GetMapPosition(float x, float z, _map_pos* ret);

	void Update();

	bool SetNode(int zone_id,int cell_id);

	struct _path_container{
		int row = -1;
		int cell = -1;
		cList<vec3> path_list;

		~_path_container()
		{
			path_list.free();
		}
	};

	bool MapFindPath(const _i2p& start, const _i2p& end);
	cList<vec3>* ZoneFindPath(_map_asnode* zone, vec2& local_start, vec2& local_end);
};

#endif