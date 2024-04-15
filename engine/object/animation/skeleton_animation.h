#ifndef _SKELETON_ANIMATION_H_
#define _SKELETON_ANIMATION_H_

#include "animation.h"

//using namespace glm;

struct cBone
{
	cBone* pSibling = 0;
	cBone* pChildren = 0;
	int Id = -1;
	int ParentId = -1;

	cKey KeyState;

	vec3 world_position;
	vec3 position;
	quat rotation;
	vec3 scale;

	mat4 WorldMx;
	mat4 InvWorldMx;

	char* Name = 0;

	~cBone();
};

class cSkeleton{
public:
	int BonesCount = 0;
	cBone* Bones = 0;

	mat4* pMxNodesAnimate = 0;
	mat4* pMxNodesResult = 0;

	cSkeleton();
	cSkeleton(int bones_count);
	~cSkeleton();

	void Allocate(int bones_count);

	int GetBoneId(char* name);
	int GetBoneId(const char* name){ return GetBoneId((char*)name); }

	void _calculate_world_transform(cBone* bone, mat4* pParentMatrix = 0);
	void CalculateWorldTransforms();

	void _update_bones(cBone* bone, mat4* pParentMatrix = 0);
	void Update();
};

#endif