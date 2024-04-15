#ifndef _MODEL_H_
#define _MODEL_H_

#include "mesh.h"
#include <engine/common/list.h>
//#include <engine/shaders/mesh_shader_controller.h>
#include <engine/object/animation/skeleton_animation.h>
#include <engine/object/animation/animation.h>
#include <engine/camera.h>

class cModel{
public:

	cList<cMesh*> mesh_list;

	int defuse_textures_count = 0;
	int normalmap_textures_count = 0;
	cTexture** ppdefuse_textures;
	cTexture** ppnormalmap_textures;

	mat4 ModelWorldMx;
	mat4 ViewWorldMx;
	mat4 ProjViewWorldMx;

	mat4 model_localmx = mat4(1.0f);

	char* name = 0;

	~cModel();

	void set_name(const char* n);
	void AddMesh(cMesh* pMesh);
	void SetupShaderValues(bool use_projviewworld = true, bool use_viewworld = true, bool use_world = true);
	void RenderModel(cCamera* pCamera, mat4* pworld_mx);

	void Update();
	void GathereShadowDepth(mat4* pworld_mx, mat4* plightmx);

	void DBG_DrawVertexes(mat4* pworld_mx);
};

#endif