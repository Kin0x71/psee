#ifndef _CHARACTER_H
#define _CHARACTER_H
#include "unit_base.h"
#include <map>

using namespace std;

class cCharacter :public cUnitBase
{
public:

#define char_trans_length 3.0f

	enum class ANIMATION_ID{
		NONE,
		TRANS,
		IDLE,
		WALK,
		RUN,
		BACKWARD,
		JUMP,
		ATTACK,
		ACTIONS_COUNT
	};

	/*const char* action_names[] = {
		"NONE",
		"IDLE",
		"WALK",
		"RUN",
		"TURN_LEFT",
		"TURN_RIGHT"
	};*/

	/*char*[] action_names = {
		{ "NONE" },
		{ "IDLE" },
		{ "WALK" },
		{ "RUN" },
		{ "TURN_LEFT" },
		{ "TURN_RIGHT" }
	};*/

	enum class BODY_PART{
		TORSO = 0,
		LEGS = 1,
		ALL
	};

	static const char* action_names[];

	struct _anim_action{
		ANIMATION_ID action_id = ANIMATION_ID::NONE;
		cAnimation* pAnim = 0;
	};

	struct _attach_item{
		cSkeleton* pSkeleton = 0;
		int bone_id = -1;
		mat4* pmx_world = 0;

		_attach_item(cSkeleton* ps, int b, mat4* pw)
		{
			pSkeleton = ps;
			bone_id = b;
			pmx_world = pw;
		}

		_attach_item()
		{
			pSkeleton = 0;
			bone_id = 0;
			pmx_world = 0;
		}
	};

	map<BODY_PART, int> attach_parts_bones;
	map<BODY_PART, cList<int>> PartBoneIds;
	map<BODY_PART, map<ANIMATION_ID, cAnimation*>> AnimTabParts;
	map<BODY_PART, ANIMATION_ID> CurAnimIdPart;
	map<BODY_PART, ANIMATION_ID> NextAnimIdPart;
	map<BODY_PART, ANIMATION_ID> LastAnimIdPart;

	enum class JUMP_STAT{
		NONE,STARTING,FLY,LOST_CONTACT_LANDSCEPE
	};
	JUMP_STAT JumpStat = JUMP_STAT::NONE;
	bool JumpingFlag = false;
	bool JumpingFlyFlag = false;

	mat4* bones_transforms;
	vec3 dbg_dir;
	bool dbg_pause = false;
	btVector3 dbg_StartJumpPos;
	btVector3 dbg_EndJumpPos;
	float dbg_JumpStartTime = 0.0f;
	float dbg_JumpFlyStartTime = 0.0f;

	cCharacter()
	{
		CurAnimIdPart[BODY_PART::TORSO] = ANIMATION_ID::NONE;
		CurAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::NONE;
		NextAnimIdPart[BODY_PART::TORSO] = ANIMATION_ID::NONE;
		NextAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::NONE;
		LastAnimIdPart[BODY_PART::TORSO] = ANIMATION_ID::NONE;
		LastAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::NONE;

		attach_parts_bones[BODY_PART::TORSO] = -1;
		attach_parts_bones[BODY_PART::LEGS] = -1;

		is_traceable = true;
	}

	~cCharacter()
	{
	}

	void release();

	void CollideCallback(const btRigidBody* SelfRigidBody, const vec3& contact, const btRigidBody* pCollisionRigidBody);

	void UpdateTransAnimPart(BODY_PART part);
	void UpdateSkeletonPart(BODY_PART part);
	void UpdateAttachments();

	void Update();
	void UpdateAfterTransforms();
	void Draw(cCamera* pCamera);

	void AddAnimationPart(ANIMATION_ID ActionId, BODY_PART part, cAnimation* Animation);
	void AddAnimation(ANIMATION_ID ActionId, cAnimation* Animation);
	bool SetAnimAction(ANIMATION_ID ActionId, BODY_PART part);

	inline bool IsMoveForward(){ return MovingForwardFlag; }
	bool IsMoveBackward(){ return MovingBackwardFlag; }
	bool IsPlayAttack();
	
	void MoveForward(bool run);
	void MoveBackward();
	void MoveStop();
	void Jump();
	void JumpEnd();
	void Attack();

	map<int, _attach_item> AttachedMatrices;
	void AttachSkeletonObject(cSkeletonObject* pSkeletonObject, const char* attached_bone, const char* attaching_bone);
};

#endif