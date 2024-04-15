#ifndef _MAP_H_
#define _MAP_H_

#include <string.h>

#include <engine/common/list.h>

#include <glm/vec2.hpp>

#include <btBulletDynamicsCommon.h>

#include <engine/object/objects.h>

#include "navigation.h"

using namespace glm;

#pragma pack(push, 1)
struct _plane_vert{
	vec3 pos;
	vec3 normal;
	vec3 binormal;
	vec3 tangent;
	vec2 tex;
};
#pragma pack(pop)
/*
class cRoomPlaneObject :public cCollidedObject {
public:

	cRoomPlaneObject(){};
	~cRoomPlaneObject(){};

	void Update();
	void UpdateAfterTransforms();
	void RenderShadowDepth(mat4* plightmx);
	void Draw(cCamera* pCamera);
};
*/
class cWallsObject :public cCollidedObject {
public:

	cWallsObject(){};
	~cWallsObject(){};

	void Update(){};
	void UpdateAfterTransforms(){};
	//void RenderShadowDepth(mat4* plightmx);
	//void Draw(cCamera* pCamera);
};

class cJointObject :public cCollidedObject {
public:

	cJointObject(){
		//is_cast_shadow = false;
	};
	~cJointObject(){};

	void Update(){};
	void UpdateAfterTransforms(){};
	//void RenderShadowDepth(mat4* plightmx){};
	//void Draw(cCamera* pCamera);
};

class cFloorObject :public cCollidedObject {
public:

	cFloorObject(){
		//is_cast_shadow = false;
	};
	~cFloorObject(){};

	void Update(){};
	void UpdateAfterTransforms(){};
	//void RenderShadowDepth(mat4* plightmx){};
	//void Draw(cCamera* pCamera);
};

class cLandscapeObject :public cCollidedObject {
public:
	bool already_collided = false;
	cLandscapeObject(){
		//is_cast_shadow = false;
	};
	~cLandscapeObject(){};

	int InitGeometrix(_plane_vert* vertices, int vertices_count, word* indexes, int indexes_count, btTriangleMesh* triangle_mesh, bool init_ogl_mesh = false);

	void Update(){};
	void UpdateAfterTransforms(){};
	//void RenderShadowDepth(mat4* plightmx){};
};

class cHouseObject :public cCollidedObject {
public:

	cHouseObject(){};
	~cHouseObject(){};

	void Update(){};
	void UpdateAfterTransforms(){};
	void InitCollision();
};

class cZone
{
public:
	int id = -1;
	vec2 min_point = vec2(9999.0f, 9999.0f);
	vec2 max_point = vec2(-9999.0f, -9999.0f);
	cFloorObject* pFloorObject = 0;
	cWallsObject* pWallsObject = 0;
};

class cMap
{
public:

	struct _line{
		vec2 pa;
		vec2 pb;
		vec2 pn;

		_line()
		{

		}

		_line(float xa, float ya, float xb, float yb, float nx, float ny)
		{
			pa.x = xa;
			pa.y = ya;
			pb.x = xb;
			pb.y = yb;
			pn.x = nx;
			pn.y = ny;
		}
	};

	struct _quad{
		vec2 pa;
		vec2 pb;
		vec2 pc;
		vec2 pd;

		_quad()
		{

		}

		_quad(float xa, float ya, float xb, float yb, float xc, float yc, float xd, float yd)
		{
			pa.x = xa;
			pa.y = ya;
			pb.x = xb;
			pb.y = yb;
			pc.x = xc;
			pc.y = yc;
			pd.x = xd;
			pd.y = yd;
		}
	};

#pragma pack(push, 1)
	struct _cell_file{
		int x = 0;
		int y = 0;

		_cell_file(){
			x = -1;
			y = -1;
		}

		_cell_file(int X, int Y){
			x = X;
			y = Y;
		}
	};

	struct _door_file{
		float x = 0;
		float y = 0;
		int dir = 0;
	};
#pragma pack(pop)

	/*struct _zone_file
	{
		int walls_count = 0;
		int tiles_vcount = 0;
		int tiles_icount = 0;
		int plates_count = 0;
		int cells_count = 0;
		int joints_count = 0;
		int field_rows = 0;
		int field_cells = 0;
		int cell_xmin = 0;
		int cell_ymin = 0;
		int cell_xmax = 0;		
		int cell_ymax = 0;

		_line* walls = 0;
		vec4* p_tiles_vt = 0;
		word* p_tiles_i = 0;
		_quad* plates = 0;
		_cell_file* cells = 0;
		int* joints = 0;

		void allock();
		~_zone_file();
	};

	struct _map_file
	{
		int zones_count = 0;
		int doors_count = 0;
		_zone_file* zones = 0;
		_door_file* pdoors = 0;
		vec2 player_pos;

		_map_file(int zc, int dc, _zone_file* pzf, _door_file* pd, const vec2 pp);
		~_map_file();
	};*/

	//удалить
	struct _zone_file
	{
		_map_asnode* zone;
		cLandscapeObject* landscape;
	};

	char* Name = 0;
	char* LandscapeHeightMapFile = 0;
	//btVector3 offs;// = btVector3(-50.0f, 0.0f, -50.0f);

	//cList<cZone*> ZonesList;
	//cList<cJointObject*> JointsList;

	struct _view_zone_item{

		enum class _VIEW_ZONE_STAT{NON,LOADING,LOADED};

		_VIEW_ZONE_STAT ZoneStat = _VIEW_ZONE_STAT::NON;

		cLandscapeObject* LandscapeObject = 0;
		vec2 view_box_a;
		vec2 view_box_b;
		vec2 view_box_c;
		vec2 view_box_d;
		int row_id = -1;
		int cell_id = -1;

		_view_zone_item(_VIEW_ZONE_STAT vzs, const vec2& vba, const vec2& vbb, const vec2& vbc, const vec2& vbd){
			ZoneStat = vzs;
			view_box_a = vba;
			view_box_b = vbb;
			view_box_c = vbc;
			view_box_d = vbd;
		}
	};

	_view_zone_item*** ViewZones = 0;

	uint FullMapWidth = 0;
	uint FullMapHeight = 0;

	cNavigation Navigation;

	~cMap();
	void set_name(char* n);

	inline void GetZoneByPoint(float x, float y, int* ret_zone_row, int* ret_zone_cell){
		*ret_zone_cell = x / Navigation.ZoneCells;
		*ret_zone_row = y / Navigation.ZoneRows;
	}

	//_map_file* read_house_map(const char* filename);
	bool InitZones(const char* landscape_height_map_file, uint field_rows, uint field_cells);
	void InitZones(uint map_rows, uint map_cells, uint field_rows, uint field_cells);

	static _zone_file* ReadZone(
		char* landscape_height_map_file,
		int field_rows,
		int field_cells,
		int zone_rows,
		int zone_cells,
		int zone_row,
		int zone_cell
	);
	bool LoadZone(int map_row, int map_cell);

	void WriteMapOld(const char* fname);
	void ReadMapOld(const char* fname);
	void WriteMap(const char* fname);
	void ReadMap(const char* fname);

	void DBGCreateBox();
	
	void Update();
};

#endif