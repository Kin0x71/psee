#include "objects.h"

#include <engine/debug/primitives.h>
#include <engine/common/timer.h>
#include <glm/ext/matrix_transform.hpp>
#include <stdio.h>

#include <glm/gtc/quaternion.hpp>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletDynamics/Featherstone/btMultiBodyLinkCollider.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

extern cTimer Timer;

cObjectBase::cObjectBase()
{
	//printf("cObjectBase::cObjectBase()\n");
}

cObjectBase::~cObjectBase()
{
	printf("~cObjectBase()\n");

	if(name)delete[] name;
}

void cObjectBase::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	WorldMx[3][0] = x;
	WorldMx[3][1] = y;
	WorldMx[3][2] = z;
}

void cObjectBase::SetRotation(float angle, const vec3& v)
{
	WorldMx = rotate(WorldMx, angle, v);
}

void cObjectBase::SetScaling(float x, float y, float z)
{
	scaling = vec3(x, y, z);
	WorldMx *= scale(WorldMx, scaling);
}

void cObjectBase::set_name(const char* n)
{
	if(name)delete[] name;

	int nlen = strlen(n);
	name = new char[nlen + 1];
	memcpy(name, n, nlen);
	name[nlen] = 0;
}

cObjectModel::~cObjectModel()
{
	printf("~cObjectModel()\n");

	models_list.reset_enumerator();
	while(cModel* pmodel = models_list.next())
	{
		delete pmodel;
	}
	models_list.free();

	ShadowedMeshList.reset_enumerator();
	while(_shadowed_mesh_item* shadowed_mesh_item = ShadowedMeshList.next())
	{
		delete shadowed_mesh_item->pShadowDepthShaderController;
		delete shadowed_mesh_item;
	}
	ShadowedMeshList.free();

	printf("~cObjectModel() ok\n");
}

void cObjectModel::Update()
{
}

void cObjectModel::AddModel(cModel* model)
{
	models_list.push(model);
}

void cObjectModel::Draw(cCamera* pCamera)
{
	//printf(">>> cObjectModel::Draw(%s)\n",name);

	models_list.reset_enumerator();
	while(cModel* pmodel = models_list.next())
	{
		pmodel->RenderModel(pCamera, &WorldMx);
	}

	//DBG_DrawNormals(pCamera);
}

bool cObjectModel::UpdateFrustumViewBox(cCamera* pCamera)
{
	bool ret = false;

	models_list.reset_enumerator();
	while(cModel* model = models_list.next())
	{
		model->mesh_list.reset_enumerator();
		while(cMesh* mesh = model->mesh_list.next())
		{
			mat4 pos_mx = translate(WorldMx, mesh->view_centre);

			mesh->is_in_frustum = pCamera->IsSphereInFrustum(pos_mx[3][0], pos_mx[3][1], pos_mx[3][2], mesh->view_radius);

			if(!ret && mesh->is_in_frustum)ret = true;

			//DrawSphere(pCamera, vec3(pos_mx[3][0], pos_mx[3][1], pos_mx[3][2]), pmesh->view_radius, 10, 10);
			//DrawCross(pCamera, vec3(pos_mx[3][0], pos_mx[3][1], pos_mx[3][2]), 1.0f, "\xff\x00\x00");
		}
	}

	return ret;
}

void cObjectModel::SetupShadows(shaders_container* pShader, int TechId)
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

			ShadowedMeshList.push(new _shadowed_mesh_item(mesh, pshadow_depth_shader_controller));
		}
	}
}

void cObjectModel::GathereShadowDepth(mat4* plightmx)
{
	Shadows_LightSpaceMatrix = *plightmx;

	ShadowedMeshList.reset_enumerator();
	while(_shadowed_mesh_item* shadowed_mesh_item = ShadowedMeshList.next())
	{
		shadowed_mesh_item->pShadowDepthShaderController->UpdateValues();

		shadowed_mesh_item->pShadowDepthShaderController->shaders_list.reset_enumerator();
		while(MSC::_shader_item* shader_item = shadowed_mesh_item->pShadowDepthShaderController->shaders_list.next())
		{
			shader_item->tech_list.reset_enumerator();
			while(MSC::_tech_item* tech_item = shader_item->tech_list.next())
			{
				shader_item->pshader->Begin(shadowed_mesh_item->Mesh, tech_item->tech_id);
			}
		}
	}
}

