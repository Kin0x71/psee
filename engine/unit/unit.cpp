#include "unit.h"
#include <engine/debug/primitives.h>

#include <engine/map/navigation.h>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

cUnit::cUnit()
{
	is_traceable = true;
}

cUnit::~cUnit()
{
	for(int i = 0; i < (int)UNIT_ANIMATION_ID::ACTIONS_COUNT; ++i)
	{
		if(AnimTab[(UNIT_ANIMATION_ID)i]){
			delete AnimTab[(UNIT_ANIMATION_ID)i];
		}
	}
}

void cUnit::UpdateTransAnim()
{
	if(CurAnimId == UNIT_ANIMATION_ID::TRANS && NextAnimId != UNIT_ANIMATION_ID::NONE && !AnimTab[CurAnimId]->PlayFlag){
		//printf("end trans set next:%s\n", AnimTabParts[part][NextAnimIdPart[part]]->name);
		SetAnimAction(NextAnimId);
	}
}

void cUnit::Update()
{
	Bihavior.Update();

	if(AnimTab[CurAnimId])
	{
		AnimTab[CurAnimId]->Update();
	}

	if(pSkeleton)
	{
		for(int i = 0; i < AnimTab[CurAnimId]->BonesFramesCount; ++i)
		{
			pSkeleton->Bones[i].KeyState.position = AnimTab[CurAnimId]->BonesFrames[i].KeyState.position;
			pSkeleton->Bones[i].KeyState.rotation = AnimTab[CurAnimId]->BonesFrames[i].KeyState.rotation;
			pSkeleton->Bones[i].KeyState.scale = AnimTab[CurAnimId]->BonesFrames[i].KeyState.scale;
		}

		pSkeleton->Update();
	}

	cSkeletonObject::Update();

	UpdateTransAnim();

	cUnitBase::Update();
}

void cUnit::AddAnimation(UNIT_ANIMATION_ID ActionId, cAnimation* Animation)
{
	if(AnimTab[ActionId]){
		delete AnimTab[ActionId];
	}

	AnimTab[ActionId] = Animation;
}

bool cUnit::SetAnimAction(UNIT_ANIMATION_ID ActionId)
{
	if(!AnimTab[ActionId])return false;

	CurAnimId = ActionId;

	AnimTab[CurAnimId]->Play();
	return true;
}

void cUnit::MoveForward(bool run)
{
	//printf("MoveForward %d\n",run);
	MovingForwardFlag = true;
	MovingBackwardFlag = false;
	MovingRunFlag = false;

	vec3 direction = GetDirection();

	float ms = MovingSpeed;
	UNIT_ANIMATION_ID ft = UNIT_ANIMATION_ID::WALK;

	if(run)
	{
		MovingRunFlag = true;
		ms = MovingSpeed * 2.0f;
		ft = UNIT_ANIMATION_ID::RUN;
	}

	btMovingVelocity = (btVector3(direction.x, direction.y, direction.z) * ms);

	RigidBodys[0]->setLinearVelocity(btMovingVelocity);

	if(CurAnimId != ft)
	{
		if(AnimTab[ft])
		{
			_setup_trans_animation(
				AnimTab[UNIT_ANIMATION_ID::TRANS],
				AnimTab[CurAnimId],
				AnimTab[ft],
				unit_trans_length
			);

			NextAnimId = ft;

			SetAnimAction(UNIT_ANIMATION_ID::TRANS);
		}
		else{
			SetAnimAction(ft);
		}
	}

	//AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]]->DebugMod = true;
}

void cUnit::MoveToDirection(const vec3& vdir, bool run)
{
	MovingForwardFlag = true;
	MovingBackwardFlag = false;
	MovingRunFlag = false;

	float ms = MovingSpeed;
	UNIT_ANIMATION_ID ft = UNIT_ANIMATION_ID::WALK;

	if(run)
	{
		MovingRunFlag = true;
		ms = MovingSpeed * 2.0f;
		ft = UNIT_ANIMATION_ID::RUN;
	}

	btMovingVelocity = (btVector3(vdir.x, vdir.y, vdir.z) * ms);

	RigidBodys[0]->setLinearVelocity(btMovingVelocity);

	if(CurAnimId != ft)
	{
		if(AnimTab[ft])
		{
			_setup_trans_animation(
				AnimTab[UNIT_ANIMATION_ID::TRANS],
				AnimTab[CurAnimId],
				AnimTab[ft],
				unit_trans_length
			);

			NextAnimId = ft;

			SetAnimAction(UNIT_ANIMATION_ID::TRANS);
		}
		else{
			SetAnimAction(ft);
		}
	}
}

void cUnit::MoveStop()
{
	MovingForwardFlag = false;
	MovingBackwardFlag = false;
	MovingRunFlag = false;

	btMovingVelocity = btVector3(0.0f, 0.0f, 0.0f);
	RigidBodys[0]->setLinearVelocity(btMovingVelocity);

	if(CurAnimId != UNIT_ANIMATION_ID::IDLE)
	{
		if(AnimTab[CurAnimId])
		{
			_setup_trans_animation(
				AnimTab[UNIT_ANIMATION_ID::TRANS],
				AnimTab[CurAnimId],
				AnimTab[UNIT_ANIMATION_ID::IDLE],
				unit_trans_length
			);

			NextAnimId = UNIT_ANIMATION_ID::IDLE;

			SetAnimAction(UNIT_ANIMATION_ID::TRANS);
		}
		else{
			SetAnimAction(UNIT_ANIMATION_ID::IDLE);
		}
	}
}