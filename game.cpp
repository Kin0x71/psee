#include "game.h"
#include <engine/engine.h>
#include <engine/common/screen.h>
#include <engine/files/file.h>
#include <engine/files/mesh_file.h>
#include <engine/unit/unit.h>
#include <engine/unit/character.h>
#include <engine/ui/text_object.h>
//#include <engine/events/events.h>

#include <engine/map/map_builder.h>
#include <engine/debug/primitives.h>

#include <glm/gtx/compatibility.hpp>//vec3 lerp

extern cCamera* DBG_MainCamera;
extern cInput Input;

cCharacter* pPlayer = 0;
cUnit* pUnit = 0;
cTextObject* DebugText = 0;

bool AlwaisRunFlag = false;

void CreateSkyBox(cObjectModel* object)
{
	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/skybox.vss", GL_REPEAT, GL_LINEAR);//GL_NEAREST
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	object->set_name("SkyBox");

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_sky_box"));

		if(mesh->DefuseTextureId != -1){
			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_sky_box"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);
		}
	}

	pModel->SetupShaderValues(true, false, false);

	object->AddModel(pModel);
	object->SetScaling(512.0f, 512.0f, 512.0f);
}

cHouseObject* CreateHouse()
{
	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/house.vss");
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	cHouseObject* pObject = new cHouseObject();
	pObject->set_name("House");

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"));

		if(mesh->DefuseTextureId != -1){

			mesh->is_cast_shadow = true;

			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);
		}
	}

	pModel->SetupShaderValues();

	pObject->AddModel(pModel);

	pObject->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_shadows_depth"));

	pObject->InitCollision();

	return pObject;
}

cSkeletonObject* CreateWeponObject()
{
	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/wow/rifle.vss");
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	cSkeletonObject* pSkeletonObject = new cSkeletonObject();
	pSkeletonObject->set_name("Wepon");

	pSkeletonObject->pSkeleton = LoadSkeletonKbh((char*)"data/models/wow/rifle.kbh");

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"));

		if(mesh->DefuseTextureId != -1){

			mesh->is_cast_shadow = true;

			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);
		}

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_BONES,
			pSkeletonObject->pSkeleton->pMxNodesResult,
			&pSkeletonObject->pSkeleton->BonesCount
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTTEX,
			&mesh->WeightsTexId
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTSIZE,
			&mesh->weights_size
		);
	}

	pModel->SetupShaderValues();

	pSkeletonObject->AddModel(pModel);

	return pSkeletonObject;
}

cCollidedObject* CreateStatue()
{
	cCollidedObject* object = new cCollidedObject();
	object->set_name("Lenin");

	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/lenin.vss", GL_REPEAT, GL_LINEAR);
	cModel* pModel = model_file_container->model;

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"));

		mesh->is_cast_shadow = true;

		if(mesh->DefuseTextureId != -1){
			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);

			/*if(mesh->NormalmapTextureId != -1){
				mesh->shader_controller.AddValue(
					pMainShadersContainer,
					pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_normalmap"),
					0,
					_VALUESEMANTIC_SPEC::VALSEM_NORMALMAP,
					(void*)pModel->ppdefuse_textures[mesh->NormalmapTextureId]->textureID,
					1
				);
			}*/
		}
	}

	pModel->SetupShaderValues();

	object->AddModel(pModel);

	object->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_shadows_depth"));

	object->CreateCollideMesh(model_file_container->collide_mesh->vertices, model_file_container->collide_mesh->vertices_count, model_file_container->collide_mesh->indexes, model_file_container->collide_mesh->indexes_count);

	delete model_file_container;

	return object;
}

cCollidedObject* CreateStructure()
{
	cCollidedObject* object = new cCollidedObject();
	object->set_name("Tavern");

	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/structures/tavern.vss", GL_REPEAT, GL_LINEAR);
	cModel* pModel = model_file_container->model;

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"));

		mesh->is_cast_shadow = true;

		if(mesh->DefuseTextureId != -1){
			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);

			/*if(mesh->NormalmapTextureId != -1){
				mesh->shader_controller.AddValue(
					pMainShadersContainer,
					pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_normalmap"),
					0,
					_VALUESEMANTIC_SPEC::VALSEM_NORMALMAP,
					(void*)pModel->ppdefuse_textures[mesh->NormalmapTextureId]->textureID,
					1
				);
			}*/
		}
	}

	pModel->SetupShaderValues();

	object->AddModel(pModel);

	object->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_shadows_depth"));

	object->CreateCollideMesh(model_file_container->collide_mesh->vertices, model_file_container->collide_mesh->vertices_count, model_file_container->collide_mesh->indexes, model_file_container->collide_mesh->indexes_count);

	delete model_file_container;

	return object;
}

cObjectModel* CreateObject(const char* fname)
{
	cObjectModel* object = new cObjectModel();
	object->set_name(fname);

	_model_file_container* model_file_container = LoadModelVss((char*)fname, GL_REPEAT, GL_LINEAR);
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"));

		mesh->is_cast_shadow = true;

		if(mesh->DefuseTextureId != -1){
			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);

			/*if(mesh->NormalmapTextureId != -1){
				mesh->shader_controller.AddValue(
					pMainShadersContainer,
					pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_normalmap"),
					0,
					_VALUESEMANTIC_SPEC::VALSEM_NORMALMAP,
					(void*)pModel->ppdefuse_textures[mesh->NormalmapTextureId]->textureID,
					1
				);
			}*/
		}
	}

	pModel->SetupShaderValues();

	object->AddModel(pModel);

	object->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex_shadows_depth"));

	return object;
}

