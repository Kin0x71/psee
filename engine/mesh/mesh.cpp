#include "mesh.h"
#include <engine/common/gl_head.h>
#include <engine/textures/texture.h>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

cMesh::cMesh()
{
}

cMesh::~cMesh()
{
	printf("~cMesh()\n");

	if(name){
		delete[] name;
	}

	if(index_buff){
		delete index_buff;
	}

	if(vert_buff){
		free(vert_buff);
	}

	if(wbuff){
		delete wbuff;
	}
}

void cMesh::set_name(const char* n)
{
	if(name)delete[] name;

	int nlen = strlen(n);
	name = new char[nlen + 1];
	memcpy(name, n, nlen);
	name[nlen] = 0;
}

void cMesh::init_vertex_def_container(_VALUESEMANTIC_SPEC item_types[], int items_count, _vert_def_cont* ret_vert_def)
{
	//printf("init_vertex_def_container ret_vert_def:%x items_count:%d\n", ret_vert_def, items_count);

	ret_vert_def->items_count = items_count;

	int viofs = 0;
	for(int i = 0; i < items_count; ++i)
	{
		ret_vert_def->use_items[i] = item_types[i];
		ret_vert_def->item_offsets[(int)item_types[i]] = viofs;
		viofs += ShaderSemantics::vert_def[(int)item_types[i]].part_size;

		//printf("item item_types[%d]=%d offset=%d %x\n", i, item_types[i], ret_vert_def->item_offsets[item_types[i]], ret_vert_def->item_offsets[item_types[i]]);
	}

	//printf("struct_size=%d\n", viofs);

	ret_vert_def->struct_size = viofs;
}

void cMesh::init_indexes(cMesh* pmesh, word* indexes, int indexes_count)
{
	glGenBuffers(1, &pmesh->IGLBidI);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmesh->IGLBidI);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes_count * sizeof(word), indexes, GL_STATIC_DRAW);
}

void cMesh::init_vertexes(cMesh* pmesh, void* vertexes, int verts_count)
{
	glGenBuffers(1, &pmesh->IGLBidV);
	glBindBuffer(GL_ARRAY_BUFFER, pmesh->IGLBidV);
	glBufferData(GL_ARRAY_BUFFER, pmesh->vert_def.struct_size * verts_count, vertexes, GL_STATIC_DRAW);

	float min_x = 999999.0f;
	float max_x = -999999.0f;
	float min_y = 999999.0f;
	float max_y = -999999.0f;
	float min_z = 999999.0f;
	float max_z = -999999.0f;

	uchar* pvertoffs = (uchar*)vertexes;
	for(int vi = 0; vi < verts_count; ++vi)
	{
		for(int i = 0; i < pmesh->vert_def.items_count; ++i)
		{
			if(pmesh->vert_def.use_items[i] == _VALUESEMANTIC_SPEC::VALSEM_VERTEX)
			{
				vec3* pvpos = (vec3*)pvertoffs;
				//printf("v:%f,%f,%f\n", pvpos->x, pvpos->y, pvpos->z);

				if(pvpos->x < min_x)min_x = pvpos->x;
				if(pvpos->x > max_x)max_x = pvpos->x;

				if(pvpos->y < min_y)min_y = pvpos->y;
				if(pvpos->y > max_y)max_y = pvpos->y;

				if(pvpos->z < min_z)min_z = pvpos->z;
				if(pvpos->z > max_z)max_z = pvpos->z;
			}
			pvertoffs += ShaderSemantics::vert_def[(int)pmesh->vert_def.use_items[i]].part_size;
		}
	}

	//printf("[%f:%f] [%f:%f] [%f:%f]\n", min_x, max_x, min_y, max_y, min_z, max_z);

	pmesh->view_box[0] = vec3(min_x, max_y, min_z);
	pmesh->view_box[1] = vec3(max_x, max_y, min_z);
	pmesh->view_box[2] = vec3(max_x, max_y, max_z);
	pmesh->view_box[3] = vec3(min_x, max_y, max_z);
	pmesh->view_box[4] = vec3(min_x, min_y, min_z);
	pmesh->view_box[5] = vec3(max_x, min_y, min_z);
	pmesh->view_box[6] = vec3(max_x, min_y, max_z);
	pmesh->view_box[7] = vec3(min_x, min_y, max_z);

	pmesh->view_centre = vec3(
		min_x + ((max_x - min_x) / 2.0f),
		min_y + ((max_y - min_y) / 2.0f),
		min_z + ((max_z - min_z) / 2.0f)
	);

	for(int p = 0; p < 8; ++p)
	{
		pmesh->view_radius = max(pmesh->view_radius, distance(pmesh->view_centre, pmesh->view_box[p]));
	}
}

