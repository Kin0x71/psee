#include "scene.h"
#include <engine/common/gl_head.h>

#include "engine/common/timer.h"
#include <engine/files/file.h>
#include <engine/files/mesh_file.h>
#include <engine/debug/primitives.h>

#include <BulletDynamics/Featherstone/btMultiBodyLinkCollider.h>

//#define DBVT_BP_PROFILING_RATE 256
//#include <LinearMath/btQuickprof.h>

using namespace data_files;

extern cTimer Timer;

cTEvent_System<cScene::EventSceneCallback, cScene::EventSceneArgBase*> cScene::EventScene;
CRITICAL_SECTION cScene::EventSceneCriticalSection;

struct FilterCallback : public btOverlapFilterCallback
{
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const;
};
FilterCallback filterCallback;


bool  FilterCallback::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
{
	//btCollisionObject* collisionObject = (btCollisionObject*)proxy0->m_clientObject;
	//void* userData = (void*)collisionObject->getUserPointer();

	//proxy0->m_collisionFilterGroup
	//proxy0->m_collisionFilterMask
	//return proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask;
	bool collides = (proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) != 0;
	collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

	//printf("FilterCallback::needBroadphaseCollision %d %p %p\n", collides, proxy0->m_clientObject, proxy1->m_clientObject);

	if(collides){

		/*btCollisionObject* collisionObject0 = (btCollisionObject*)proxy0->m_clientObject;
		btCollisionObject* collisionObject1 = (btCollisionObject*)proxy1->m_clientObject;

		const void* ptrA = collisionObject0->getUserPointer();
		const void* ptrB = collisionObject1->getUserPointer();

		cObjectBase* pObjectBaseA = (cObjectBase*)ptrA;
		cObjectBase* pObjectBaseB = (cObjectBase*)ptrB;

		//printf("A:%s->B:%s\n", pObjectBaseA->name, pObjectBaseB->name);

		if(pObjectBaseA && pObjectBaseA->is_collided)
		{
			//printf("A:%s->B:%s %f\n", pObjectBaseA->name, pObjectBaseB->name, Timer.ProgrammTime);
			((cCollidedObject*)pObjectBaseA)->CollideCallback(static_cast<const btRigidBody*>(proxy0->m_clientObject), static_cast<const btRigidBody*>(proxy1->m_clientObject));
		}

		if(pObjectBaseB && pObjectBaseB->is_collided)
		{
			//printf("B:%s->A:%s %f\n", pObjectBaseB->name, pObjectBaseA->name, Timer.ProgrammTime);
			((cCollidedObject*)pObjectBaseB)->CollideCallback(static_cast<const btRigidBody*>(proxy1->m_clientObject), static_cast<const btRigidBody*>(proxy0->m_clientObject));
		}*/
	}

	return collides;
}

//btOverlapCallback

void SceneEvent(cScene::EventSceneArgBase* parg);

cScene::cScene()
{
	pMainCamera = 0;

	/*SceneViewZones = new _scene_view_zone * [MaxViwZoneRows];

	for(int zri = 0; zri < MaxViwZoneRows; ++zri)
	{
		SceneViewZones[zri] = new _scene_view_zone[MaxViwZoneCells];

		for(int zci = 0; zci < MaxViwZoneCells; ++zci)
		{

		}
	}*/
}