cCharacter* CreateCharacter()
{
	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/wow/bloodelf_female/body.vss");
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	//pModel->shader_controller.ShaderSetupAllValues();

	cCharacter* pObjectCharacter = new cCharacter();
	pObjectCharacter->set_name("Character");

	pObjectCharacter->pSkeleton = LoadSkeletonKbh((char*)"data/models/wow/bloodelf_female/body.kbh");

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"));

		if(mesh->DefuseTextureId != -1){
			mesh->is_cast_shadow = true;

			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);
		}

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_BONES,
			pObjectCharacter->pSkeleton->pMxNodesResult,
			&pObjectCharacter->pSkeleton->BonesCount
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTTEX,
			&mesh->WeightsTexId
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTSIZE,
			&mesh->weights_size
		);
	}

	pModel->SetupShaderValues();

	pObjectCharacter->AddModel(pModel);

	pObjectCharacter->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt_shadows_depth"));

	cList<char*> parts_a_bones;
	cList<char*> parts_b_bones;
	data_files::_FILE* pf_parts_a = data_files::files::pFilesObject->GetFile((char*)"data/models/wow/bloodelf_female/part_torso.txt");
	data_files::_FILE* pf_parts_b = data_files::files::pFilesObject->GetFile((char*)"data/models/wow/bloodelf_female/part_legs.txt");

	while(true)
	{
		char str_tmp[1024];
		int len = FileReadLine(str_tmp, pf_parts_a);
		if(len == 0)break;

		char* name = new char[len + 1];
		memcpy(name, str_tmp, len);
		name[len] = 0;

		parts_a_bones.push(name);
	}
	FileClose(pf_parts_a);

	while(true)
	{
		char str_tmp[1024];
		int len = FileReadLine(str_tmp, pf_parts_b);
		if(len == 0)break;

		char* name = new char[len + 1];
		memcpy(name, str_tmp, len);
		name[len] = 0;

		parts_b_bones.push(name);
	}
	FileClose(pf_parts_b);

	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::TRANS] = new cAnimation();
	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::LEGS][cCharacter::ANIMATION_ID::TRANS] = new cAnimation();

	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::TRANS]->set_name((char*)"trans_torso");
	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::LEGS][cCharacter::ANIMATION_ID::TRANS]->set_name((char*)"trans_legs");

	for(int i = 0; i < parts_a_bones.count; ++i)
	{
		int bid = pObjectCharacter->pSkeleton->GetBoneId(parts_a_bones[i]);

		if(bid != -1)
		{
			//printf("push torso %d [%d] %s\n", i, bid, parts_a_bones[i]);
			pObjectCharacter->PartBoneIds[cCharacter::BODY_PART::TORSO].push(bid);
		}
		else{
			printf("ERROR bone \"%s\" not found\n", parts_a_bones[i]);
		}

		delete[] parts_a_bones[i];
	}

	for(int i = 0; i < parts_b_bones.count; ++i)
	{
		int bid = pObjectCharacter->pSkeleton->GetBoneId(parts_b_bones[i]);

		if(bid != -1)
		{
			//printf("push legs %d [%d] %s\n", i, bid, parts_b_bones[i]);
			pObjectCharacter->PartBoneIds[cCharacter::BODY_PART::LEGS].push(bid);
		}
		else{
			printf("ERROR bone \"%s\" not found\n", parts_b_bones[i]);
		}

		delete[] parts_b_bones[i];
	}

	if(pObjectCharacter->PartBoneIds[cCharacter::BODY_PART::TORSO].count + pObjectCharacter->PartBoneIds[cCharacter::BODY_PART::LEGS].count != pObjectCharacter->pSkeleton->BonesCount)
	{
		printf("ERROR bone parts sum != bones_count\n");
	}

	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::TRANS]->BonesFramesCount = pObjectCharacter->PartBoneIds[cCharacter::BODY_PART::TORSO].count;
	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::LEGS][cCharacter::ANIMATION_ID::TRANS]->BonesFramesCount = pObjectCharacter->PartBoneIds[cCharacter::BODY_PART::LEGS].count;

	pObjectCharacter->_init_trans_animation(pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::TRANS]);
	pObjectCharacter->_init_trans_animation(pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::LEGS][cCharacter::ANIMATION_ID::TRANS]);

	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::IDLE, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/idle_rifle.akf"));
	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::RUN, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/run.akf"));
	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::WALK, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/walk.akf"));
	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::BACKWARD, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/backward.akf"));
	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::RUN, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/run.akf"));
	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::JUMP, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/jump_start.akf"));
	pObjectCharacter->AddAnimation(cCharacter::ANIMATION_ID::ATTACK, LoadAnimationAkf((char*)"data/models/wow/bloodelf_female/attack_rifle.akf"));

	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::JUMP]->PlayType = _ANIM_PLAY_TYPE::ONCE;
	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::LEGS][cCharacter::ANIMATION_ID::JUMP]->PlayType = _ANIM_PLAY_TYPE::ONCE;

	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::ATTACK]->PlayType = _ANIM_PLAY_TYPE::ONCE;
	pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::LEGS][cCharacter::ANIMATION_ID::ATTACK]->PlayType = _ANIM_PLAY_TYPE::ONCE;

	//pObjectCharacter->AnimTabParts[cCharacter::BODY_PART::TORSO][cCharacter::ANIMATION_ID::ATTACK]->TimeSpeedFactor = 1.0f;

	pObjectCharacter->SetAnimAction(cCharacter::ANIMATION_ID::IDLE, cCharacter::BODY_PART::TORSO);
	pObjectCharacter->SetAnimAction(cCharacter::ANIMATION_ID::IDLE, cCharacter::BODY_PART::LEGS);

	pObjectCharacter->bones_transforms = new mat4[pObjectCharacter->pSkeleton->BonesCount];
	pObjectCharacter->attach_parts_bones[cCharacter::BODY_PART::TORSO] = pObjectCharacter->pSkeleton->GetBoneId("BloodElfFemale_Bone04");
	pObjectCharacter->attach_parts_bones[cCharacter::BODY_PART::LEGS] = pObjectCharacter->pSkeleton->GetBoneId("BloodElfFemale_Bone02_SpineLow");

	//printf("TimeEnd:%f\n", pObjectCharacter->pAnimation->TimeEnd);
	//pObjectCharacter->pAnimation->TimeEnd = 20.8f;

	pObjectCharacter->InitCollision();

	return pObjectCharacter;
}

