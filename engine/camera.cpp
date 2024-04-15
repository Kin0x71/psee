
#include "camera.h"
#include <stdio.h>
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

cCamera::cCamera()
{
	width=0;
	height=0;

	eye=vec3(0.0f,0.0f,0.0f);
	target=vec3(0.0f,0.0f,0.0f);
	dir=vec3(0.0f,0.0f,-1.0f);

	dir=normalize(dir);

	memset(frustum, 0, sizeof(frustum));

	Updating=true;
}

void cCamera::ProjectionPerspective()
{
	ProjMx = perspective(glm::pi<float>() * 0.25f, float(width)/float(height), 0.5f, 1024.0f );
}

void cCamera::ProjectionPerspective(float width,float height)
{
	ProjMx = perspective(glm::pi<float>() * 0.25f, float(width)/float(height), 0.5f, 1000.0f );
}

void cCamera::Init(int w,int h)
{
	width=w;
	height=h;
	
	ProjectionPerspective();
}

void cCamera::Position(float x, float y, float z)
{
	eye = vec3(x, y, z);

	dir = normalize(target - eye);

	Updating = true;
}

void cCamera::Target(float x, float y, float z)
{
	target = vec3(x, y, z);

	dir = normalize(target - eye);

	Updating = true;
}

void cCamera::Forward(float s)
{
	vec3 offs = normalize(dir) * s;

	eye += offs;
	target += offs;

	Updating = true;
}

void cCamera::Strafe(float s)
{
	vec3 offs = normalize(cross(dir, vec3(0.0f, 1.0f, 0.0f))) * s;

	eye += offs;
	target += offs;

	Updating = true;
}

void cCamera::Rotate(vec3 axis, float s)
{
	dir = normalize(rotate(normalize(dir), s, axis));

	target = eye + dir;

	Updating = true;
}

void cCamera::UpdateYawPitchRoll()
{
	dir.x = cos(radians(yaw)) * cos(radians(pitch));
	dir.y = sin(radians(pitch));
	dir.z = sin(radians(yaw)) * cos(radians(pitch));
}

void cCamera::SetupThirdPerson(vec3* target, float dist, const vec3& offset)
{
	ThirdPersonFlag = true;
	ThirdPersonTargetRef = target;
	ThirdPersonDistance = dist;
	ThirdPersonTargetOffset = offset;
}

void cCamera::Update()
{
	if(!Updating)return;

	if(ThirdPersonFlag){
		UpdateYawPitchRoll();

		eye = ((*ThirdPersonTargetRef) + ThirdPersonTargetOffset) - dir * ThirdPersonDistance;
	}

	target = eye + dir * 1.0f;

	ViewMx = lookAt(eye, target, vec3(0.0f, 1.0f, 0.0f));

	ProjViewMx = ProjMx * ViewMx;

	UpdateFrustum();

	Updating = false;
}

