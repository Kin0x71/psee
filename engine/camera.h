
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;

class cCamera
{
public:

	int width;
	int height;

	float yaw = 0.0f;
	float pitch = 0.0f;

	vec3 eye;
	vec3 target;
	vec3 dir;

	int viewport[4];

	mat4 ProjMx;
	mat4 ViewMx;
	mat4 ProjViewMx;

	float frustum[6][4];
	bool Updating;

	bool ThirdPersonFlag = false;
	vec3* ThirdPersonTargetRef = 0;
	vec3 ThirdPersonTargetOffset;
	float ThirdPersonDistance = 0.0f;

	cCamera();

	void Init(int w,int h);

	void ProjectionPerspective();
	void ProjectionPerspective(float width,float height);

	void Position(float x, float y, float z);
	void Target(float x, float y, float z);

	void Forward(float s);
	void Strafe(float s);
	void Rotate(vec3 axis, float s);
	void UpdateYawPitchRoll();
	void SetupThirdPerson(vec3* target, float dist, const vec3& offset);

	void Update();
	void UpdateFrustum();
	bool IsPointInFrustum( float x, float y, float z );
	bool IsPointsInFrustum( int numpoints,vec3* pointlist );
	bool IsSphereInFrustum(float x, float y, float z, float radius);
	bool IsBoxInFrustum(float x, float y, float z, float size_x, float size_y, float size_z);
	//bool IsRectangleInFrustum(const vec3& left, const vec3& top, const vec3& right, const vec3& bottom);
	bool IsLineInFrustum(const vec3& a, const vec3& b);
};

#endif