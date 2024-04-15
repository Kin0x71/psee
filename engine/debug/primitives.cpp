#include "primitives.h"
#include <engine/common/gl_head.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

cCamera* DBG_MainCamera = 0;

void DrawCross(float x, float y, float z, float l, const char* color, float h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&DBG_MainCamera->ProjMx);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&DBG_MainCamera->ViewMx);

	glDisable(GL_TEXTURE_2D);

	glColor3d((double)(unsigned char)color[0] / 255.0, (double)(unsigned char)color[1] / 255.0, (double)(unsigned char)color[2] / 255.0);
	glLineWidth(h);

	glBegin(GL_LINES);

	glVertex3f(x - l, y, z);
	glVertex3f(x + l, y, z);
	glVertex3f(x, y, z - l);
	glVertex3f(x, y, z + l);

	glVertex3f(x, y - l, z);
	glVertex3f(x, y + l, z);

	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void DrawCross(vec3 v, float l, const char* color, float h)
{
	DrawCross(v.x, v.y, v.z, l, color, h);
}

void DrawLine(float xa, float ya, float za, float xb, float yb, float zb, const char* color, float h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&DBG_MainCamera->ProjMx);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&DBG_MainCamera->ViewMx);

	glDisable(GL_TEXTURE_2D);

	glColor3d((double)(unsigned char)color[0] / 255.0, (double)(unsigned char)color[1] / 255.0, (double)(unsigned char)color[2] / 255.0);
	glLineWidth(h);

	glBegin(GL_LINES);

	glVertex3f(xa, ya, za);
	glVertex3f(xb, yb, zb);

	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void DrawLine(vec3 a, vec3 b, const char* color, float h)
{
	DrawLine(a.x, a.y, a.z, b.x, b.y, b.z, color, h);
}

void DrawTriangle(const vec3& p0, const vec3& p1, const vec3& p2, const vec4& color)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&DBG_MainCamera->ProjMx);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&DBG_MainCamera->ViewMx);

	glDisable(GL_TEXTURE_2D);

	glColor3d(color.r, color.g, color.b);
	glLineWidth(1);

	glBegin(GL_LINES);

	glVertex3f(p0.x, p0.y, p0.z);

	glVertex3f(p1.x, p1.y, p1.z);

	glVertex3f(p2.x, p2.y, p2.z);

	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void DrawSphere(const vec3& pos, float r, int lats, int longs)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&DBG_MainCamera->ProjMx);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&DBG_MainCamera->ViewMx);

	glDisable(GL_TEXTURE_2D);

	for(int i = 0; i <= lats; i++)
	{
		double lat0 = glm::pi<float>() * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = glm::pi<float>() * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(int j = 0; j <= longs; j++)
		{
			double lng = 2 * glm::pi<float>() * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f((x * zr0) + pos.x, (y * zr0) + pos.y, (z0)+pos.z);
			glVertex3f((r * x * zr0) + pos.x, (r * y * zr0) + pos.y, (r * z0) + pos.z);
			glNormal3f((x * zr1) + pos.x, (y * zr1) + pos.y, z1);
			glVertex3f((r * x * zr1) + pos.x, (r * y * zr1) + pos.y, (r * z1) + pos.z);
		}
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
}

