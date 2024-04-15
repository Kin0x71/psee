#include "lights.h"
#include <glm/gtc/matrix_transform.hpp>

void cLights::allocate(int c)
{
	count = c;
	pos_cone = new vec4[count];
	color_power = new vec4[count];
	dir_spread = new vec4[count];
	perspective_flag = new bool[count];

	DepthMapsFBO = new uint[count];
	DepthMapsIds = new uint[count];
	DepthMapsTex = new cTexture[count];
	LightSpaceMatrces = new mat4[count];

	light_fovs = new float[count];
	near_planes = new float[count];
	far_planes = new float[count];

	glGenFramebuffers(count, DepthMapsFBO);
	glGenTextures(count, DepthMapsIds);

	for(int li = 0; li < count; ++li)
	{
		glBindTexture(GL_TEXTURE_2D, DepthMapsIds[li]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glBindFramebuffer(GL_FRAMEBUFFER, DepthMapsFBO[li]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthMapsIds[li], 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DepthMapsIds[li], 0);
		//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapsIds[li], 0);

		//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		//glDrawBuffers(1, DrawBuffers);

		DepthMapsTex[li].textureID = DepthMapsIds[li];

		light_fovs[li] = 56.0f;
		near_planes[li] = 10.0f;
		far_planes[li] = 100.0f;

		perspective_flag[li] = false;
	}

	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cLights::release()
{
	glDeleteBuffers(count, DepthMapsFBO);
	glDeleteTextures(count, DepthMapsIds);

	for(int li = 0; li < count; ++li)
	{
		DepthMapsTex[li].textureID = 0;
	}

	delete[] pos_cone;
	delete[] pos_cone;
	delete[] color_power;
	delete[] dir_spread;
	delete[] perspective_flag;

	delete[] DepthMapsFBO;
	delete[] DepthMapsIds;
	delete[] DepthMapsTex;
	delete[] LightSpaceMatrces;

	delete[] near_planes;
	delete[] far_planes;
	delete[] light_fovs;
}

void cLights::set_pos_cone(int id, const vec3& pos, float cone)
{
	pos_cone[id].x = pos.x;
	pos_cone[id].y = pos.y;
	pos_cone[id].z = pos.z;
	pos_cone[id].w = cone;
}

void cLights::set_color_power(int id, const vec3& color, float power)
{
	color_power[id].x = color.x;
	color_power[id].y = color.y;
	color_power[id].z = color.z;
	color_power[id].w = power;
}

void cLights::set_dir_spread(int id, const vec3& dir, float spread)
{
	dir_spread[id].x = dir.x;
	dir_spread[id].y = dir.y;
	dir_spread[id].z = dir.z;
	dir_spread[id].w = spread;
}

void cLights::UpdateLightSpec()
{
	for(int li = 0; li < count; ++li)
	{
		mat4 light_proj;

		if(perspective_flag[li]){
			light_proj = perspective(radians(light_fovs[li]), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_planes[li], far_planes[li]);
		}
		else{
			light_proj = ortho(-light_fovs[li] / 2.0f, light_fovs[li] / 2.0f, -light_fovs[li] / 2.0f, light_fovs[li] / 2.0f, near_planes[li], far_planes[li]);
		}


		vec3 pos(pos_cone[li].x, pos_cone[li].y, pos_cone[li].z);
		vec3 dir(0.0f, -1.0f, 0.0f);

		if(pos_cone[li].w != -1.0f){
			dir = vec3(dir_spread[li].x, dir_spread[li].y, dir_spread[li].z);
		}

		mat4 light_view = lookAt(pos, pos + dir, vec3(0.0f, 1.0f, 0.0f));
		//mat4 light_view = lookAt(vec3(), dir, vec3(0.0f, 1.0f, 0.0f));

		LightSpaceMatrces[li] = light_proj * light_view;
	}
}

void cLights::DepthBuffersSetView()
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cLights::DepthBuffersBindFBO(int id)
{
	glBindFramebuffer(GL_FRAMEBUFFER, DepthMapsFBO[id]);
}

void cLights::DepthBuffersUnbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}