void cObjectModel::DBG_DrawNormals(cCamera* pCamera)
{/*
	for(int mi = 0; mi < pmodel->mesh_list.count; ++mi)
	{
		cMesh* pMesh = pmodel->mesh_list[mi];

		uchar* vert_item_offset = (uchar*)pMesh->vert_buff;

		for(uint vert_index = 0; vert_index < pMesh->vnum; ++vert_index)
		{
			//printf("\tvert_index:%d\n", vert_index);
			vec3* p_vpos = 0;
			vec3* p_vnormal = 0;
			vec3* p_vbinormal = 0;
			vec3* p_vtangent = 0;

			for(int pi = 0; pi < pMesh->vert_def.items_count; ++pi)
			{
				switch(pMesh->vert_def.use_items[pi]){
					case VALSEM_VERTEX:
						p_vpos = (vec3*)vert_item_offset;
						break;

					case VALSEM_NORMAL:
						p_vnormal = (vec3*)vert_item_offset;
						break;

					case VALSEM_BINORMAL:
						p_vbinormal = (vec3*)vert_item_offset;
						break;

					case VALSEM_TANGENT:
						p_vtangent = (vec3*)vert_item_offset;
						break;
				}

				vert_item_offset += ShaderSemantics::vert_def[pMesh->vert_def.use_items[pi]].part_size;
			}

			vec3 vp = world_mx * vec4(*p_vpos, 1.0f);
			vec3 vn = mat3(world_mx) * (*p_vnormal);
			//vec3 vp = (vec3)*&(world_mx * (vec4(*p_vpos, 1.0f)))[3];
			DrawLine(
				pCamera,
				vp,
				vp+vn*0.1f,
				"\x00\x00\xff"
			);

			if(p_vbinormal){
				vec3 vb = mat3(world_mx) * (*p_vbinormal);
				DrawLine(
					pCamera,
					vp,
					vp + vb * 0.1f,
					"\x00\xff\x00"
				);
			}

			if(p_vtangent){
				vec3 vt = mat3(world_mx) * (*p_vtangent);
				DrawLine(
					pCamera,
					vp,
					vp + vt * 0.1f,
					"\xff\x00\x00"
				);
			}
		}
	}*/
}

cCollidedObject::~cCollidedObject()
{
	if(RigidBodys){

		for(int i = 0; i < RigidBodysCount; ++i)
		{
			delete RigidBodys[i]->getCollisionShape();
			delete RigidBodys[i]->getMotionState();
			delete RigidBodys[i];
		}

		delete[] RigidBodys;
		delete[] CollideGroups;
		delete[] CollideFilters;
	}

	if(Connections){
		for(int i = 0; i < ConnectionsCount; ++i)
		{
			delete Connections[i];
		}

		delete[] Connections;
	}
}
/*
void cCollidedObject::CollideCallback(const btRigidBody* SelfRigidBody, const btRigidBody* pCollisionRigidBody)
{
	printf("cCollidedObject::CollideCallback()\n");

	const void* self_userptr = SelfRigidBody->getUserPointer();

	if(!self_userptr){
		return;
	}

	const void* collider_userptr = pCollisionRigidBody->getUserPointer();

	if(!collider_userptr){
		return;
	}

	cObjectBase* pSelfObjectBase = (cObjectBase*)self_userptr;

	cObjectBase* pColliderObjectBase = (cObjectBase*)collider_userptr;
}
*/

void cCollidedObject::CreateCollideMesh(vec3* vertices, int vertices_count, word* indeces, int indeces_count)
{
	btTriangleMesh* pTriangleMesh = new btTriangleMesh();

	for(int i = 0; i < indeces_count; i += 3)
	{
		btVector3 vertex_1(vertices[indeces[i]].x, vertices[indeces[i]].y, vertices[indeces[i]].z);
		btVector3 vertex_2(vertices[indeces[i + 1]].x, vertices[indeces[i + 1]].y, vertices[indeces[i + 1]].z);
		btVector3 vertex_3(vertices[indeces[i + 2]].x, vertices[indeces[i + 2]].y, vertices[indeces[i + 2]].z);

		pTriangleMesh->addTriangle(vertex_1, vertex_2, vertex_3);
	}

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(pTriangleMesh, true, true);

	//btBoxShape* shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

	btTransform SphereTransform;
	SphereTransform.setIdentity();

	btVector3 blocalInertia(0.0f, 0.0f, 0.0f);

	btRigidBody* RigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0.0f, new btDefaultMotionState(SphereTransform), shape, blocalInertia));

	RigidBody->setCollisionFlags(RigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	RigidBody->setFriction(1.0f);

	allocate_rigidbodys(1);
	CollideGroups[0] = COLLISION_GROUP_WALLS;
	CollideFilters[0] = COLLISION_GROUP_UNITS;

	is_collided = true;
	RigidBodys[0] = RigidBody;

	RigidBody->setUserPointer(this);
}