cScene::~cScene()
{
	for(int i = CollisionsDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = CollisionsDynamicsWorld->getCollisionObjectArray()[i];
		/*btRigidBody* body = btRigidBody::upcast(obj);
		if(body && body->getMotionState())
		{
			delete body->getMotionState();
		}*/
		CollisionsDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	delete (btDiscreteDynamicsWorld*)CollisionsDynamicsWorld;
	delete (btSequentialImpulseConstraintSolver*)collusions_info.constant_solver;

	delete collusions_info.overlappingPairCache;

	delete collusions_info.dispatcher;

	delete collusions_info.collisionConfiguration;
}

void cScene::EnableGravity(bool enable)
{
	GravityEnabled = enable;

	btVector3 gravity = GravityEnabled ? btVector3(0.0f, -50.0f, 0.0f) : btVector3(0.0f, 0.0f, 0.0f);

	CollisionsDynamicsWorld->setGravity(gravity);
}

void cScene::InitCollisions()
{
	collusions_info.collisionConfiguration = new btDefaultCollisionConfiguration();
	collusions_info.dispatcher = new btCollisionDispatcher(collusions_info.collisionConfiguration);
	collusions_info.overlappingPairCache = new btDbvtBroadphase();

	collusions_info.constant_solver = new btSequentialImpulseConstraintSolver();

	CollisionsDynamicsWorld = new btDiscreteDynamicsWorld(
		collusions_info.dispatcher,
		collusions_info.overlappingPairCache,
		collusions_info.constant_solver,
		collusions_info.collisionConfiguration
	);

	CollisionsDynamicsWorld->setGravity(btVector3(0.0f, -50.0f, 0.0f));

	InitializeCriticalSection(&EventSceneCriticalSection);
	EventScene.Subscribe(SceneEvent);
}

bool cScene::InitMap(const char* landscape_height_map_file, uint field_rows, uint field_cells)
{
	SceneMap = new cMap();

	if(!SceneMap->InitZones(landscape_height_map_file, field_rows, field_cells))return false;

	return true;
}

void SceneEvent(cScene::EventSceneArgBase* parg)
{
	switch(parg->event)
	{
		case cScene::_SCENE_EVENT::ZONE_LOADED:
		{
			cScene::EventSceneArgZoneLoaded* arg_zone_loaded = (cScene::EventSceneArgZoneLoaded*)parg;
			printf("\tZONE_LOADED r:%d c:%d\n", arg_zone_loaded->map_row, arg_zone_loaded->map_cell);

			arg_zone_loaded->plandscape->models_list.reset_enumerator();
			while(cModel* model = arg_zone_loaded->plandscape->models_list.next())
			{
				model->mesh_list.reset_enumerator();
				while(cMesh* mesh = model->mesh_list.next())
				{
					cMesh::init_vertexes(mesh, mesh->vert_buff, mesh->vnum);
					cMesh::init_indexes(mesh, mesh->index_buff, mesh->inum);
				}
			}

			int map_node_id = (arg_zone_loaded->map_row * arg_zone_loaded->scene->SceneMap->Navigation.MapRows) + arg_zone_loaded->map_cell;
			_map_asnode* map_node = &arg_zone_loaded->scene->SceneMap->Navigation.map_nodes[map_node_id];

			map_node->id = map_node_id;
			map_node->r = arg_zone_loaded->map_row;
			map_node->c = arg_zone_loaded->map_cell;
			map_node->min_height = arg_zone_loaded->pzone->min_height;
			map_node->max_height = arg_zone_loaded->pzone->max_height;
			map_node->field = arg_zone_loaded->pzone->field;
			map_node->nodes = arg_zone_loaded->pzone->nodes;
			map_node->nodes_count = arg_zone_loaded->pzone->nodes_count;

			arg_zone_loaded->scene->SceneMap->Navigation.NavZones[arg_zone_loaded->map_row][arg_zone_loaded->map_cell] = map_node;
			arg_zone_loaded->scene->SceneMap->ViewZones[arg_zone_loaded->map_row][arg_zone_loaded->map_cell]->LandscapeObject = arg_zone_loaded->plandscape;

			arg_zone_loaded->scene->SceneMap->ViewZones[arg_zone_loaded->map_row][arg_zone_loaded->map_cell]->ZoneStat = cMap::_view_zone_item::_VIEW_ZONE_STAT::LOADED;

			arg_zone_loaded->scene->CollisionsDynamicsWorld->addRigidBody(
				arg_zone_loaded->scene->SceneMap->ViewZones[arg_zone_loaded->map_row][arg_zone_loaded->map_cell]->LandscapeObject->RigidBodys[0],
				(int)arg_zone_loaded->scene->SceneMap->ViewZones[arg_zone_loaded->map_row][arg_zone_loaded->map_cell]->LandscapeObject->CollideGroups[0],
				-1
			);

			arg_zone_loaded->scene->LandscapeList.push(arg_zone_loaded->scene->SceneMap->ViewZones[arg_zone_loaded->map_row][arg_zone_loaded->map_cell]->LandscapeObject);

			arg_zone_loaded->pzone->field = 0;
			arg_zone_loaded->pzone->nodes = 0;
			delete arg_zone_loaded->pzone;

			delete arg_zone_loaded;
		}
			break;
	}
}

bool cScene::LoadZone(int zone_row, int zone_cell)
{
	//CreateThread()
	printf("cScene::LoadZone(%d, %d)\n", zone_row, zone_cell);

	if(SceneMap->LoadZone(zone_row, zone_cell))
	{
		CollisionsDynamicsWorld->addRigidBody(
			SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject->RigidBodys[0],
			(int)SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject->CollideGroups[0],
			-1
		);

		LandscapeList.push(SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject);

		return true;
	}

	return false;
}

void cScene::SetMap(cMap* map)
{
	SceneMap = map;

	for(int mr = 0; mr < SceneMap->Navigation.MapRows; ++mr)
	{
		for(int mc = 0; mc < SceneMap->Navigation.MapCells; ++mc)
		{
			/*CollisionsDynamicsWorld->addRigidBody(
				SceneMap->ViewZones[mr][mc]->LandscapeObject->RigidBodys[0],
				(int)SceneMap->ViewZones[mr][mc]->LandscapeObject->CollideGroups[0],
				-1
			);*/

			LandscapeList.push(SceneMap->ViewZones[mr][mc]->LandscapeObject);
		}
	}
}

struct _load_zone_threa_arg
{
	cScene* scene = 0;
	char* landscape_height_map_file = 0;
	int zone_rows = 0;
	int zone_cells = 0;
	int map_rows = 0;
	int map_cells = 0;
	int map_row = 0;
	int map_cell = 0;

	_load_zone_threa_arg(cScene* ps, char* lhmf, int nfr, int nfc, int nmr, int nmc, int mr, int mc){
		scene = ps;
		landscape_height_map_file = lhmf;
		zone_rows = nfr;
		zone_cells = nfc;
		map_rows = nmr;
		map_cells = nmc;
		map_row = mr;
		map_cell = mc;
	}
};

DWORD WINAPI LoadZoneThreadProc(void* pParam)
{
	_load_zone_threa_arg* pload_zone_threa_arg = (_load_zone_threa_arg*)pParam;

	cMap::_zone_file* zone_file = cMap::ReadZone(
		pload_zone_threa_arg->landscape_height_map_file,
		pload_zone_threa_arg->zone_rows, pload_zone_threa_arg->zone_cells,
		pload_zone_threa_arg->map_rows, pload_zone_threa_arg->map_cells,
		pload_zone_threa_arg->map_row, pload_zone_threa_arg->map_cell
	);

	EnterCriticalSection(&cScene::EventSceneCriticalSection);

	if(zone_file)
	{
		cScene::EventScene.PushEvent(
			new cScene::EventSceneArgZoneLoaded(
				pload_zone_threa_arg->scene, pload_zone_threa_arg->map_row, pload_zone_threa_arg->map_cell, zone_file->zone, zone_file->landscape
			)
		);

		delete zone_file;
	}

	delete pload_zone_threa_arg;

	LeaveCriticalSection(&cScene::EventSceneCriticalSection);

	return 0;
}

void cScene::DeferredLoadZone(int zone_row, int zone_cell)
{
	SceneMap->ViewZones[zone_row][zone_cell]->ZoneStat = cMap::_view_zone_item::_VIEW_ZONE_STAT::LOADING;

	_load_zone_threa_arg* pload_zone_threa_arg = new _load_zone_threa_arg(
		this,
		SceneMap->LandscapeHeightMapFile,
		SceneMap->Navigation.ZoneRows, SceneMap->Navigation.ZoneCells,
		SceneMap->Navigation.MapRows, SceneMap->Navigation.MapCells,
		zone_row, zone_cell
	);

	HANDLE hThreads = CreateThread(NULL, 0, &LoadZoneThreadProc, pload_zone_threa_arg, 0, NULL);

	/*_map_asnode* ret_zone = 0;
	cLandscapeObject* ret_landscape = 0;

	if(cMap::LoadZone(SceneMap->LandscapeHeightMapFile, SceneMap->ZoneRows, SceneMap->ZoneCells, SceneMap->Navigation.MapRows, SceneMap->Navigation.MapCells, zone_row, zone_cell, &ret_zone, &ret_landscape))
	{
		EventScene.PushEvent(new EventSceneArg(_SCENE_EVENT::ZONE_LOADED, 0, 0));

		SceneMap->Navigation.NavZones[zone_row][zone_cell] = ret_zone;
		SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject = ret_landscape;

		CollisionsDynamicsWorld->addRigidBody(
			SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject->RigidBodys[0],
			(int)SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject->CollideGroups[0],
			-1
		);

		LandscapeList.push(SceneMap->ViewZones[zone_row][zone_cell]->LandscapeObject);
	}*/
}

void cScene::AddShader(shaders_container* sc)
{
	SceneLevelShaders.push(sc);
}

void cScene::update_scene_level_shaders()
{
	SceneLevelShaders.reset_enumerator();

	while(shaders_container* pshader = SceneLevelShaders.next())
	{
		pshader->valuesL_engine.reset_enumerator();

		while(_shader_values_level_item* valcur = pshader->valuesL_engine.next())
		{
			switch(valcur->sem)
			{
				case _VALUESEMANTIC_SPEC::VALSEM_PROJMX:
				pshader->SetMatrix(valcur->id, &pMainCamera->ProjMx);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_UIPROJMX:
				//pshader->SetMatrix(valcur->id, &Ui.UiCore.ProjMx);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_VIEWMX:
				pshader->SetMatrix(valcur->id, &pMainCamera->ViewMx);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_PROJVIEWMX:
				pshader->SetMatrix(valcur->id, &pMainCamera->ProjViewMx);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_VIEWINVMX:
				//pshader->SetMatrix(valcur->id, &ViewInvMx);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_VIEWDIRMX:
				//pshader->SetMatrix(valcur->id, &ViewDirMx);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_VIEWDIR:
				pshader->SetVector(valcur->id, &pMainCamera->dir);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_VIEWPOS:
				pshader->SetVector(valcur->id, &pMainCamera->eye);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_LIGHTVIEWPOS:
				//pshader->SetVectorArray(valcur->id, Lights->ViewPos, Lights->NumLights);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_LIGHTPOSCONE:
				//printf("VALSEM_LIGHTPOSCONE %s\n", valcur->name);
				pshader->SetVectorArray(valcur->id, SceneLights.pos_cone, SceneLights.count);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_LIGHTCOLORPOWER:
				pshader->SetVectorArray(valcur->id, SceneLights.color_power, SceneLights.count);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_LIGHTDIRSPREAD:
				pshader->SetVectorArray(valcur->id, SceneLights.dir_spread, SceneLights.count);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_LIGHTSPECMXARRAY:
				/*printf("VALSEM_LIGHTSPECMXARRAY %s id:%d ptr:%x count:%d\n", valcur->name, valcur->id, SceneLights.LightSpaceMatrces, SceneLights.count);
				//printf("\tval_ptr:%x\n", &pshader->Values[valcur->id]);
				for(int li = 0; li < SceneLights.count; ++li)
				{
					printf("%d %f %f %f\n", li, SceneLights.LightSpaceMatrces[li][3][0], SceneLights.LightSpaceMatrces[li][3][1], SceneLights.LightSpaceMatrces[li][3][2]);
				}*/

				pshader->SetMatrixArray(valcur->id, SceneLights.LightSpaceMatrces, SceneLights.count);
				break;
				case _VALUESEMANTIC_SPEC::VALSEM_SHADOWMAPS:
				//printf("VALSEM_SHADOWMAPS %s\n", valcur->name);
				pshader->SetSamplerArray(valcur->id, SceneLights.DepthMapsIds, SceneLights.count);
				break;
				default:
				printf("update_renderer_level_shaders: %s (sem_id:%d) not setup\n", valcur->name, valcur->sem);
			}
		}
	}
}

void cScene::SetMainCamera(cCamera* pcamera)
{
	pMainCamera = pcamera;
	//SceneViewZones[(MaxViwZoneRows / 2) + 1][(MaxViwZoneCells / 2) + 1].row_id = CurrentViewZonRow;
	//SceneViewZones[(MaxViwZoneRows / 2) + 1][(MaxViwZoneCells / 2) + 1].cell_id = CurrentViewZonCell;

	btDebugDrawer.SetCameraRef(pMainCamera);
	btDebugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);

	CollisionsDynamicsWorld->setDebugDrawer(&btDebugDrawer);
}

