#ifndef _UNIT_H_
#define _UNIT_H_

#include "unit_base.h"
#include "unit_bihavior.h"

#include <map>

using namespace std;

class cUnit : public cUnitBase
{
public:

#define unit_trans_length 3.0f

	enum class RIGIDBODY_TYPE :int{
		LANDSCAPE = 0,
		UNITS = 1
	};

	map<UNIT_ANIMATION_ID, cAnimation*> AnimTab;

	float MovingSpeed = 3.0f;
	UNIT_ANIMATION_ID CurAnimId = UNIT_ANIMATION_ID::NONE;
	UNIT_ANIMATION_ID NextAnimId = UNIT_ANIMATION_ID::NONE;

	cUnitBihavior Bihavior;

	cUnit();
	~cUnit();

	void SetBihavior(const cBihaviorBase::_start_param_base& start_param){
		Bihavior.SetBihavior(this, start_param);
	}

	void UpdateTransAnim();
	void Update();
	void AddAnimation(UNIT_ANIMATION_ID ActionId, cAnimation* Animation);
	bool SetAnimAction(UNIT_ANIMATION_ID ActionId);

	void MoveForward(bool run);
	void MoveToDirection(const vec3& vdir, bool run);
	void MoveStop();
};
#endif