void cCollidedObject::SetPosition(float x, float y, float z)
{
	cObjectBase::SetPosition(x, y, z);

	btTransform collision_transform;
	collision_transform.setFromOpenGLMatrix((float*)&WorldMx);

	for(int i = 0; i < RigidBodysCount; ++i)
	{
		RigidBodys[i]->setWorldTransform(collision_transform);
		RigidBodys[i]->activate(true);
	}
}

void cCollidedObject::SetRotation(float angle, const vec3& v)
{
	cObjectBase::SetRotation(angle, v);

	btTransform collision_transform;
	collision_transform.setFromOpenGLMatrix((float*)&WorldMx);

	RigidBodys[0]->setWorldTransform(collision_transform);
	RigidBodys[0]->activate(true);
}

void cCollidedObject::SetScaling(float x, float y, float z)
{
	cObjectBase::SetScaling(x, y, z);

	btTransform collision_transform;
	collision_transform.setFromOpenGLMatrix((float*)&WorldMx);

	for(int i = 0; i < RigidBodysCount; ++i)
	{
		RigidBodys[i]->setWorldTransform(collision_transform);
		RigidBodys[i]->activate(true);
	}
}

vec3 cCollidedObject::GetPosition()
{
	btTransform rb_mx = RigidBodys[0]->getWorldTransform();
	btVector3 rb_pos = rb_mx.getOrigin();

	mat4 oglmx;
	rb_mx.getOpenGLMatrix((float*)&oglmx);

	return vec3(rb_pos.x(), rb_pos.y(), rb_pos.z());
}

void cCollidedObject::UpdateAfterTransforms()
{
	//printf("UpdateAfterTransforms() %f %f %f\n", btCorrectVelocity.x(), btCorrectVelocity.y(), btCorrectVelocity.z());
	//printf("get correct (%f %f) (%s)\n", btCorrectVelocity.x(), btCorrectVelocity.z(), name);

	btVector3 root_velocity = RigidBodys[0]->getLinearVelocity();
	RigidBodys[0]->setLinearVelocity(
		btVector3(
			btMovingVelocity.x(),
			btMovingVelocity.y() + root_velocity.y(),
			btMovingVelocity.z()
		)
	);

	//btCorrectVelocity = btVector3(0.0f, 0.0f, 0.0f);

	//root_velocity = RigidBodys[0]->getLinearVelocity();
	//printf("(%f %f %f) (%f %f)\n", root_velocity.x(), root_velocity.y(), root_velocity.z(), btCorrectVelocity.x(), btCorrectVelocity.z());

	/*btTransform collision_transform = RigidBodys[0]->getWorldTransform();
	collision_transform.getOpenGLMatrix((float*)&WorldMx);

	for(int i = 1; i < RigidBodysCount; ++i)
	{
		//RigidBodys[i]->activate(true);
		RigidBodys[i]->setWorldTransform(collision_transform);
	}*/
}

cSkeletonObject::~cSkeletonObject()
{
	printf("~cSkeletonObject()\n");
	if(pSkeleton){
		delete pSkeleton;
	}

	/*if(pAnimation){
		delete pAnimation;
	}*/
}

void cSkeletonObject::release()
{
	if(pSkeleton){
		delete pSkeleton;
	}
}