void cScene::AllocateLights(int count)
{
	SceneLights.allocate(count);
}

void cScene::SetLight(int light_id)
{
}

void cScene::AddObject(cObjectBase* object)
{
	ObjectList.push(object);

	SceneMap->Navigation.GetMapPosition(object->position.x, object->position.z, &object->NavInfo);

	if(object->ObjectType & OBJECT_TYPE_COLLIDE)
	{
		cCollidedObject* coll_object = (cCollidedObject*)object;

		if(CollisionsDynamicsWorld){

			if(coll_object->RigidBodys){
				for(int i = 0; i < coll_object->RigidBodysCount; ++i)
				{
					if(coll_object->RigidBodys[i]){
						CollisionsDynamicsWorld->addRigidBody(coll_object->RigidBodys[i], (int)coll_object->CollideGroups[i], (int)coll_object->CollideFilters[i]);
					}
				}

				if(coll_object->Connections){
					for(int i = 0; i < coll_object->ConnectionsCount; ++i)
					{
						CollisionsDynamicsWorld->addConstraint(coll_object->Connections[i], false);
					}
				}
			}
		}
	}

	if(object->is_traceable){
		SceneMap->Navigation.AddTrackingObject(object);
	}
}

void cScene::RemoveObject(cObjectBase* object, bool deleting)
{
	printf("RemoveObject(%s)\n", object->name);
	int index=ObjectList.enumerate([object](cObjectBase* cur_obj){
		if(cur_obj == object)
		{
			return true;
		}
		return false;
	});

	if(index!=-1)
	{
		/*if(ObjectList[index]->is_skeleton)
		{
			delete (cSkeletonObject*)ObjectList[index];
		}*/

		ObjectList.pop_index(index);

		if((object->ObjectType & OBJECT_TYPE_COLLIDE))
		{
			cCollidedObject* pcoll_obj = (cCollidedObject*)object;

			if(pcoll_obj->RigidBodys){
				for(int i = 0; i < pcoll_obj->RigidBodysCount; ++i)
				{
					if(pcoll_obj->RigidBodys[i]){
						CollisionsDynamicsWorld->removeRigidBody(pcoll_obj->RigidBodys[i]);
					}
				}
			}

			if(pcoll_obj->Connections){
				for(int i = 0; i < pcoll_obj->ConnectionsCount; ++i)
				{
					if(pcoll_obj->Connections[i]){
						CollisionsDynamicsWorld->removeConstraint(pcoll_obj->Connections[i]);
					}
				}
			}
		}

		/*if(deleting){
			delete ObjectList[index];
		}*/
	}

	SceneMap->Navigation.RemoveTrackingObject(object);
}

