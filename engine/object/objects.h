#ifndef _BASE_OBJECT_H_
#define _BASE_OBJECT_H_

#include <engine/common/exception.h>
#include <engine/mesh/model.h>
#include <engine/camera.h>
#include <engine/render/lights.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Featherstone/btMultiBody.h>

#define BIT(x) (1<<(x))
enum OBJECT_TYPE
{
	OBJECT_TYPE_BASE = BIT(1),
	OBJECT_TYPE_MODEL = BIT(2),
	OBJECT_TYPE_COLLIDE = BIT(3),
	OBJECT_TYPE_SKELETON = BIT(4)
};

struct _i2p{
	union {
		int x, cell;
	};

	union {
		int y, z, row;
	};

	_i2p(){
		x = -1;
		y = -1;
	}

	_i2p(int r, int c){
		x = c;
		y = r;
	}

	_i2p operator = (const _i2p& p){
		return _i2p(p.x, p.y);
	}

	bool operator == (const _i2p& p){
		return x == p.x && y == p.y;
	}

	bool operator != (const _i2p& p){
		return !(x == p.x && y == p.y);
	}

	void set(int r, int c){
		x = c;
		y = r;
	}
};

struct _map_pos{
	_i2p map_pos;
	_i2p zone_pos;

	void set(int mr, int mc, int zr, int zc){
		map_pos.set(mr, mc);
		zone_pos.set(zr, zc);
	}
};

class cObjectBase {
public:
	OBJECT_TYPE ObjectType = OBJECT_TYPE_BASE;

	mat4 WorldMx = mat4(1.0f);
	vec3 position = vec3(0.0f, 0.0f, 0.0f);
	vec3 scaling = vec3(1.0f, 1.0f, 1.0f);

	_map_pos NavInfo;
	
	bool is_in_frustum = false;
	bool is_rendered = false;
	bool is_collided = false;
	bool is_multi_collided = false;
	bool is_cast_shadow = false;
	bool is_skeleton = false;
	bool is_character = false;
	bool is_traceable = false;
	char* name = 0;

	cObjectBase();
	~cObjectBase();