cUnit* CreateUnit()
{
	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/wow/zombie/zombie.vss");
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	cUnit* pObjectcUnit = new cUnit();
	pObjectcUnit->set_name("ZombieUnit");

	pObjectcUnit->pSkeleton = LoadSkeletonKbh((char*)"data/models/wow/zombie/zombie.kbh");

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"));

		if(mesh->DefuseTextureId != -1){

			mesh->is_cast_shadow = true;

			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);
		}

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_BONES,
			pObjectcUnit->pSkeleton->pMxNodesResult,
			&pObjectcUnit->pSkeleton->BonesCount
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTTEX,
			&mesh->WeightsTexId
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTSIZE,
			&mesh->weights_size
		);
	}

	pModel->SetupShaderValues();

	pObjectcUnit->AddModel(pModel);

	pObjectcUnit->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt_shadows_depth"));

	pObjectcUnit->AnimTab[UNIT_ANIMATION_ID::TRANS] = new cAnimation();
	pObjectcUnit->AnimTab[UNIT_ANIMATION_ID::TRANS]->BonesFramesCount = pObjectcUnit->pSkeleton->BonesCount;

	pObjectcUnit->AddAnimation(UNIT_ANIMATION_ID::IDLE, LoadAnimationAkf((char*)"data/models/wow/zombie/idle_0.akf"));
	pObjectcUnit->AddAnimation(UNIT_ANIMATION_ID::WALK, LoadAnimationAkf((char*)"data/models/wow/zombie/walk.akf"));
	pObjectcUnit->AddAnimation(UNIT_ANIMATION_ID::RUN, LoadAnimationAkf((char*)"data/models/wow/zombie/run.akf"));

	pObjectcUnit->_init_trans_animation(pObjectcUnit->AnimTab[UNIT_ANIMATION_ID::TRANS]);

	pObjectcUnit->SetAnimAction(UNIT_ANIMATION_ID::IDLE);

	pObjectcUnit->InitCollision();

	return pObjectcUnit;
}

