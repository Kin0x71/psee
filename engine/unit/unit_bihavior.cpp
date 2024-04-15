#include "unit_bihavior.h"
#include <engine/engine.h>
#include <engine/debug/primitives.h>

#include <stdio.h>

void cUnitAction_FollowToTarget::Update()
{
	vec3 target_pos = Target->GetPosition();
	vec3 self_pos = SelfUnit->GetPosition();

	if(SelfUnit->MovingForwardFlag && distance(self_pos, vec3(target_pos.x, self_pos.y, target_pos.z)) <= ToDistance){
		CurrentAction = ACTION::END;
		SelfUnit->MoveStop();
		return;
	}

	switch(CurrentAction)
	{
		case ACTION::FIND_PATH:
		{
			if(distance(SelfUnit->position, Target->position) > ToDistance)
			{
				if(SelfUnit->NavInfo.map_pos != Target->NavInfo.map_pos)
				{
					//Engine::pCurrentScene->SceneMap->Navigation.MapFindPath(SelfUnit->NavInfo.map_pos, Target->NavInfo.map_pos);
				}

				//printf("%f %f->%f %f\n", SelfUnit->position.x, SelfUnit->position.z, Target->position.x, Target->position.z);

				PathContainer.path_list.free();

				/*if(Engine::pCurrentScene->SceneMap->Navigation.FindPath(SelfUnit->position, Target->position, &PathContainer))
				{
					CurrentAction = ACTION::MOVE_TO_POINT;

					vec3 first_point = PathContainer.path_list.pop();

					NextPos = PathContainer.path_list.pop();
					LastDist = distance(SelfUnit->position, NextPos);

					SelfUnit->LookAt(vec3(NextPos.x, SelfUnit->GetPosition().y, NextPos.z));
					SelfUnit->MoveForward(false);
				}*/
			}
		}
		break;

		case ACTION::MOVE_TO_POINT:
		{
			vec3 pa = PathContainer.path_list[0];
			for(int i = 1; i < PathContainer.path_list.count; ++i)
			{
				DrawLine(pa, PathContainer.path_list[i], "\x00\x00\xff");
				pa = PathContainer.path_list[i];
			}

			DrawCross(NextPos, 0.3f, "\x00\x00\xff");

			float dist = distance(self_pos, vec3(NextPos.x, self_pos.y, NextPos.z));

			if(dist < 0.01f || dist > LastDist)
			{
				if(PathContainer.path_list.count == 0){
					CurrentAction = ACTION::END;
					SelfUnit->MoveStop();
				}
				else{
					NextPos = PathContainer.path_list.pop();

					SelfUnit->MoveToDirection(normalize(vec3(NextPos.x, self_pos.y, NextPos.z) - self_pos), false);
					SelfUnit->RotateLookAt(vec3(NextPos.x, self_pos.y, NextPos.z), 1.0f);
				}

				dist = distance(self_pos, vec3(NextPos.x, self_pos.y, NextPos.z));
			}

			LastDist = dist;
		}
		break;
	}
}

void cUnitAction_AttacTarget::Update()
{

}

cBihavior_AggressiveToTarget::cBihavior_AggressiveToTarget(cUnitBase* self_unit, cObjectBase* target)
{
	Actions = new cUnitActionBase*[(int)ACTIONS::ACTIONS_COUNT];

	Actions[(int)ACTIONS::FOLLOW_TO_TARGET] = new cUnitAction_FollowToTarget(self_unit, target);
	Actions[(int)ACTIONS::ATTACK_TARGET] = new cUnitAction_AttacTarget(self_unit, target);

	CurrentAction = Actions[(int)ACTIONS::FOLLOW_TO_TARGET];
}

cBihavior_AggressiveToTarget::~cBihavior_AggressiveToTarget()
{
	for(int i = 0; i < (int)ACTIONS::ACTIONS_COUNT; ++i)
	{
		delete Actions[i];
	}

	delete[] Actions;
}

void cBihavior_AggressiveToTarget::Update()
{
	if(CurrentAction){
		CurrentAction->Update();
	}
}

void cUnitBihavior::SetBihavior(cUnitBase* self_unit, const cBihaviorBase::_start_param_base& start_param)
{
	if(CurrentBihavior)delete CurrentBihavior;

	switch(start_param.bihavior_type)
	{
		case UNIT_BIHAVIOR_TYPE::AGGRESSIVE_TO_TARGET:
			CurrentBihavior = new cBihavior_AggressiveToTarget(
				self_unit,
				((cBihavior_AggressiveToTarget::_start_param&)start_param).target
				//((cBihavior_AggressiveToTarget::_start_param&)start_param).attac_distance
			);
			break;
	}
}

void cUnitBihavior::Update()
{
	if(CurrentBihavior)CurrentBihavior->Update();
}