void cSkeletonObject::InitCollision()
{
	//printf("cCharacter::InitCollision()\n");

	rigidbodys_offset = vec3(0.0f, -1.0f, 0.0f);

	allocate_rigidbodys(2);

	CollideGroups[0] = COLLISION_GROUP_UNITS;
	CollideFilters[0] = COLLISION_GROUP_LANDSCAPE;

	CollideGroups[1] = COLLISION_GROUP_UNITS;
	CollideFilters[1] = (COLLISION_GROUPS)(COLLISION_GROUP_UNITS | COLLISION_GROUP_WALLS);

	float sphere_radius = 1.0f;

	btSphereShape* sphere_shape = new btSphereShape(sphere_radius);

	btVector3 sphere_blocal_inertia(0.0f, 0.0f, 0.0f);
	sphere_shape->calculateLocalInertia(0.0f, sphere_blocal_inertia);

	btTransform sphere_sphere_transform;
	sphere_sphere_transform.setIdentity();
	//sphere_sphere_transform.setOrigin(btVector3(position.x, position.y, position.z));
	sphere_sphere_transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	btDefaultMotionState* sphere_motion_state = new btDefaultMotionState(sphere_sphere_transform);

	btRigidBody::btRigidBodyConstructionInfo sphere_rb_info(1.0f, sphere_motion_state, sphere_shape, sphere_blocal_inertia);

	RigidBodys[0] = new btRigidBody(sphere_rb_info);

	RigidBodys[0]->setUserPointer(this);

	//RigidBody->setCollisionFlags(RigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	RigidBodys[0]->setSleepingThresholds(0.0f, 0.0f);
	RigidBodys[0]->setAngularFactor(0.0f);
	RigidBodys[0]->setFriction(0.7f);
	RigidBodys[0]->setRestitution(0.0f);
	RigidBodys[0]->setActivationState(DISABLE_DEACTIVATION);

	//-------------------------------------------------------

	float capsule_height = 1.5f;
	float capsule_radius = 0.3f;
	//object_offset = vec3(0.0f, -1.05f, 0.0f);

	btCapsuleShape* capsule_shape = new btCapsuleShape(capsule_radius, capsule_height);
	//btSphereShape* CollisionShape = new btSphereShape(radius);

	btVector3 capsule_blocal_inertia(0.0f, 0.0f, 0.0f);
	capsule_shape->calculateLocalInertia(0.0f, capsule_blocal_inertia);

	btQuaternion capsule_rotation;
	capsule_rotation.setEulerZYX(0.0f, 0.0f, 0.0f);

	btTransform capsule_sphere_transform;
	capsule_sphere_transform.setIdentity();
	capsule_sphere_transform.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
	capsule_sphere_transform.setRotation(capsule_rotation);
	btDefaultMotionState* capsule_motion_state = new btDefaultMotionState(capsule_sphere_transform);

	btRigidBody::btRigidBodyConstructionInfo capsule_rb_info(0.5f, capsule_motion_state, capsule_shape, capsule_blocal_inertia);

	RigidBodys[1] = new btRigidBody(capsule_rb_info);

	RigidBodys[1]->setUserPointer(this);

	//RigidBodys[1]->setCollisionFlags(RigidBodys[1]->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_STATIC_OBJECT);
	RigidBodys[1]->setSleepingThresholds(0.0f, 0.0f);
	RigidBodys[1]->setAngularFactor(0.0f);
	RigidBodys[1]->setFriction(0.0f);
	RigidBodys[1]->setRestitution(0.0f);
	RigidBodys[1]->setActivationState(DISABLE_DEACTIVATION);

	//-------------------------------------------------------

	allocate_connections(1);

	btTransform localA, localB;
	localA.setIdentity();
	localB.setIdentity();
	localA.setOrigin(btVector3(0.0f, -0.01f, 0.0f));
	//localA.setOrigin(btVector3(0.0f, -2.01f, -2.0f));

	btQuaternion local_a_rotation;
	local_a_rotation.setEulerZYX(0.0f, 1.0f, 0.0f);
	localA.setRotation(local_a_rotation);

	btGeneric6DofConstraint* connection = new btGeneric6DofConstraint(*RigidBodys[0], *RigidBodys[1], localA, localB, false);

	connection->setLinearLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
	connection->setLinearUpperLimit(btVector3(0.0f, 0.0f, 0.0f));
	connection->setAngularLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
	//connection->setAngularUpperLimit(btVector3(0.0f, 0.0f, 0.0f));

	Connections[0] = connection;

	is_collided = true;
}