void cMesh::init_vertexes(cMesh* pmesh, void* vertexes, int verts_count, vec4* blend_indices, int weights_size)
{
	pmesh->is_skinned = true;

	glGenBuffers(1, &pmesh->IGLBidV);
	glBindBuffer(GL_ARRAY_BUFFER, pmesh->IGLBidV);
	glBufferData(GL_ARRAY_BUFFER, pmesh->vert_def.struct_size * verts_count, vertexes, GL_STATIC_DRAW);

	uint WeightsTexId = 0;
	glGenTextures(1, &WeightsTexId);

	pmesh->weights_size = weights_size;

	glBindTexture(GL_TEXTURE_2D, WeightsTexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, weights_size, 1, 0, GL_RGBA, GL_FLOAT, blend_indices);

	//checkOpenGLerror("CREATE WEIGHTS TEXTURE BUFFER");

	glBindTexture(GL_TEXTURE_2D, 0);

	pmesh->WeightsTexId = WeightsTexId;

	float min_x = 999999.0f;
	float max_x = -999999.0f;
	float min_y = 999999.0f;
	float max_y = -999999.0f;
	float min_z = 999999.0f;
	float max_z = -999999.0f;

	uchar* pvertoffs = (uchar*)vertexes;
	for(int vi = 0; vi < verts_count; ++vi)
	{
		for(int i = 0; i < pmesh->vert_def.items_count; ++i)
		{
			if(pmesh->vert_def.use_items[i] == _VALUESEMANTIC_SPEC::VALSEM_VERTEX)
			{
				vec3* pvpos = (vec3*)pvertoffs;
				//printf("v:%f,%f,%f\n", pvpos->x, pvpos->y, pvpos->z);

				if(pvpos->x < min_x)min_x = pvpos->x;
				if(pvpos->x > max_x)max_x = pvpos->x;

				if(pvpos->y < min_y)min_y = pvpos->y;
				if(pvpos->y > max_y)max_y = pvpos->y;

				if(pvpos->z < min_z)min_z = pvpos->z;
				if(pvpos->z > max_z)max_z = pvpos->z;
			}
			pvertoffs += ShaderSemantics::vert_def[(int)pmesh->vert_def.use_items[i]].part_size;
		}
	}

	//printf("[%f:%f] [%f:%f] [%f:%f]\n", min_x, max_x, min_y, max_y, min_z, max_z);

	pmesh->view_box[0] = vec3(min_x, max_y, min_z);
	pmesh->view_box[1] = vec3(max_x, max_y, min_z);
	pmesh->view_box[2] = vec3(max_x, max_y, max_z);
	pmesh->view_box[3] = vec3(min_x, max_y, max_z);
	pmesh->view_box[4] = vec3(min_x, min_y, min_z);
	pmesh->view_box[5] = vec3(max_x, min_y, min_z);
	pmesh->view_box[6] = vec3(max_x, min_y, max_z);
	pmesh->view_box[7] = vec3(min_x, min_y, max_z);

	pmesh->view_centre = vec3(
		min_x + ((max_x - min_x) / 2.0f),
		min_y + ((max_y - min_y) / 2.0f),
		min_z + ((max_z - min_z) / 2.0f)
	);

	for(int p = 0; p < 8; ++p)
	{
		pmesh->view_radius = max(pmesh->view_radius, distance(pmesh->view_centre, pmesh->view_box[p]));
	}
}

void cMesh::calc_binormal_tangent(vec3& vn, vec3* pvb, vec3* pvt)
{
	vec3 vA;
	vec3 vB;
	vec3 c1 = vn;
	vec3 c2 = vn;
	c1 = cross(c1,vec3(1.0, 0.0, 0.0));
	c2 = cross(c2,vec3(0.0, 0.0, 1.0));

	if(c1.length() > c2.length())
	{
		vA = c1;
	}
	else
	{
		vA = c2;
	}

	vA=normalize(vA);

	vB = vn;
	vB = cross(vB,vA);
	vB = normalize(vB);

	*pvb = vA;

	*pvb = vB;
}