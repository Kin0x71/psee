#ifndef _MESH_H_
#define _MESH_H_
//#include <math/vector.h>
//#include <math/matrix.h>
//#include <textures/texture.h>
#include <engine/common/exception.h>
//#include <engine/shaders/shaders.h>
#include <engine/shaders/mesh_shader_controller.h>
#include "mesh_defines.h"

class cMesh {
public:

	_vert_def_cont vert_def;

	uint IGLBidV = 0;
	uint IGLBidI = 0;

	uint vnum = 0;
	uint inum = 0;
	uint fnum = 0;

	mat4 mesh_localmx = mat4(1.0f);

	//uint* wofs = 0;
	float weights_size = 0.0f;
	//uint wsize = 0;
	//vec4* BlendIndices = 0;
	uint WeightsTexId = 0;

	char* name = 0;

	int DefuseTextureId = -1;
	int NormalmapTextureId = -1;

	word* index_buff = 0;
	void* vert_buff = 0;
	vec4* wbuff = 0;

	cMeshShaderController shader_controller;

	vec3 view_box[8] = {
		vec3(-9999.0f,9999.0f,-9999.0f),vec3(9999.0f,9999.0f,-9999.0f),vec3(9999.0f,9999.0f,9999.0f),vec3(-9999.0f,9999.0f,9999.0f),
		vec3(-9999.0f,-9999.0f,-9999.0f),vec3(9999.0f,-9999.0f,-9999.0f),vec3(9999.0f,-9999.0f,9999.0f),vec3(-9999.0f,-9999.0f,9999.0f),
	};

	vec3 view_centre = vec3(0.0f, 0.0f, 0.0f);
	float view_radius = 0.0f;

	bool is_in_frustum = false;

	bool is_skinned = false;
	bool is_cast_shadow = false;

	cMesh();
	~cMesh();

	void set_name(const char* n);

	static void init_vertex_def_container(_VALUESEMANTIC_SPEC item_types[], int items_count, _vert_def_cont* ret_vert_def);

	static void init_indexes(cMesh* pmesh, word* indexes, int indexes_count);
	static void init_vertexes(cMesh* pmesh, void* vertexes, int verts_count);
	static void init_vertexes(cMesh* pmesh, void* vertexes, int verts_count, vec4* blend_indices, int weights_size);
	static void calc_binormal_tangent(vec3& vn, vec3* pvb, vec3* pvt);
};

#endif