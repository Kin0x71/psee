#ifndef _SCENE_H_
#define _SCENE_H_

#include <engine/object/objects.h>
#include <engine/common/list.h>
#include <engine/shaders/shaders.h>
#include <engine/events/events.h>
#include <engine/camera.h>
#include <engine/map/map.h>

#include "btBulletDynamicsCommon.h"
#include <BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.h>

#include <engine/render/lights.h>
#include <engine/debug/bt_debug_drawer.h>

#ifdef LINUX_DESCTOP_SPACE
#include <pthread.h>
#endif

class cScene {
	struct _collusions_info{
		btDefaultCollisionConfiguration* collisionConfiguration = 0;
		btCollisionDispatcher* dispatcher = 0;
		btBroadphaseInterface* overlappingPairCache = 0;
		btConstraintSolver* constant_solver = 0;
	};

public:

	enum class _SCENE_EVENT{ NON, ZONE_LOADED };

	struct EventSceneArgBase
	{
		_SCENE_EVENT event;
		cScene* scene = 0;

		EventSceneArgBase(){
			event = _SCENE_EVENT::NON;
		}

		EventSceneArgBase(cScene* ps, _SCENE_EVENT e){
			scene = ps;
			event = e;
		}
	};

	struct EventSceneArgZoneLoaded:public EventSceneArgBase
	{
		int map_row = -1;
		int map_cell = -1;
		_map_asnode* pzone = 0;
		cLandscapeObject* plandscape = 0;

		EventSceneArgZoneLoaded(cScene* ps, int zr, int zc, _map_asnode* pz, cLandscapeObject* pl):EventSceneArgBase(ps, _SCENE_EVENT::ZONE_LOADED){
			map_row = zr;
			map_cell = zc;
			pzone = pz;
			plandscape = pl;
		}
	};

	typedef void(EventSceneCallback)(EventSceneArgBase*);

	static cTEvent_System<EventSceneCallback, EventSceneArgBase*> EventScene;

#ifdef LINUX_DESCTOP_SPACE
	static pthread_mutex_t EventSceneCriticalSection;
#elif defined(WINDOWS_SPACE)
	static CRITICAL_SECTION EventSceneCriticalSection;
#endif
	//------------------------------------------------------------------

	int MaxViwZoneRows = 9;
	int MaxViwZoneCells = 9;

	cList<shaders_container*> SceneLevelShaders;

	cList<cObjectBase*> ObjectList;
	cList<cObjectBase*> LandscapeList;
	cCamera* pMainCamera;

	cLights SceneLights;

	_collusions_info collusions_info;
	btDynamicsWorld* CollisionsDynamicsWorld = 0;
	btRigidBody* GroundRigidBody = 0;

	float last_time = 0.0f;

	cObjectModel SkyBox;

	struct _scene_view_zone{
		int row_id = -1;
		int cell_id = -1;
	};

	cMap* SceneMap = 0;
	//_scene_view_zone** SceneViewZones = 0;
	int CurrentViewZonRow = -1;
	int CurrentViewZonCell = -1;


	BTDebugDrawer btDebugDrawer;

	//cTEvent_System<EventCallback_Scene, void, void, uint, uint, uint> EventScene;

	cScene();
	~cScene();

	bool GravityEnabled = true;
	void EnableGravity(bool enable);

	void InitCollisions();
	bool InitMap(const char* landscape_height_map_file, uint field_rows, uint field_cells);
	
	bool LoadZone(int zone_row, int zone_cell);
	void SetMap(cMap* map);
	void DeferredLoadZone(int zone_row, int zone_cell);//отложенная загрузка

	void AddShader(shaders_container* sc);
	void update_scene_level_shaders();

	void SetMainCamera(cCamera* pcamera);

	void AllocateLights(int count);
	void SetLight(int light_id);

	void AddObject(cObjectBase* object);
	void RemoveObject(cObjectBase* object, bool deleting = false);

	int Update();
};

#endif