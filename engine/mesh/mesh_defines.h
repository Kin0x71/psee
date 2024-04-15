#ifndef _MESH_DEFINES_H_
#define _MESH_DEFINES_H_

#define MODEL_FILE_COLLISION_MESH 0x1

#define MESH_FILE_SKIN 0x1
#define MESH_FILE_VERTEXT_COLOR 0x2

#include <engine/common/base_types.h>
#include <engine/shaders/ShaderSemantics.h>

struct _vert_def_cont {
	uint item_offsets[9];
	_VALUESEMANTIC_SPEC use_items[9];
	int items_count = 0;
	int struct_size = 0;
};

struct _MESH_VERTEX_WEIGHTS
{
	unsigned short int bonenum = 0;
	unsigned short int* boneids = 0;
	float* weights = 0;
};

#endif