void DrawBox(cCamera* pCamera, const vec3& pos, const vec3& halfs)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&pCamera->ProjMx);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&pCamera->ViewMx);

	glDisable(GL_TEXTURE_2D);

	glLineWidth(1);

	glBegin(GL_TRIANGLE_STRIP);

	float left = pos.x - halfs.x;
	float right = pos.x + halfs.x;
	float top = pos.y + halfs.y;
	float bottom = pos.y - halfs.y;
	float back = pos.z + halfs.z;
	float front = pos.z - halfs.z;

	vec3 g_vertex_buffer_data[] = {
		vec3(left, bottom, front),
		vec3(left, bottom, back),
		vec3(left, top, back),
		vec3(right, top, front),
		vec3(left, bottom, front),
		vec3(left, top, front),
		vec3(right, bottom, back),
		vec3(left, bottom, front),
		vec3(right, bottom, front),
		vec3(right, top, front),
		vec3(right, bottom, front),
		vec3(left, bottom, front),
		vec3(left, bottom, front),
		vec3(left, top, back),
		vec3(left, top, front),
		vec3(right, bottom, back),
		vec3(left, bottom, back),
		vec3(left, bottom, front),
		vec3(left, top, back),
		vec3(left, bottom, back),
		vec3(right, bottom, back),
		vec3(right, top, back),
		vec3(right, bottom, front),
		vec3(right, top, front),
		vec3(right, bottom, front),
		vec3(right, top, back),
		vec3(right, bottom, back),
		vec3(right, top, back),
		vec3(right, top, front),
		vec3(left, top, front),
		vec3(right, top, back),
		vec3(left, top, front),
		vec3(left, top, back),
		vec3(right, top, back),
		vec3(left, top, back),
		vec3(right, bottom, back)
	};

	glVertex3f(left, top, back);
	glVertex3f(right, top, back);

	glVertex3f(right, top, back);
	glVertex3f(right, top, front);

	glVertex3f(left, top, front);
	glVertex3f(right, top, front);

	glVertex3f(left, top, back);
	glVertex3f(left, top, front);

	glVertex3f(left, bottom, back);
	glVertex3f(right, bottom, back);

	glVertex3f(right, bottom, back);
	glVertex3f(right, bottom, front);

	glVertex3f(left, bottom, front);
	glVertex3f(right, bottom, front);

	glVertex3f(left, bottom, back);
	glVertex3f(left, bottom, front);
	//--
	glVertex3f(left, top, back);
	glVertex3f(left, bottom, back);

	glVertex3f(right, top, back);
	glVertex3f(right, bottom, back);

	glVertex3f(left, top, front);
	glVertex3f(left, bottom, front);

	glVertex3f(right, top, front);
	glVertex3f(right, bottom, front);


	/*for(int i = 0; i < 36; ++i)
	{
		glVertex3f(g_vertex_buffer_data[i].x, g_vertex_buffer_data[i].y, g_vertex_buffer_data[i].z);
	}*/
	/*glVertex3f(pos.x - halfs.x, pos.y + halfs.y, pos.z + halfs.z);
	glVertex3f(pos.x + halfs.x, pos.y + halfs.y, pos.z + halfs.z);
	glVertex3f(pos.x + halfs.x, pos.y + halfs.y, pos.z - halfs.z);
	glVertex3f(pos.x - halfs.x, pos.y + halfs.y, pos.z - halfs.z);

	glVertex3f(pos.x - halfs.x, pos.y - halfs.y, pos.z + halfs.z);
	glVertex3f(pos.x + halfs.x, pos.y - halfs.y, pos.z + halfs.z);
	glVertex3f(pos.x + halfs.x, pos.y - halfs.y, pos.z - halfs.z);
	glVertex3f(pos.x - halfs.x, pos.y - halfs.y, pos.z - halfs.z);*/

	glEnd();

	glEnable(GL_TEXTURE_2D);
}

vec3 GetAnyPerpendicularUnitVector(const vec3& vec)
{
	if(vec.y != 0.0f || vec.z != 0.0f)
		return vec3(1, 0, 0);
	else
		return vec3(0, 1, 0);
}

