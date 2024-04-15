#ifndef _UNIT_BASE_H_
#define _UNIT_BASE_H_
#include <engine/object/objects.h>

class cUnitBase :public cSkeletonObject{
public:

	float MovingSpeed = 5.0f;
	bool MovingRunFlag = false;
	bool MovingForwardFlag = false;
	bool MovingBackwardFlag = false;
	bool Rotating = false;
	quat RotationTargetQuaternion;
	float RotationScalar = 0.0f;
	float RotationSpeed = 0.0f;
	float RotationStartTime = 0.0f;

	vec3 floor_vec(vec3& v)
	{
		return vec3(floor(v.x * 100.0f) / 100.0f, floor(v.y * 100.0f) / 100.0f, floor(v.z * 100.0f) / 100.0f);
	}

	void Update();

	vec3 GetForwardVector();
	vec3 GetDirection();
	void LookAt(const vec3& vto, bool update_moving_direction = true);
	void RotateLookAt(const vec3& vto, float speed);

	virtual void MoveForward(bool run){};
	virtual void MoveToDirection(const vec3& vdir, bool run){};
	virtual void MoveStop(){};
};

#endif