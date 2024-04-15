#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_
#include <engine/camera.h>

void DrawCross(float x, float y, float z, float l, const char* color, float h = 1.5f);
void DrawCross(vec3 v, float l, const char* color, float h = 1.5f);
void DrawLine(float xa, float ya, float za, float xb, float yb, float zb, const char* color, float h = 1.5f);
void DrawLine(vec3 a, vec3 b, const char* color, float h = 1.5f);
void DrawTriangle(const vec3& p0, const vec3& p1, const vec3& p2, const vec4& color);
void DrawSphere(const vec3& pos, float r, int lats, int longs);
void DrawBox(const vec3& pos, const vec3& halfs);

struct _Capsule{
	vec3 start = vec3(0.0f, 0.0f, 0.0f);
	vec3 end = vec3(0.0f, 0.0f, 0.0f);
	float radius = 0.0f;

	_Capsule(vec3 s, vec3 e, float r)
	{
		start = s;
		end = e;
		radius = r;
	}
};

void DrawCapsule(cCamera* pCamera, const _Capsule& c, const vec4& color);
void DrawQuad(int tex_id);
void DrawFrustum(cCamera* pCamera, const vec3& pos, float frustum[6][4]);
#endif