	virtual void release(){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
	void set_name(const char* n);

	void SetPosition(float x, float y, float z);
	void SetRotation(float angle, const vec3& v);
	void SetScaling(float x, float y, float z);

	virtual vec3 GetPosition(){ return vec3(); };

	virtual void Update(){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
	virtual void UpdateAfterTransforms(){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
	virtual void Draw(cCamera* pCamera){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
	virtual bool UpdateFrustumViewBox(cCamera* pCamera){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
	virtual void SetupShadows(shaders_container* pShader, int TechId){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
};

class cObjectModel :public cObjectBase {
public:

	cList<cModel*> models_list;

	struct _shadowed_mesh_item
	{
		cMesh* Mesh = 0;
		cMeshShaderController* pShadowDepthShaderController = 0;

		_shadowed_mesh_item(cMesh* mesh, cMeshShaderController* shadow_depth_shader_controller)
		{
			Mesh = mesh;
			pShadowDepthShaderController = shadow_depth_shader_controller;
		}
	};

	mat4 Shadows_LightSpaceMatrix;
	cList<_shadowed_mesh_item*> ShadowedMeshList;

	cObjectModel(){
		ObjectType = (OBJECT_TYPE)(ObjectType | OBJECT_TYPE_MODEL);
		is_rendered = true;
	}
	
	~cObjectModel();

	void AddModel(cModel*);

	void Update();

	virtual void UpdateAfterTransforms(){}

	void Draw(cCamera* pCamera);

	bool UpdateFrustumViewBox(cCamera* pCamera);

	void SetupShadows(shaders_container* pShader, int TechId);
	void GathereShadowDepth(mat4* plightmx);

	void DBG_DrawNormals(cCamera* pCamera);
};

enum COLLISION_GROUPS
{
	COLLISION_GROUP_NOTHING = 0,
	COLLISION_GROUP_LANDSCAPE = BIT(1),
	COLLISION_GROUP_WALLS = BIT(2),
	COLLISION_GROUP_UNITS = BIT(3),
	COLLISION_GROUP_ALL = BIT(1) | BIT(2) | BIT(3)
};

enum COLLISION_FILTERS
{
	//COL_SPHERE = BIT(2)
	//COL_TRIANGLE = BIT(3)
	//COL_QUAD = BIT(4)
};

class cCollidedObject :public cObjectModel {
public:

	int RigidBodysCount = 0;
	btRigidBody** RigidBodys = 0;
	COLLISION_GROUPS* CollideGroups = 0;
	COLLISION_GROUPS* CollideFilters = 0;

	int ConnectionsCount = 0;
	btTypedConstraint** Connections = 0;

	vec3 rigidbodys_offset;
	btVector3 btMovingVelocity;

	cCollidedObject()
	{
		ObjectType = (OBJECT_TYPE)(ObjectType | OBJECT_TYPE_COLLIDE);
		is_multi_collided = true;
	}

	~cCollidedObject();

	void allocate_rigidbodys(int count){
		RigidBodysCount = count;
		RigidBodys = new btRigidBody * [RigidBodysCount];
		CollideGroups = new COLLISION_GROUPS[RigidBodysCount];
		CollideFilters = new COLLISION_GROUPS[RigidBodysCount];

		for(int i = 0; i < RigidBodysCount; ++i)
		{
			RigidBodys[i] = 0;
			CollideGroups[i] = COLLISION_GROUP_NOTHING;
			CollideFilters[i] = COLLISION_GROUP_NOTHING;
		}
	}

	void allocate_connections(int count){
		ConnectionsCount = count;
		Connections = new btTypedConstraint * [ConnectionsCount];

		for(int i = 0; i < ConnectionsCount; ++i)
		{
			Connections[i] = 0;
		}
	}

	virtual void InitCollision(){ throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__); }
	virtual void CollideCallback(const btRigidBody* SelfRigidBody, const vec3& contact, const btRigidBody* pCollisionRigidBody){}
	void CreateCollideMesh(vec3* vertices, int vertices_count, word* indeces, int indeces_count);

	void SetPosition(float x, float y, float z);
	void SetRotation(float angle, const vec3& v);
	void SetScaling(float x, float y, float z);
	vec3 GetPosition();
	void UpdateAfterTransforms();
};

class cSkeletonObject :public cCollidedObject {
public:

	cSkeleton* pSkeleton = 0;
	//cAnimation* pAnimation = 0;

	bool dbg_draw_bones = false;

	cSkeletonObject(){
		ObjectType = (OBJECT_TYPE)(ObjectType | OBJECT_TYPE_SKELETON);
		is_skeleton = true;
	}
	~cSkeletonObject();

	void release();

	void InitCollision();

	void _init_trans_animation(cAnimation* pTransAnimation);
	void _setup_trans_animation(cAnimation* TransAnimation, cAnimation* AnimFrom, cAnimation* AnimTo, float trans_duration = 1.0f);

	void Update();
	virtual void UpdateAfterTransforms();
	//void Draw(cCamera* pCamera);

	void SetupShadows(shaders_container* pShader, int TechId)
	{
		is_cast_shadow = true;

		models_list.reset_enumerator();
		while(cModel* model = models_list.next())
		{
			model->mesh_list.reset_enumerator();
			while(cMesh* mesh = model->mesh_list.next())
			{
				if(!mesh->is_cast_shadow)continue;

				cMeshShaderController* pshadow_depth_shader_controller = new cMeshShaderController();

				pshadow_depth_shader_controller->AddShader(pShader);
				pshadow_depth_shader_controller->AddTech(pShader, TechId);

				pshadow_depth_shader_controller->AddValue(
					pShader,
					TechId,
					0,
					_VALUESEMANTIC_SPEC::VALSEM_WORLDMX,
					&model->ModelWorldMx
				);

				pshadow_depth_shader_controller->AddValue(
					pShader,
					TechId,
					0,
					_VALUESEMANTIC_SPEC::VALSEM_LIGHTSPECMX,
					&Shadows_LightSpaceMatrix
				);

				pshadow_depth_shader_controller->AddValue(
					pShader,
					TechId,
					0,
					_VALUESEMANTIC_SPEC::VALSEM_BONES,
					pSkeleton->pMxNodesResult,
					&pSkeleton->BonesCount
				);

				pshadow_depth_shader_controller->AddValue(
					pShader,
					TechId,
					0,
					_VALUESEMANTIC_SPEC::VALSEM_WEIGHTTEX,
					&mesh->WeightsTexId
				);

				pshadow_depth_shader_controller->AddValue(
					pShader,
					TechId,
					0,
					_VALUESEMANTIC_SPEC::VALSEM_WEIGHTSIZE,
					&mesh->weights_size
				);

				ShadowedMeshList.push(new _shadowed_mesh_item(mesh, pshadow_depth_shader_controller));
			}
		}
	}

	void DBG_DrawBones(cCamera* pCamerac);
};

#endif