void cSkeletonObject::_init_trans_animation(cAnimation* pTransAnimation)
{
	pTransAnimation->PlayType = _ANIM_PLAY_TYPE::ONCE;

	pTransAnimation->BonesFrames = new _bone_frame[pTransAnimation->BonesFramesCount];

	float trans_duration = 1.0f;
	pTransAnimation->TimeEnd = trans_duration;

	for(int bi = 0; bi < pTransAnimation->BonesFramesCount; ++bi)
	{
		pTransAnimation->BonesFrames[bi].KeysRNum = 2;
		pTransAnimation->BonesFrames[bi].KeysXNum = 2;
		pTransAnimation->BonesFrames[bi].KeysYNum = 2;
		pTransAnimation->BonesFrames[bi].KeysZNum = 2;
		pTransAnimation->BonesFrames[bi].KeysSNum = 2;

		pTransAnimation->BonesFrames[bi].KeysR = new cKeyR[2];
		pTransAnimation->BonesFrames[bi].KeysX = new cKeyP[2];
		pTransAnimation->BonesFrames[bi].KeysY = new cKeyP[2];
		pTransAnimation->BonesFrames[bi].KeysZ = new cKeyP[2];
		pTransAnimation->BonesFrames[bi].KeysS = new cKeyS[2];

		pTransAnimation->BonesFrames[bi].KeysR[0].time = 0.0f;
		pTransAnimation->BonesFrames[bi].KeysR[1].time = trans_duration;
		pTransAnimation->BonesFrames[bi].KeysX[0].time = 0.0f;
		pTransAnimation->BonesFrames[bi].KeysX[1].time = trans_duration;
		pTransAnimation->BonesFrames[bi].KeysY[0].time = 0.0f;
		pTransAnimation->BonesFrames[bi].KeysY[1].time = trans_duration;
		pTransAnimation->BonesFrames[bi].KeysZ[0].time = 0.0f;
		pTransAnimation->BonesFrames[bi].KeysZ[1].time = trans_duration;
		pTransAnimation->BonesFrames[bi].KeysS[0].time = 0.0f;
		pTransAnimation->BonesFrames[bi].KeysS[1].time = trans_duration;
	}
}

void cSkeletonObject::_setup_trans_animation(cAnimation* TransAnimation, cAnimation* AnimFrom, cAnimation* AnimTo, float trans_duration)
{
	if(TransAnimation->BonesFramesCount != AnimFrom->BonesFramesCount || TransAnimation->BonesFramesCount != AnimTo->BonesFramesCount)
	{
		throw exception_box(__FUNCTION__, __FILE__, __LINE__, "count key trand !=");
	}

	for(int bi = 0; bi < TransAnimation->BonesFramesCount; ++bi)
	{
		if(AnimFrom->BonesFrames[bi].KeysRNum > 1 && AnimTo->BonesFrames[bi].KeysRNum > 0)
		{
			TransAnimation->BonesFrames[bi].KeysR[0].v = AnimFrom->BonesFrames[bi].KeyState.rotation;
			TransAnimation->BonesFrames[bi].KeysR[1].v = AnimTo->BonesFrames[bi].KeysR[0].v;
			TransAnimation->BonesFrames[bi].KeysR[1].time = trans_duration;

			//printf("_setup_trans_animation (%f %f %f %f)\n", TransAnimation->BonesFrames[bi].KeysR[0].v.x, TransAnimation->BonesFrames[bi].KeysR[0].v.y, TransAnimation->BonesFrames[bi].KeysR[0].v.z, TransAnimation->BonesFrames[bi].KeysR[0].v.w);
		}
		else{
			TransAnimation->BonesFrames[bi].KeysRNum = 0;
		}

		if(AnimFrom->BonesFrames[bi].KeysXNum > 1 && AnimTo->BonesFrames[bi].KeysXNum > 0)
		{
			TransAnimation->BonesFrames[bi].KeysX[0].v = AnimFrom->BonesFrames[bi].KeyState.position.x;
			TransAnimation->BonesFrames[bi].KeysX[1].v = AnimTo->BonesFrames[bi].KeysX[0].v;
			TransAnimation->BonesFrames[bi].KeysX[1].time = trans_duration;
		}
		else{
			TransAnimation->BonesFrames[bi].KeysXNum = 0;
		}

		if(AnimFrom->BonesFrames[bi].KeysYNum > 1 && AnimTo->BonesFrames[bi].KeysYNum > 0)
		{
			TransAnimation->BonesFrames[bi].KeysY[0].v = AnimFrom->BonesFrames[bi].KeyState.position.y;
			TransAnimation->BonesFrames[bi].KeysY[1].v = AnimTo->BonesFrames[bi].KeysY[0].v;
			TransAnimation->BonesFrames[bi].KeysY[1].time = trans_duration;
		}
		else{
			TransAnimation->BonesFrames[bi].KeysYNum = 0;
		}

		if(AnimFrom->BonesFrames[bi].KeysZNum > 1 && AnimTo->BonesFrames[bi].KeysZNum > 0)
		{
			TransAnimation->BonesFrames[bi].KeysZ[0].v = AnimFrom->BonesFrames[bi].KeyState.position.z;
			TransAnimation->BonesFrames[bi].KeysZ[1].v = AnimTo->BonesFrames[bi].KeysZ[0].v;
			TransAnimation->BonesFrames[bi].KeysZ[1].time = trans_duration;
		}
		else{
			TransAnimation->BonesFrames[bi].KeysZNum = 0;
		}

		if(AnimFrom->BonesFrames[bi].KeysSNum > 1 && AnimTo->BonesFrames[bi].KeysSNum > 0)
		{
			TransAnimation->BonesFrames[bi].KeysS[0].v = AnimFrom->BonesFrames[bi].KeyState.scale;
			TransAnimation->BonesFrames[bi].KeysS[1].v = AnimTo->BonesFrames[bi].KeysS[0].v;
			TransAnimation->BonesFrames[bi].KeysS[1].time = trans_duration;
		}
		else{
			TransAnimation->BonesFrames[bi].KeysSNum = 0;
		}
	}

	TransAnimation->TimeEnd = trans_duration;
}