int cScene::Update()
{
	EnterCriticalSection(&EventSceneCriticalSection);

	EventScene.UpdateCalls();

	LeaveCriticalSection(&EventSceneCriticalSection);

	if(!pMainCamera)return -1;

	SceneMap->GetZoneByPoint(pMainCamera->eye.x, pMainCamera->eye.z, &CurrentViewZonRow, &CurrentViewZonCell);
			
	ObjectList.reset_enumerator();
	while(cObjectBase* object = ObjectList.next())
	{
		object->Update();
	}

	CollisionsDynamicsWorld->stepSimulation(Timer.ProgrammTime - last_time, (int)Timer.fps);

	last_time = Timer.ProgrammTime;

	//exclude pushing
	btDispatcher* dp = CollisionsDynamicsWorld->getDispatcher();
	const int numManifolds = dp->getNumManifolds();

	//printf("numManifolds:%d %f ######################\n", numManifolds, Timer.ProgrammTime);

	for(int m = 0; m < numManifolds; ++m)
	{
		btPersistentManifold* man = dp->getManifoldByIndexInternal(m);
		//const btRigidBody* RigidBodyA = static_cast<const btRigidBody*>(man->getBody0());
		//const btRigidBody* RigidBodyB = static_cast<const btRigidBody*>(man->getBody1());
		const btCollisionObject* pCollisionObjectA = man->getBody0();
		const btCollisionObject* pCollisionObjectB = man->getBody1();

		const void* ptrA = pCollisionObjectA->getUserPointer();
		const void* ptrB = pCollisionObjectB->getUserPointer();

		cObjectBase* pObjectBaseA = (cObjectBase*)ptrA;
		cObjectBase* pObjectBaseB = (cObjectBase*)ptrB;

		//printf("A:%s->B:%s (%f)\n", pObjectBaseA->name, pObjectBaseB->name, Timer.ProgrammTime);

		int numContacts = man->getNumContacts();
		//printf("  (%s)->(%s) numContacts:%d\n", ((cObjectBase*)man->getBody0()->getUserPointer())->name, ((cObjectBase*)man->getBody1()->getUserPointer())->name, numContacts);
		for(int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = man->getContactPoint(j);

			//printf("    %d %f %s %s\n",j , floor(pt.getDistance() * 100.0f) / 100.0f, pObjectBaseA->name, pObjectBaseB->name);
			//DrawCross(vec3(pt.m_positionWorldOnA.x(), pt.m_positionWorldOnA.y(), pt.m_positionWorldOnA.z()), 0.3f, "\xff\x00\x00");
			//DrawCross(vec3(pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z()), 0.3f, "\x00\x00\xff");

			if(floor(pt.getDistance() * 100.0f) / 100.0f <= 0.000000f)
			{
				const void* ptrA = pCollisionObjectA->getUserPointer();
				const void* ptrB = pCollisionObjectB->getUserPointer();

				cObjectBase* pObjectBaseA = (cObjectBase*)ptrA;
				cObjectBase* pObjectBaseB = (cObjectBase*)ptrB;

				if(pObjectBaseA && pObjectBaseA->is_collided)
				{
					//printf("A:%s->B:%s\n", pObjectBaseA->name, pObjectBaseB->name);
					vec3 contact = vec3(pt.m_positionWorldOnA.x(), pt.m_positionWorldOnA.y(), pt.m_positionWorldOnA.z());
					((cCollidedObject*)pObjectBaseA)->CollideCallback(static_cast<const btRigidBody*>(pCollisionObjectA), contact, static_cast<const btRigidBody*>(pCollisionObjectB));
				}

				if(pObjectBaseB && pObjectBaseB->is_collided)
				{
					//printf("B:%s->A:%s\n", pObjectBaseB->name, pObjectBaseA->name);
					vec3 contact = vec3(pt.m_positionWorldOnB.x(), pt.m_positionWorldOnB.y(), pt.m_positionWorldOnB.z());
					((cCollidedObject*)pObjectBaseB)->CollideCallback(static_cast<const btRigidBody*>(pCollisionObjectB), contact, static_cast<const btRigidBody*>(pCollisionObjectA));
				}
			}
		}

		//printf("\n");
	}

	ObjectList.reset_enumerator();
	while(cObjectBase* object = ObjectList.next())
	{
		object->UpdateAfterTransforms();
	}

	SceneMap->Update();

	//pMainCamera->Update();

	update_scene_level_shaders();

	return 0;
}