cSkeletonObject* CreateMount()
{
	_model_file_container* model_file_container = LoadModelVss((char*)"data/models/wow/gyrocopter.vss");
	cModel* pModel = model_file_container->model;
	delete model_file_container;

	cSkeletonObject* pObjectc = new cSkeletonObject();
	pObjectc->set_name("Gyrocopter");

	pObjectc->pSkeleton = LoadSkeletonKbh((char*)"data/models/wow/gyrocopter.kbh");

	pModel->mesh_list.reset_enumerator();
	while(cMesh* mesh = pModel->mesh_list.next())
	{
		mesh->shader_controller.AddShader(Engine::pMainShadersContainer);
		mesh->shader_controller.AddTech(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"));

		if(mesh->DefuseTextureId != -1){

			mesh->is_cast_shadow = true;

			mesh->shader_controller.AddValue(
				Engine::pMainShadersContainer,
				Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
				0,
				_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
				&pModel->ppdefuse_textures[mesh->DefuseTextureId]->textureID
			);
		}

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_BONES,
			pObjectc->pSkeleton->pMxNodesResult,
			&pObjectc->pSkeleton->BonesCount
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTTEX,
			&mesh->WeightsTexId
		);

		mesh->shader_controller.AddValue(
			Engine::pMainShadersContainer,
			Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt"),
			0,
			_VALUESEMANTIC_SPEC::VALSEM_WEIGHTSIZE,
			&mesh->weights_size
		);
	}

	pModel->SetupShaderValues();

	pObjectc->AddModel(pModel);

	pObjectc->SetupShadows(Engine::pMainShadersContainer, Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_skinnwt_shadows_depth"));

	/*pObjectc->AnimTab[UNIT_ANIMATION_ID::TRANS] = new cAnimation();
	pObjectc->AnimTab[UNIT_ANIMATION_ID::TRANS]->BonesFramesCount = pObjectcUnit->pSkeleton->BonesCount;

	pObjectc->AddAnimation(UNIT_ANIMATION_ID::IDLE, LoadAnimationAkf((char*)"data/models/wow/zombie/idle_0.akf"));
	pObjectc->AddAnimation(UNIT_ANIMATION_ID::WALK, LoadAnimationAkf((char*)"data/models/wow/zombie/walk.akf"));
	pObjectc->AddAnimation(UNIT_ANIMATION_ID::RUN, LoadAnimationAkf((char*)"data/models/wow/zombie/run.akf"));

	pObjectc->_init_trans_animation(pObjectcUnit->AnimTab[UNIT_ANIMATION_ID::TRANS]);

	pObjectc->SetAnimAction(UNIT_ANIMATION_ID::IDLE);*/

	pObjectc->InitCollision();

	return pObjectc;
}

vec3 floor_vec(vec3& v)
{
	return vec3(floor(v.x * 100.0f) / 100.0f, floor(v.y * 100.0f) / 100.0f, floor(v.z * 100.0f) / 100.0f);
}

void UpdateForwardMoving(float dir_offs = 0.0f, bool run = false)
{
	vec3 player_pos = pPlayer->GetPosition();
	vec3 cam_pos = Engine::pCurrentScene->pMainCamera->eye;
	vec3 player_dir = pPlayer->GetDirection();

	vec3 look_dir = normalize(player_pos - vec3(cam_pos.x, player_pos.y, cam_pos.z));

	if(dir_offs != 0.0f)
	{
		quat q_look_dir = angleAxis(atan(look_dir.x, look_dir.z) + dir_offs, vec3(0.0f, 1.0f, 0.0f));
		look_dir = normalize(vec3(mat4_cast(q_look_dir)[2]));
	}

	//bool update_moving = false;

	//printf("ld:(%f %f %f)==pd:(%f %f %f)\n", floor_vec(look_dir).x, floor_vec(look_dir).y, floor_vec(look_dir).z, floor_vec(player_dir).x, floor_vec(player_dir).y, floor_vec(player_dir).z);
	if(floor_vec(look_dir) != floor_vec(player_dir))
	{
		//printf("LookAt\n");
		pPlayer->LookAt(player_pos + look_dir);
		//update_moving = true;
	}

	if((run && !pPlayer->MovingRunFlag) || (pPlayer->MovingRunFlag && !run) || !pPlayer->MovingForwardFlag){
		//update_moving = true;
		pPlayer->MoveForward(run);
	}

	/*if(update_moving)
	{
		Player->MoveForward(run);
	}*/
}

void UpdateBackwardMoving(float dir_offs = 0.0f)
{
	vec3 player_pos = pPlayer->GetPosition();
	vec3 cam_pos = Engine::pCurrentScene->pMainCamera->eye;

	vec3 look_dir = normalize(player_pos - vec3(cam_pos.x, player_pos.y, cam_pos.z));

	if(dir_offs != 0.0f)
	{
		quat q_look_dir = angleAxis(atan(look_dir.x, look_dir.z) + dir_offs, vec3(0.0f, 1.0f, 0.0f));
		look_dir = normalize(vec3(mat4_cast(q_look_dir)[2]));
	}

	vec3 player_dir = pPlayer->GetDirection();

	if(floor_vec(look_dir) != floor_vec(player_dir))
	{
		pPlayer->LookAt(player_pos + look_dir);

		if(pPlayer->MovingBackwardFlag)
		{
			pPlayer->MoveBackward();
		}
	}

	if(!pPlayer->MovingBackwardFlag)
	{
		pPlayer->MoveBackward();
	}
}

void UpdateInput()
{
	if(!pPlayer->JumpingFlag)
	{
		if(Input.KeyBuffer[_KEY_W].pressed && Input.KeyBuffer[_KEY_A].pressed)
		{//W+A
			UpdateForwardMoving(pi<float>() / 4.0f, (bool)HIBYTE(GetKeyState(0x10)));
		}
		else if(Input.KeyBuffer[_KEY_W].pressed && Input.KeyBuffer[_KEY_D].pressed)
		{//W+D
			UpdateForwardMoving(-pi<float>() / 4.0f, (bool)HIBYTE(GetKeyState(0x10)));
		}
		else if(Input.KeyBuffer[_KEY_S].pressed && Input.KeyBuffer[_KEY_A].pressed)
		{//S+A
			UpdateBackwardMoving(-pi<float>() / 4.0f);
		}
		else if(Input.KeyBuffer[_KEY_S].pressed && Input.KeyBuffer[_KEY_D].pressed)
		{//S+D
			UpdateBackwardMoving(pi<float>() / 4.0f);
		}
		else if(Input.KeyBuffer[_KEY_W].pressed || AlwaisRunFlag)
		{//W
			UpdateForwardMoving(0.0f, AlwaisRunFlag || (bool)HIBYTE(GetKeyState(0x10)));
		}
		else if(Input.KeyBuffer[_KEY_S].pressed)
		{//S
			UpdateBackwardMoving();
		}
		else if(Input.KeyBuffer[_KEY_A].pressed)
		{//A
			UpdateForwardMoving(pi<float>() / 2.0f, (bool)HIBYTE(GetKeyState(0x10)));
		}
		else if(Input.KeyBuffer[_KEY_D].pressed)
		{//D
			UpdateForwardMoving(-pi<float>() / 2.0f, (bool)HIBYTE(GetKeyState(0x10)));
		}
		else
		{
			if(pPlayer->MovingForwardFlag || pPlayer->MovingBackwardFlag)
			{
				pPlayer->MoveStop();
			}
		}

		if(Input.KeyBuffer[_KEY_SPACE].pressed){
			pPlayer->Jump();
		}
	}//!Player->JumpingFlag
}

void GameKeyEvent(const cInput::EventKeyArg& arg)
{
	if(arg.kode == _KEY_KODE_::_KEY_F1 && arg.stat == 1){
		Engine::DBGShowCollisionsWorldFlag = !Engine::DBGShowCollisionsWorldFlag;
		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_F2 && arg.stat == 1){
		//vec3 pos=pUnit->GetPosition();
		//pUnit->MoveToPos(pos + vec3(0.1f, 0.0f, 0.0f));
		pUnit->SetBihavior(cBihavior_AggressiveToTarget::_start_param(pPlayer));
		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_F3 && arg.stat == 1){
		pUnit->LookAt(pPlayer->position);
		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_F4 && arg.stat == 1){

		//btTransform rb_mx = pUnit->RigidBodys[0]->getWorldTransform();
		//btVector3 rb_pos = rb_mx.getOrigin();

		//pUnit->RotateLookAt(vec3(pPlayer->position.x, rb_pos.y(), pPlayer->position.z), 1.1f);
		vec3 pos = pUnit->GetPosition();
		pUnit->MoveToDirection(normalize(vec3(pPlayer->position.x, pos.y, pPlayer->position.z) - pos), false);
		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_F5 && arg.stat == 1){

		Engine::pCurrentScene->EnableGravity(!Engine::pCurrentScene->GravityEnabled);
		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_Q && arg.stat == 1){

		AlwaisRunFlag = !AlwaisRunFlag;
		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_F6 && arg.stat == 1){

		cMap::_view_zone_item*** ViewZones = Engine::pCurrentScene->SceneMap->ViewZones;

		for(int ri = 0; ri < Engine::pCurrentScene->SceneMap->Navigation.MapRows; ++ri)
		{
			for(int ci = 0; ci < Engine::pCurrentScene->SceneMap->Navigation.MapCells; ++ci)
			{
				if(!ViewZones[ri][ci]->LandscapeObject && ViewZones[ri][ci]->ZoneStat == cMap::_view_zone_item::_VIEW_ZONE_STAT::NON){
					Engine::pCurrentScene->DeferredLoadZone(ri, ci);
				}
			}
		}

		return;
	}

	if(arg.kode == _KEY_KODE_::_KEY_F7 && arg.stat == 1){
		Engine::pCurrentScene->SceneMap->Navigation.BuildZonesLinks();

		return;
	}
}

void GameCursorButtonEvent(const cInput::EventCursorButtonArg& arg)
{
	if(arg.event == CURSOR_MOUSEWHEEL){
		if(arg.button > 0){
			Engine::pCurrentScene->pMainCamera->ThirdPersonDistance += 0.5f;
		}
		else{
			Engine::pCurrentScene->pMainCamera->ThirdPersonDistance -= 0.5f;
		}

		Engine::pCurrentScene->pMainCamera->SetupThirdPerson(&pPlayer->position, Engine::pCurrentScene->pMainCamera->ThirdPersonDistance, vec3(0.0f, 0.8f, 0.0f));
	}
}

bool last_mouse_button_stat = false;
void GameCursorMoveEvent(const cInput::EventCursorMoveArg& arg)
{
	float x = arg.x;
	float y = arg.y;
	//printf("GameCursorMoveEvent(%f, %f)\n", x, y);

	if(Input.Cursor.CursorButton2Down){
		POINT cp = { 0, 0 };

		GetCursorPos(&cp);

		if(ScreenToClient(Screen::GameWindow.hWnd, &cp))
		{
			RECT cr = { 0, 0, 0, 0 };
			GetClientRect(Screen::GameWindow.hWnd, &cr);

			if(cp.x > 0 && cp.x < cr.right && cp.y > 0 && cp.y < cr.bottom)
			{
				if(last_mouse_button_stat)
				{
					int offs_x = cp.x - (cr.right / 2);
					int offs_y = (cr.bottom / 2) - cp.y;

					Engine::pCurrentScene->pMainCamera->yaw += offs_x / 2.0f;
					Engine::pCurrentScene->pMainCamera->pitch += offs_y / 2.0f;

					if(Engine::pCurrentScene->pMainCamera->pitch <= -89.0f){
						Engine::pCurrentScene->pMainCamera->pitch = -89.0f;
					}
					else if(Engine::pCurrentScene->pMainCamera->pitch >= 89.0f){
						Engine::pCurrentScene->pMainCamera->pitch = 89.0f;
					}

					Engine::pCurrentScene->pMainCamera->Updating = true;
				}

				last_mouse_button_stat = true;

				RECT wr = { 0, 0, 0, 0 };
				GetWindowRect(Screen::GameWindow.hWnd, &wr);
				SetCursorPos((cr.right / 2) + wr.left, (cr.bottom / 2) + wr.top);
			}
			else{
				last_mouse_button_stat = false;
			}
		}
	}
	else{
		last_mouse_button_stat = false;
	}

	if(Input.Cursor.CursorButton1Down)
	{
		if(!pPlayer->IsPlayAttack())
		{
			pPlayer->Attack();
		}
	}
}

cCollidedObject* Lenin = 0;
cTextObject* PlayerText = 0;
cTextObject* ZonePointText = 0;
cTextObject* ZoneCentreText = 0;

cTextObject* TextLandscepePoint[4] = { 0,0,0,0 };

void cGame::Create()
{
	cScene* pScene = new cScene();
	pScene->AddShader(Engine::pMainShadersContainer);

	pScene->InitCollisions();

	//pScene->EnableGravity(false);

	cMapBuilder* pMapBuilder = new cMapBuilder();

	pMapBuilder->ReadHeightMap("data/maps/map_height_256.hm", 32, 32);

	for(int ri = 0; ri < pMapBuilder->Navigation.MapRows; ++ri)
	{
		for(int ci = 0; ci < pMapBuilder->Navigation.MapCells; ++ci)
		{
			pMapBuilder->LoadZone(ri, ci);
		}
	}

	pMapBuilder->BuildLinks();

	//system("pause");

	pMapBuilder->WriteMap("data/maps/map_256.zl");

	//system("pause");

	cMap* Map = new cMap();
	Map->ReadMap("data/maps/map_256.zl");

	pScene->SetMap(Map);

	PlayerStartPositionX = 2 * 32;// 187 * 512;
	PlayerStartPositionY = 2 * 32;// 141 * 512;
	printf("PlayerStartPositionX:%d PlayerStartPositionY:%d\n", PlayerStartPositionX, PlayerStartPositionY);

	int player_zone_row = -1;
	int player_zone_cell = -1;
	pScene->SceneMap->GetZoneByPoint(PlayerStartPositionX, PlayerStartPositionY, &player_zone_row, &player_zone_cell);
	printf("player_zone_row:%d player_zone_cell:%d\n", player_zone_row, player_zone_cell);

	vec2 target_zon_centre(
		player_zone_cell * pScene->SceneMap->Navigation.ZoneCells + (pScene->SceneMap->Navigation.ZoneCells / 2),
		player_zone_row * pScene->SceneMap->Navigation.ZoneRows + (pScene->SceneMap->Navigation.ZoneRows / 2)
	);

	float target_zon_height = pScene->SceneMap->Navigation.NavZones[player_zone_row][player_zone_cell]->max_height;

	//printf("target_zon_centre: %f %f target_zon_height:%f\n", target_zon_centre.x, target_zon_centre.y, target_zon_height);

	pScene->AllocateLights(3);

	pScene->SceneLights.set_pos_cone(0, vec3(10.0f, 10.0f, 10.0f), 1.0f);
	pScene->SceneLights.set_color_power(0, vec3(0.5f, 0.7f, 0.8f), 1.0f);
	pScene->SceneLights.set_dir_spread(0, normalize(vec3(0.0f, -0.8f, -0.2f)), 10.0f);
	pScene->SceneLights.light_fovs[0] = 10.0f;
	pScene->SceneLights.near_planes[0] = 0.0f;
	pScene->SceneLights.far_planes[0] = 40.0f;
	pScene->SceneLights.perspective_flag[0] = false;

	pScene->SceneLights.set_pos_cone(1, vec3(15.0f, 10.0f, 40.0f), 0.5f);
	pScene->SceneLights.set_color_power(1, vec3(0.35f, 0.5f, 0.35f), 1.0f);
	pScene->SceneLights.set_dir_spread(1, normalize(vec3(0.0f, -0.8f, -0.2f)), 10.0f);
	pScene->SceneLights.light_fovs[1] = 90.0f;
	pScene->SceneLights.near_planes[1] = 0.01f;
	pScene->SceneLights.far_planes[1] = 10.0f;
	pScene->SceneLights.perspective_flag[1] = true;

	pScene->SceneLights.set_pos_cone(2, vec3(20.0f, 10.0f, 80.0f), 0.5f);
	pScene->SceneLights.set_color_power(2, vec3(0.35f, 0.35f, 0.5f), 1.0f);
	pScene->SceneLights.set_dir_spread(2, normalize(vec3(0.0f, -0.8f, -0.2f)), 10.0f);
	pScene->SceneLights.light_fovs[2] = 90.0f;
	pScene->SceneLights.near_planes[2] = 0.01f;
	pScene->SceneLights.far_planes[2] = 10.0f;
	pScene->SceneLights.perspective_flag[2] = true;

	//-------------------------------------------------------------------

	cCamera* pcamera = new cCamera();

	pcamera->Init(Screen::width, Screen::height);

	//pcamera->Position(0.0f, 10.0f, 5.0f);
	//pcamera->Target(0.0f, 0.0f, 0.0f);
	//pcamera->eye = vec3((500.0f, 10.0f, 500.0f));
	pcamera->yaw = 90.0f;
	pcamera->pitch = -15.0f;
	pcamera->Updating = true;
	pcamera->Update();

	pScene->SetMainCamera(pcamera);

	Engine::SetScene(pScene);

	DBG_MainCamera = pcamera;

	//----------------------------------------------
	cInput::EventKey.Subscribe(GameKeyEvent);
	cInput::EventCursorButton.Subscribe(GameCursorButtonEvent);
	cInput::EventCursorMove.Subscribe(GameCursorMoveEvent);
	//----------------------------------------------

	CreateSkyBox(&pScene->SkyBox);

	pPlayer = CreateCharacter();

	pPlayer->SetPosition(target_zon_centre.x, 10.0f/*target_zon_height + 3.0f*/, target_zon_centre.y);

	Engine::pCurrentScene->pMainCamera->SetupThirdPerson(&pPlayer->position, 8.0f, vec3(0.0f, 0.8f, 0.0f));

	Lenin = CreateStatue();
	Lenin->SetPosition(72.56f, 0.0f, 155.27f);
	Lenin->SetRotation(pi<float>(), vec3(0.0f, 1.0f, 0.0f));
	Engine::pCurrentScene->AddObject(Lenin);

	cCollidedObject* Tavern = CreateStructure();
	Tavern->SetPosition(-2.0f, 1.0f, 73.0f);
	//Tavern->SetRotation(pi<float>(), vec3(0.0f, 1.0f, 0.0f));
	Engine::pCurrentScene->AddObject(Tavern);

	pUnit = CreateUnit();
	pUnit->SetPosition(target_zon_centre.x - 3.0f, target_zon_height + 1.0f, target_zon_centre.y + 10.0f);
	Engine::pCurrentScene->AddObject(pUnit);

	//---------------------------------------

	DebugText = new  cTextObject(true);
	DebugText->SetScaling(0.25f, 0.25f, 0.25f);
	DebugText->AddRow(16, vec3(1.0f, 1.0f, 1.0f));

	const char* LeninSayStr = "\x8e\xa1\xb6\xb4\xaa\x20\xbe\xb3\xaf\x20\xb3\xb4\xa5\xa1";
	cTextObject* LeninSay = new  cTextObject(true);
	//LeninSay->SetScaling(0.25f, 0.25f, 0.25f);
	LeninSay->AddRow(strlen(LeninSayStr), vec3(1.0f, 0.5f, 0.1f));
	LeninSay->SetText(0, LeninSayStr);
	LeninSay->SetPosition(Lenin->position.x, Lenin->position.y + 7.5f, Lenin->position.z);
	Engine::pCurrentScene->AddObject(LeninSay);

	const char* str0 = "0";
	const char* str1 = "Hello world!";
	const char* str2 = "_";

	PlayerText = new cTextObject(true);
	PlayerText->SetScaling(0.5f, 0.5f, 0.5f);

	PlayerText->AddRow(64, vec3(0.1f, 1.0f, 0.1f));
	PlayerText->AddRow(64, vec3(0.1f, 1.0f, 0.1f));
	PlayerText->AddRow(32, vec3(0.1f, 1.0f, 0.1f));

	PlayerText->SetText(0, str0);
	PlayerText->SetText(1, str1);
	PlayerText->SetText(2, str2);

	Engine::pCurrentScene->AddObject(PlayerText);
	Engine::pCurrentScene->AddObject(pPlayer);

	ZonePointText = new cTextObject(true);
	//ZonePointText->SetScaling(0.25f, 0.25f, 0.25f);
	ZonePointText->AddRow(32, vec3(1.0f, 1.0f, 1.0f));
	//Engine::pCurrentScene->AddObject(ZonePointText);

	ZoneCentreText = new cTextObject(true);
	ZoneCentreText->AddRow(64, vec3(1.0f, 1.0f, 1.0f));
	ZoneCentreText->AddRow(64, vec3(0.0f, 0.0f, 1.0f));
	//Engine::pCurrentScene->AddObject(ZoneCentreText);

	TextLandscepePoint[0] = new cTextObject(true);
	TextLandscepePoint[0]->AddRow(64, vec3(1.0f, 1.0f, 0.0f));
}

void cGame::UpdateBeforeCalculations()
{
	UpdateInput();
}

void cGame::UpdateAfterCalculations()
{
	Engine::pCurrentScene->SceneLights.pos_cone[0].x = pPlayer->position.x;
	Engine::pCurrentScene->SceneLights.pos_cone[0].z = pPlayer->position.z;
	Engine::pCurrentScene->SceneLights.pos_cone[0].y = pPlayer->position.y + 10.0f;
	DrawCross(Engine::pCurrentScene->SceneLights.pos_cone[0].x, Engine::pCurrentScene->SceneLights.pos_cone[0].y, Engine::pCurrentScene->SceneLights.pos_cone[0].z, 1.0f, "\xff\x00\x00");

	DrawCross(Lenin->position, 1.0f, "\xff\x00\x00");

	Lenin->models_list.reset_enumerator();
	while(cModel* model = Lenin->models_list.next())
	{
		model->mesh_list.reset_enumerator();
		while(cMesh* mesh = model->mesh_list.next())
		{
			mat4 worldmx = Lenin->WorldMx * mesh->mesh_localmx * model->model_localmx;
			DrawCross(vec3(worldmx[3]), 1.0f, "\x00\x00\xff");
		}
	}

	float player_x = pPlayer->position.x;
	float player_y = pPlayer->position.z;

	PlayerText->TextLines[0]->SPrintF("player %f %f %f", pPlayer->position.x, pPlayer->position.y, pPlayer->position.z);
	PlayerText->TextLines[1]->SPrintF(
		"m:[r:%d c:%d] z:[r:%d c:%d]",
		pPlayer->NavInfo.map_pos.row, pPlayer->NavInfo.map_pos.cell,
		pPlayer->NavInfo.zone_pos.row, pPlayer->NavInfo.zone_pos.cell
	);
	//PlayerText->TextLines[2]->SPrintF("%f %f", length(Engine::pCurrentScene->pMainCamera->eye), distance(vec3(0.0f, 0.0f, 0.0f), Engine::pCurrentScene->pMainCamera->eye));

	PlayerText->Update();

	PlayerText->SetPosition(pPlayer->position.x, pPlayer->position.y + 2.0f, pPlayer->position.z);
}

void cGame::UpdateAfterRender()
{
	//printf("---------------------------------------------------- %d %d\n", Engine::pCurrentScene->CurrentViewZonRow, Engine::pCurrentScene->CurrentViewZonCell);
	int view_planes_count = 0;

	int MapRows = Engine::pCurrentScene->SceneMap->Navigation.MapRows;
	int MapCells = Engine::pCurrentScene->SceneMap->Navigation.MapCells;
	int ZoneRows = Engine::pCurrentScene->SceneMap->Navigation.ZoneRows;
	int ZoneCells = Engine::pCurrentScene->SceneMap->Navigation.ZoneCells;

	cMap::_view_zone_item*** ViewZones = Engine::pCurrentScene->SceneMap->ViewZones;

	for(int zri = Engine::pCurrentScene->CurrentViewZonRow - 4; zri < Engine::pCurrentScene->CurrentViewZonRow + 4; ++zri)
	{
		for(int zci = Engine::pCurrentScene->CurrentViewZonCell - 4; zci < Engine::pCurrentScene->CurrentViewZonCell + 4; ++zci)
		{
			int pos_x = zci * ZoneCells;
			int pos_z = zri * ZoneRows;

			if(zri >= Engine::pCurrentScene->CurrentViewZonRow - 4 && zri <= Engine::pCurrentScene->CurrentViewZonRow + 4 && zci >= Engine::pCurrentScene->CurrentViewZonCell - 4 && zci <= Engine::pCurrentScene->CurrentViewZonCell + 4)
				//if(zri == Engine::pCurrentScene->CurrentViewZonRow && zci == Engine::pCurrentScene->CurrentViewZonCell)
			{
				int tzri = zri;
				int tzci = zci;

				if(zri < 0)
				{
					tzri += MapRows * -int(floor(float(zri) / float(MapRows)));
				}
				else if(tzri >= MapRows)
				{
					tzri -= MapRows * int(floor(float(zri) / float(MapRows)));
				}

				if(zci < 0)
				{
					tzci += MapCells * -int(floor(float(zci) / float(MapCells)));
				}
				else if(tzci >= MapCells)
				{
					tzci -= MapCells * int(floor(float(zci) / float(MapCells)));
				}

				if(ViewZones[tzri][tzci]->LandscapeObject && ViewZones[tzri][tzci]->ZoneStat == cMap::_view_zone_item::_VIEW_ZONE_STAT::LOADED){
					ViewZones[tzri][tzci]->LandscapeObject->SetPosition(pos_x, 0.0f, pos_z);
				}

				if(!ViewZones[tzri][tzci]->LandscapeObject->already_collided)
				{
					printf("add collided [%d %d]\n", zri, zci);
					Engine::pCurrentScene->CollisionsDynamicsWorld->addRigidBody(ViewZones[tzri][tzci]->LandscapeObject->RigidBodys[0]);
					ViewZones[tzri][tzci]->LandscapeObject->already_collided = true;
				}

				ViewZones[tzri][tzci]->LandscapeObject->is_rendered = true;

				++view_planes_count;
			}
			else{
				if(ViewZones[zri][zci]->LandscapeObject->already_collided)
				{
					printf("remove collided [%d %d]\n", zri, zci);
					Engine::pCurrentScene->CollisionsDynamicsWorld->removeRigidBody(ViewZones[zri][zci]->LandscapeObject->RigidBodys[0]);
					ViewZones[zri][zci]->LandscapeObject->already_collided = false;
				}

				ViewZones[zri][zci]->LandscapeObject->is_rendered = false;
			}

			vec3 plane_centre = vec3(pos_x + (ZoneCells / 2), 1.0f, pos_z + (ZoneRows / 2));

			/*ZoneCentreText->TextLines[0]->SPrintF(
				"rc:[%d %d] id:[%d %d]",
				zri, zci,
				ViewZones[zri][zci]->row_id, ViewZones[zri][zci]->cell_id
			);

			ZoneCentreText->SetPosition(plane_centre.x, plane_centre.y, plane_centre.z);
			ZoneCentreText->Update();
			ZoneCentreText->Draw(Engine::pCurrentScene->pMainCamera);*/

			if(Engine::pCurrentScene->pMainCamera->IsBoxInFrustum(plane_centre.x, plane_centre.y, plane_centre.z, float(ZoneCells) / 2.0f, 1.0f, float(ZoneRows) / 2.0f))
			{
				++view_planes_count;

				/*ZoneCentreText->SetPosition(pos_x + (ZoneCells / 2), 1.0f, pos_z + (ZoneRows / 2));
				ZoneCentreText->Update();
				ZoneCentreText->Draw(Engine::pCurrentScene->pMainCamera);

				ZonePointText->TextLines[0]->SPrintF(
					"%f %f",
					float(pos_x), float(pos_z)
				);

				ZonePointText->SetPosition(pos_x, 1.0f, pos_z);
				ZonePointText->Update();
				ZonePointText->Draw(Engine::pCurrentScene->pMainCamera);

				TextLandscepePoint[0]->TextLines[0]->SPrintF(
					"%f %f",
					float(pos_x), float(pos_z) + 32.0f
				);
				TextLandscepePoint[0]->SetPosition(float(pos_x), 2.0f, float(pos_z + ZoneRows));
				TextLandscepePoint[0]->Update();
				TextLandscepePoint[0]->Draw(Engine::pCurrentScene->pMainCamera);*/

				/*if(!ViewZones[tzri][tzci]->LandscapeObject && ViewZones[tzri][tzci]->ZoneStat == cMap::_view_zone_item::_VIEW_ZONE_STAT::NON){
					Engine::pCurrentScene->DeferredLoadZone(tzri, tzci);
				}
				else */

			}
		}
	}

	PlayerText->TextLines[2]->SPrintF(
		"[%d %d] view_planes_count:%d",
		Engine::pCurrentScene->CurrentViewZonRow,
		Engine::pCurrentScene->CurrentViewZonCell,
		view_planes_count
	);

	_map_asnode* nav_zone = Engine::pCurrentScene->SceneMap->Navigation.NavZones[pPlayer->NavInfo.map_pos.row][pPlayer->NavInfo.map_pos.cell];

	if(nav_zone && nav_zone->link_nodes)
	{
		vec3 from_pos = vec3((pPlayer->NavInfo.map_pos.cell * ZoneCells) + (ZoneCells / 2), 0.5f, (pPlayer->NavInfo.map_pos.row * ZoneRows) + (ZoneRows / 2));
		//printf("--- %d\n", nav_zone->links_count);
		for(int zli = 0; zli < nav_zone->links_count; ++zli)
		{
			vec3 to_pos = vec3((nav_zone->link_nodes[zli]->c * ZoneCells) + (ZoneCells / 2), 0.5f, (nav_zone->link_nodes[zli]->r * ZoneRows) + (ZoneRows / 2));
			//printf("%d:%dx%d %p\n", zli, nav_zone->link_nodes[zli]->r, nav_zone->link_nodes[zli]->c, nav_zone->link_nodes[zli]);
			DrawLine(
				from_pos,
				to_pos,
				"\xff\x00\xff"
			);
		}
	}
}