void cSkeletonObject::Update()
{

}

void cSkeletonObject::UpdateAfterTransforms()
{
	cCollidedObject::UpdateAfterTransforms();

	btTransform rb0_transform = RigidBodys[0]->getWorldTransform();

	//RigidBodys[1]->setWorldTransform(rb0_transform);

	rb0_transform.getOpenGLMatrix((float*)&WorldMx);

	WorldMx[3][0] += rigidbodys_offset.x;
	WorldMx[3][1] += rigidbodys_offset.y;
	WorldMx[3][2] += rigidbodys_offset.z;

	models_list.reset_enumerator();
	while(cModel* model = models_list.next())
	{
		model->ModelWorldMx = WorldMx;
	}

	position.x = WorldMx[3][0];
	position.y = WorldMx[3][1];
	position.z = WorldMx[3][2];
}
/*
void cSkeletonObject::Draw(cCamera* pCamera)
{
	//models_list.reset_enumerator();
	//while(cModel* model = models_list.next())
	//{
	//	model->RenderModel(pCamera, &world_mx);
	//}

	//printf("\tcSkeletonObject::Draw(%s)\n", name);
	mat4 ProjViewWorldMx = pCamera->ProjViewMx * world_mx;

	glEnable(GL_TEXTURE_2D);

	pmodel->shader_controller.links_list.reset_enumerator();
	while(MSC::_link_item* link_item = pmodel->shader_controller.links_list.next())
	{
		link_item->model_values.reset_enumerator();
		while(c_shader_value* value = link_item->model_values.next())
		{
			switch(value->Semantic)
			{
				case VALSEM_PROJVIEWWORLDMX:
				{
					link_item->pShader->SetMatrix(value->GId, &ProjViewWorldMx);
				}
				break;
				case VALSEM_WORLDMX:
				{
					link_item->pShader->SetMatrix(value->GId, &world_mx);
				}
				break;
				case VALSEM_BONES:
				{
					if(pSkeleton){
						//printf("!!!!!!!!!!!!!!!!! %f %f %f\n", pSkeleton->pMxNodesResult[1][1][1], pSkeleton->pMxNodesResult[1][2][2], pSkeleton->pMxNodesResult[1][3][3]);
						link_item->pShader->SetMatrixArray(value->GId, pSkeleton->pMxNodesResult, pSkeleton->BonesCount);
					}
				}
				break;
				default:
					printf("switch default: (%s) model_values:%d %s cSkeletonObject::Draw\n", pmodel->Name, value->Semantic, value->Name);
			}
		}

		link_item->mesh_values.reset_enumerator();
		while(c_shader_value* value = link_item->mesh_values.next())
		{
			switch(value->Semantic)
			{
				case VALSEM_SAMPLER:
				{
					if(link_item->pMesh->pTextureD && link_item->pMesh->pTextureD->textureID){
						link_item->pShader->SetSampler(value->GId, link_item->pMesh->pTextureD->textureID);
					}
					else{
						//printf("set empty texture %s %x\n", link_item->pMesh->TextureDName, link_item->pMesh->pTextureD? link_item->pMesh->pTextureD->textureID:-1);
						link_item->pShader->SetSampler(value->GId, 0);
					}
				}
				break;
				case VALSEM_WEIGHTTEX:
				{
					if(link_item->pMesh->pWeightsTex && link_item->pMesh->pWeightsTex->textureID){
						link_item->pShader->SetSampler(value->GId, link_item->pMesh->pWeightsTex->textureID);
					}
					else{
						link_item->pShader->SetSampler(value->GId, 0);
					}
				}
				break;
				case VALSEM_WEIGHTSIZE:
				{
					link_item->pShader->SetFloat(value->GId, (float)link_item->pMesh->weights_size);
				}
				break;
				default:
					printf("switch default: mesh_values:%d %s cSkeletonObject::Draw\n", value->Semantic, value->Name);
			}
		}

		//printf("cObjectModel::Draw: link_item->pShader:%s\n", link_item->pShader->Name);
		link_item->pShader->Begin(link_item->pMesh, link_item->TechId);
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(dbg_draw_bones)DBG_DrawBones(pCamera);
}
*/
template <typename T>
void _get_next_time_scalar(float cur_time, T* keys, int keys_count, int* pret_timeid, float* pret_scalar)
{
	int NextId = 0;

	for(int ti = 0; ti < keys_count; ++ti)
	{
		if(keys[ti].time >= cur_time)
		{
			NextId = ti;
			break;
		}
	}

	*pret_timeid = NextId;
	*pret_scalar = ((cur_time - keys[NextId - 1].time) / (keys[NextId].time - keys[NextId - 1].time));
}

