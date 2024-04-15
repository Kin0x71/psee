#ifndef _UNIT_BIHAVIOR_H_
#define _UNIT_BIHAVIOR_H_

#include "unit_base.h"
#include <engine/map/navigation.h>

enum class UNIT_ANIMATION_ID{
	NONE,
	TRANS,
	IDLE,
	WALK,
	RUN,
	ACTIONS_COUNT
};

enum class UNIT_BIHAVIOR_TYPE{
	NOTHING,
	AGGRESSIVE_TO_TARGET
};

class cUnitActionBase{
public:

	cUnitBase* SelfUnit = 0;

	virtual void Update(){}
};

class cUnitAction_FollowToTarget:public cUnitActionBase{
public:

	enum class ACTION{
		FIND_PATH, MOVE_TO_ZONE, MOVE_TO_POINT, END
	};

	cObjectBase* Target = 0;
	float ToDistance = 0.0f;
	ACTION CurrentAction = ACTION::FIND_PATH;

	cNavigation::_path_container PathContainer;
	vec3 NextPos;
	float LastDist = 0.0f;

	cUnitAction_FollowToTarget(cUnitBase* self_unit, cObjectBase* target){
		SelfUnit = self_unit;
		Target = target;
		//ToDistance = to_distance;

		NextPos = vec3();
	}

	void Update();
};

class cUnitAction_AttacTarget :public cUnitActionBase{
public:
	cObjectBase* Target = 0;

	cUnitAction_AttacTarget(cUnitBase* self_unit, cObjectBase* target){
		SelfUnit = self_unit;
		Target = target;
	}

	void Update();
};

class cBihaviorBase{
public:

	struct _start_param_base{
		UNIT_BIHAVIOR_TYPE bihavior_type = UNIT_BIHAVIOR_TYPE::NOTHING;
	};

	cUnitActionBase** Actions = 0;
	cUnitActionBase* CurrentAction = 0;

	virtual void Update(){}
};

class cBihavior_AggressiveToTarget:public cBihaviorBase{
public:

	enum class ACTIONS:int{
		FOLLOW_TO_TARGET,ATTACK_TARGET,
		ACTIONS_COUNT
	};

	struct _start_param:_start_param_base{
		cObjectBase* target = 0;
		float attac_distance = 0.0f;

		_start_param(cObjectBase* Target){
			bihavior_type = UNIT_BIHAVIOR_TYPE::AGGRESSIVE_TO_TARGET;
			target = Target;
			//attac_distance = AttacDistance;
		}
	};

	cBihavior_AggressiveToTarget(cUnitBase* self_unit, cObjectBase* target);
	~cBihavior_AggressiveToTarget();

	void Update();
};

class cUnitBihavior{
public:

	cBihaviorBase* CurrentBihavior = 0;

	void SetBihavior(cUnitBase* self_unit, const cBihaviorBase::_start_param_base& start_param);
	void Update();
};

#endif