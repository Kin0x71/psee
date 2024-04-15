#include "skeleton_animation.h"

#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

extern float DBG_TestAnimTime;

cBone::~cBone(){
	//printf("~cBone() %s\n", Name);
	if(Name){
		delete[] Name;
	}
}

cSkeleton::cSkeleton()
{
	BonesCount = 0;
	Bones = 0;

	pMxNodesAnimate = 0;
	pMxNodesResult = 0;
}

cSkeleton::cSkeleton(int bones_count)
{
	Allocate(bones_count);
}

cSkeleton::~cSkeleton()
{
	printf("~cSkeleton()\n");
	if(Bones){
		delete[] Bones;
		Bones = 0;
	}

	if(pMxNodesAnimate){
		delete[] pMxNodesAnimate;
		pMxNodesAnimate = 0;
	}

	if(pMxNodesResult){
		delete[] pMxNodesResult;
		pMxNodesResult = 0;
	}
}

void cSkeleton::Allocate(int bones_count)
{
	BonesCount = bones_count;

	Bones = new cBone[BonesCount];

	pMxNodesAnimate = new mat4[BonesCount];
	pMxNodesResult = new mat4[BonesCount];

	for(int bi = 0; bi < BonesCount; ++bi)
	{
		pMxNodesAnimate[bi] = mat4(1.0f);
		pMxNodesResult[bi] = mat4(1.0f);
	}
}

int cSkeleton::GetBoneId(char* name)
{
	for(int bi = 0; bi < BonesCount; ++bi)
	{
		if(!strcmp(Bones[bi].Name,name))
		{
			return bi;
		}
	}

	return -1;
}

void cSkeleton::_calculate_world_transform(cBone* bone, mat4* pParentMatrix)
{
	mat4 mx_source = translate(mat4(1.0f), bone->position) * mat4_cast(bone->rotation) * scale(mat4(1.0f), bone->scale);

	bone->KeyState.position = bone->position;
	bone->KeyState.rotation = bone->rotation;
	bone->KeyState.scale = bone->scale;

	if(pParentMatrix){
		bone->WorldMx = (*pParentMatrix) * mx_source;
	}else{
		bone->WorldMx = mx_source;
	}

	bone->InvWorldMx = inverse(bone->WorldMx);

	bone->world_position = vec3(bone->WorldMx[3][0], bone->WorldMx[3][1], bone->WorldMx[3][2]);

	if(bone->pSibling)_calculate_world_transform(bone->pSibling, pParentMatrix);
	if(bone->pChildren)_calculate_world_transform(bone->pChildren, &bone->WorldMx);
}

void cSkeleton::CalculateWorldTransforms()
{
	_calculate_world_transform(&Bones[0]);
}

void cSkeleton::_update_bones(cBone* bone, mat4* pParentMatrix)
{
	int id = bone->Id;

	mat4 mx_anim = 
			translate(mat4(1.0f), bone->KeyState.position) *
			mat4_cast(bone->KeyState.rotation) *
			scale(mat4(1.0f), bone->KeyState.scale);

	if(pParentMatrix){
		pMxNodesAnimate[id] = (*pParentMatrix) * mx_anim;
	}else{
		pMxNodesAnimate[id] = mx_anim;
	}
	
	if(bone->pSibling)_update_bones(bone->pSibling, pParentMatrix);
	if(bone->pChildren)_update_bones(bone->pChildren, &pMxNodesAnimate[id]);
}

void cSkeleton::Update()
{
	_update_bones(&Bones[0]);

	for(int bi = 0; bi < BonesCount; ++bi)
	{
		pMxNodesResult[bi] = pMxNodesAnimate[bi] * Bones[bi].InvWorldMx;
	}
}