void cSkeletonObject::DBG_DrawBones(cCamera* pCamera)
{
	class Updater{
	public:
		cCamera* pCamera = 0;
		cBone* bones = 0;
		int bones_count = 0;
		mat4* result_buff = 0;

		~Updater()
		{
			if(result_buff){
				delete[] result_buff;
				result_buff = 0;
			}
		}

		void calculate_animations(cBone* bone, mat4& ParentMatrix)
		{
			int id = bone->Id;

			quat qr = bones[id].KeyState.rotation;
			vec3 vp = bones[id].KeyState.position;
			vec3 vs = bones[id].KeyState.scale;

			mat4 mx_anim =
				translate(mat4(1.0f), vp) *
				mat4_cast(qr) *
				scale(mat4(1.0f), vs);

			result_buff[id] = ParentMatrix * mx_anim;

			if(bone->pSibling)calculate_animations(bone->pSibling, ParentMatrix);
			if(bone->pChildren)calculate_animations(bone->pChildren, result_buff[id]);
		}

		void display()
		{
			for(int i = 0; i < bones_count; ++i)
			{
				int id = bones[i].Id;
				int parent_id = bones[i].ParentId;

				DrawCross(result_buff[id][3][0], result_buff[id][3][1], result_buff[id][3][2], 0.1f, "\x00\x00\xff");

				if(parent_id != -1)
				{
					DrawLine(
						result_buff[parent_id][3][0],
						result_buff[parent_id][3][1],
						result_buff[parent_id][3][2],
						result_buff[id][3][0],
						result_buff[id][3][1],
						result_buff[id][3][2],
						"\x00\x00\xff"
					);
				}
			}
		}
	};

	mat4 worldmx;

	if(RigidBodys){
		btTransform rb_mx = RigidBodys[0]->getWorldTransform();
		rb_mx.getOpenGLMatrix((float*)&worldmx);
	}else{
		worldmx = WorldMx;
	}

	Updater updater;

	updater.pCamera = pCamera;
	updater.bones_count = pSkeleton->BonesCount;
	updater.bones = pSkeleton->Bones;
	updater.result_buff = new mat4[pSkeleton->BonesCount];

	updater.calculate_animations(&pSkeleton->Bones[0], worldmx);
	updater.display();
}
