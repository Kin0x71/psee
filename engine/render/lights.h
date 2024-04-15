#ifndef _LIGHTS_H_
#define _LIGHTS_H_

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <engine/textures/texture.h>

using namespace glm;

class cLights {
public:
	vec4* pos_cone = 0;
	vec4* color_power = 0;
	vec4* dir_spread = 0;
	bool* perspective_flag = 0;
	int count = 0;

	uint SHADOW_WIDTH = 1024;
	uint SHADOW_HEIGHT = 1024;
	uint* DepthMapsFBO = 0;
	uint* DepthMapsIds = 0;
	cTexture* DepthMapsTex = 0;
	mat4* LightSpaceMatrces = 0;

	float* light_fovs = 0;
	float* near_planes = 0;
	float* far_planes = 0;

	void allocate(int c);
	void release();
	void set_pos_cone(int id, const vec3& pos, float cone);
	void set_color_power(int id, const vec3& color, float power);
	void set_dir_spread(int id, const vec3& dir, float spread);
	void UpdateLightSpec();
	void DepthBuffersSetView();
	void DepthBuffersBindFBO(int id);
	void DepthBuffersUnbindFBO();
};

#endif