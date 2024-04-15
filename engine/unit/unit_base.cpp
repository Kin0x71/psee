#include "unit_base.h"
#include <engine/common/timer.h>

extern cTimer Timer;

void cUnitBase::Update()
{
	if(Rotating)
	{
		//printf("RotationScalar:  %f\n", RotationScalar);

		if(RotationScalar > 0.0f)
		{
			btTransform rb_mx = RigidBodys[0]->getWorldTransform();
			mat4 glm_mx;
			rb_mx.getOpenGLMatrix((float*)&glm_mx);
			quat cur_q = normalize(quat_cast(glm_mx));

			quat new_q = slerp(cur_q, RotationTargetQuaternion, RotationScalar);

			rb_mx.setRotation(btQuaternion(new_q.x, new_q.y, new_q.z, new_q.w));

			RigidBodys[0]->setWorldTransform(rb_mx);
		}

		RotationScalar += (Timer.ProgrammTime - RotationStartTime) * RotationSpeed;
		RotationStartTime = Timer.ProgrammTime;

		if(RotationScalar >= 1.0f){
			Rotating = false;
		}
	}
}

vec3 cUnitBase::GetForwardVector()
{
	btTransform rb_mx = RigidBodys[0]->getWorldTransform();

	btVector3 forward = rb_mx(btVector3(0.0f, 0.0f, 1.0f));

	return vec3(forward.x(), forward.y(), forward.z());
}

vec3 cUnitBase::GetDirection()
{
	vec3 forward = GetForwardVector();
	vec3 position = GetPosition();
	return normalize(forward - position);
}

void cUnitBase::LookAt(const vec3& vto, bool update_moving_direction)
{
	btTransform rb_mx = RigidBodys[0]->getWorldTransform();

	btVector3 rb_pos = rb_mx.getOrigin();

	mat4 lookmx = lookAt(vto, vec3(rb_pos.x(), rb_pos.y(), rb_pos.z()), vec3(0.0f, 1.0f, 0.0f));

	quat rotation = normalize(quat_cast(lookmx));

	rb_mx.setRotation(btQuaternion(rotation.x, -rotation.y, rotation.z, rotation.w));
	rb_mx.setOrigin(rb_pos);

	RigidBodys[0]->setWorldTransform(rb_mx);

	if(update_moving_direction)
	{
		btVector3 linear_velocity = RigidBodys[0]->getLinearVelocity();
		if(linear_velocity.length() > 0.0f){

			vec3 dir = GetDirection();

			btVector3 new_linear_velocity = (btVector3(dir.x, dir.y, dir.z) * btMovingVelocity.length());

			btMovingVelocity = new_linear_velocity;

			RigidBodys[0]->setLinearVelocity(btMovingVelocity + btVector3(0.0f, linear_velocity.y(), 0.0f));

			linear_velocity = RigidBodys[0]->getLinearVelocity();
			//printf("LookAt lv:%f %f %f\n", btMovingVelocity.x(), btMovingVelocity.y(), btMovingVelocity.z());
		}
	}

	//printf("LookAt\n");
}

void cUnitBase::RotateLookAt(const vec3& vto, float speed)
{
	btTransform rb_mx = RigidBodys[0]->getWorldTransform();
	btVector3 rb_pos = rb_mx.getOrigin();

	mat4 lookmx = lookAt(vto, vec3(rb_pos.x(), rb_pos.y(), rb_pos.z()), vec3(0.0f, 1.0f, 0.0f));

	quat need_rotation = normalize(quat_cast(lookmx));

	if(vto.z > rb_pos.z()){
		need_rotation.y = -need_rotation.y;
	}
	else{
		need_rotation.w = -need_rotation.w;
	}

	RotationTargetQuaternion=normalize(need_rotation);

	Rotating = true;

	RotationScalar = 0.0f;
	RotationSpeed = speed;

	RotationStartTime = Timer.ProgrammTime;
}