void DrawCapsule(cCamera* pCamera, const _Capsule& c, const vec4& color)
{
	const vec3 axis = c.end - c.start;
	const float     length = glm::length(axis);
	const vec3 localZ = axis / length;
	const vec3 localX = GetAnyPerpendicularUnitVector(localZ);
	const vec3 localY = cross(localZ, localX);

	//using cos;
	//using sin;
	//float pi = glm::pi<float>();

	const vec3 start(0.0f);
	const vec3 end(1.0f);
	const float     resolution = 16.0f;

	const vec3 step = (end - start) / resolution;

	auto cylinder = [localX, localY, localZ, c, length](const float u,
		const float v) {
		return c.start                                  //
			+ localX * cos(2.0f * glm::pi<float>() * u) * c.radius //
			+ localY * sin(2.0f * glm::pi<float>() * u) * c.radius //
			+ localZ * v * length;                   //

	};

	auto sphereStart = [localX, localY, localZ, c](const float u,
		const float v) -> vec3 {
		const float latitude = (glm::pi<float>() / 2.0f) * (v - 1);

		return c.start                                                  //
			+ localX * cos(2.0f * glm::pi<float>() * u) * cos(latitude) * c.radius //
			+ localY * sin(2.0f * glm::pi<float>() * u) * cos(latitude) * c.radius //
			+ localZ * sin(latitude) * c.radius;
	};

	auto sphereEnd = [localX, localY, localZ, c](const float u, const float v) {
		const float latitude = (glm::pi<float>() / 2.0f) * v;
		return c.end                                                    //
			+ localX * cos(2.0f * glm::pi<float>() * u) * cos(latitude) * c.radius //
			+ localY * sin(2.0f * glm::pi<float>() * u) * cos(latitude) * c.radius //
			+ localZ * sin(latitude) * c.radius;
	};

	for(float i = 0; i < resolution; ++i) {
		for(float j = 0; j < resolution; ++j) {
			const float u = i * step.x + start.x;
			const float v = j * step.y + start.y;

			const float un =
				(i + 1 == resolution) ? end.x : (i + 1) * step.x + start.x;
			const float vn =
				(j + 1 == resolution) ? end.y : (j + 1) * step.y + start.y;

			// Draw Cylinder
			{
				const vec3 p0 = cylinder(u, v);
				const vec3 p1 = cylinder(u, vn);
				const vec3 p2 = cylinder(un, v);
				const vec3 p3 = cylinder(un, vn);

				DrawTriangle(p0, p1, p2, color);
				DrawTriangle(p3, p1, p2, color);
			}

			// Draw Sphere start
			{
				const vec3 p0 = sphereStart(u, v);
				const vec3 p1 = sphereStart(u, vn);
				const vec3 p2 = sphereStart(un, v);
				const vec3 p3 = sphereStart(un, vn);
				DrawTriangle(p0, p1, p2, color);
				DrawTriangle(p3, p1, p2, color);
			}

			// Draw Sphere end
			{
				const vec3 p0 = sphereEnd(u, v);
				const vec3 p1 = sphereEnd(u, vn);
				const vec3 p2 = sphereEnd(un, v);
				const vec3 p3 = sphereEnd(un, vn);
				DrawTriangle(p0, p1, p2, color);
				DrawTriangle(p3, p1, p2, color);
			}
		}
	}
}

void DrawQuad(int tex_id)
{
	mat4 proj(1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&proj);

	mat4 model_view(1.0f);

	model_view = translate(model_view, vec3(0.7f, 0.7f, 0.0f));
	model_view = scale(model_view, vec3(0.25));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&model_view);

	glEnable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1 + tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glBegin(GL_POLYGON);

	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);

	glVertex3f(-1.0f,-1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);

	glVertex3f( 1.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);

	glEnd();
}

void DrawFrustum(cCamera* pCamera, const vec3& pos, float frustum[6][4])
{
	vec3 n(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&pCamera->ProjMx);

	mat4 model_view(1.0f);

	model_view = translate(model_view, pos);
	//model_view = scale(model_view, vec3(0.25));

	model_view = pCamera->ViewMx * model_view;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&model_view);

	GLboolean CullFaceStat = false;
	glGetBooleanv(GL_CULL_FACE, &CullFaceStat);
	glDisable(GL_CULL_FACE);

	vec3 ppos[4] = {
		vec3(-1.0f, 0.0f, 1.0f),
		vec3(-1.0f, 0.0f, -1.0f),
		vec3(1.0f, 0.0f, 1.0f),
		vec3(1.0f, 0.0f, -1.0f)
	};

	glBegin(GL_POLYGON);

	vec3 p;

	p = ppos[0];
	glVertex3f(p.x + frustum[0][0], p.y + frustum[0][1], p.z + frustum[0][2]);

	p = ppos[1];
	glVertex3f(p.x + frustum[0][0], p.y + frustum[0][1], p.z + frustum[0][2]);

	p = ppos[2];
	glVertex3f(p.x + frustum[0][0], p.y + frustum[0][1], p.z + frustum[0][2]);

	p = ppos[3];
	glVertex3f(p.x + frustum[0][0], p.y + frustum[0][1], p.z + frustum[0][2]);

	p = ppos[1];
	glVertex3f(p.x + frustum[0][0], p.y + frustum[0][1], p.z + frustum[0][2]);

	p = ppos[2];
	glVertex3f(p.x + frustum[0][0], p.y + frustum[0][1], p.z + frustum[0][2]);

	glEnd();

	if(CullFaceStat)glEnable(GL_CULL_FACE);
}