void cCamera::UpdateFrustum()
{
	float proj[16];
	float modl[16];
	float clip[16];
	float t;

	memcpy(&proj, &ProjMx, sizeof(mat4));
	memcpy(&modl, &ViewMx, sizeof(mat4));

	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
	clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	frustum[0][0] = clip[3] - clip[0];
	frustum[0][1] = clip[7] - clip[4];
	frustum[0][2] = clip[11] - clip[8];
	frustum[0][3] = clip[15] - clip[12];

	t = sqrtf(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	frustum[1][0] = clip[3] + clip[0];
	frustum[1][1] = clip[7] + clip[4];
	frustum[1][2] = clip[11] + clip[8];
	frustum[1][3] = clip[15] + clip[12];

	t = sqrtf(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	frustum[2][0] = clip[3] + clip[1];
	frustum[2][1] = clip[7] + clip[5];
	frustum[2][2] = clip[11] + clip[9];
	frustum[2][3] = clip[15] + clip[13];

	t = sqrtf(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	frustum[3][0] = clip[3] - clip[1];
	frustum[3][1] = clip[7] - clip[5];
	frustum[3][2] = clip[11] - clip[9];
	frustum[3][3] = clip[15] - clip[13];

	t = sqrtf(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	frustum[4][0] = clip[3] - clip[2];
	frustum[4][1] = clip[7] - clip[6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	t = sqrtf(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	frustum[5][0] = clip[3] + clip[2];
	frustum[5][1] = clip[7] + clip[6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];

	t = sqrtf(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;
}

bool cCamera::IsPointInFrustum(float x, float y, float z)
{
	for(int p = 0; p < 6; ++p){
		if(frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0){
			return false;
		}
	}

	return true;
}

bool cCamera::IsPointsInFrustum(int numpoints, vec3* pointlist)
{
	int f = 0;
	int p = 0;

	for(; f < 6; ++f)
	{
		for(; p < numpoints; ++p)
		{
			if(frustum[f][0] * pointlist[p].x + frustum[f][1] * pointlist[p].y + frustum[f][2] * pointlist[p].z + frustum[f][3] > 0)
				break;
		}
		if(p == numpoints)
			return false;
	}
	return true;
}

bool cCamera::IsSphereInFrustum(float x, float y, float z, float radius)
{
	for(int p = 0; p < 6; p++)
	{
		float d = frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3];
		if(d <= -radius) {
			return false;
		}
	}
	return true;
}

bool cCamera::IsBoxInFrustum(float x, float y, float z, float size_x, float size_y, float size_z)
{
	for(int i = 0; i < 6; i++)
	{
		if(frustum[i][0] * (x - size_x) + frustum[i][1] * (y - size_y) + frustum[i][2] * (z - size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x + size_x) + frustum[i][1] * (y - size_y) + frustum[i][2] * (z - size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x - size_x) + frustum[i][1] * (y + size_y) + frustum[i][2] * (z - size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x + size_x) + frustum[i][1] * (y + size_y) + frustum[i][2] * (z - size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x - size_x) + frustum[i][1] * (y - size_y) + frustum[i][2] * (z + size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x + size_x) + frustum[i][1] * (y - size_y) + frustum[i][2] * (z + size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x - size_x) + frustum[i][1] * (y + size_y) + frustum[i][2] * (z + size_z) + frustum[i][3] > 0.0f)
			continue;
		if(frustum[i][0] * (x + size_x) + frustum[i][1] * (y + size_y) + frustum[i][2] * (z + size_z) + frustum[i][3] > 0.0f)
			continue;

		return false;
	}

	return true;
}

/*
bool cCamera::IsRectangleInFrustum(const vec3& left, const vec3& top, const vec3& right, const vec3& bottom)
{
	for(int i = 0; i < 6; i++) {
		if(frustum[i][0] * (X - Width) + frustum[i][1] * (Y - Height) + frustum[i][2] * (Z - Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X + Width) + frustum[i][1] * (Y - Height) + frustum[i][2] * (Z - Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X - Width) + frustum[i][1] * (Y + Height) + frustum[i][2] * (Z - Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X + Width) + frustum[i][1] * (Y + Height) + frustum[i][2] * (Z - Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X - Width) + frustum[i][1] * (Y - Height) + frustum[i][2] * (Z + Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X + Width) + frustum[i][1] * (Y - Height) + frustum[i][2] * (Z + Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X - Width) + frustum[i][1] * (Y + Height) + frustum[i][2] * (Z + Length) + frustum[i][3] > 0)
			continue;
		if(frustum[i][0] * (X + Width) + frustum[i][1] * (Y + Height) + frustum[i][2] * (Z + Length) + frustum[i][3] > 0)
			continue;
		return false;
	}
	return true;
}*/

bool linePlaneIntersection(const vec3& ray, const vec3& rayOrigin, const vec3& normal, const vec3& coord)
{
	float d = dot(normal, coord);

	if(dot(normal, ray) == 0) {
		return false;
	}

	return true;
}

bool cCamera::IsLineInFrustum(const vec3& a, const vec3& b)
{
	for(int p = 0; p < 6; ++p)
	{
		if(linePlaneIntersection(a, normalize(a - b), normalize(vec3(frustum[p][0], frustum[p][1], frustum[p][2])), eye))continue;
		return false;
	}

	return true;
}
