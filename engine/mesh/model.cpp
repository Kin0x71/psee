#include "model.h"
#include <engine/debug/primitives.h>
#include <game.h>

#ifdef _DEBUG
#include <engine/debug/sizeof_memory.hpp>
#include <engine/debug/primitives.h>
#endif

cModel::~cModel()
{
	printf("~cModel()\n");

	if(name){
		delete[] name;
	}

	mesh_list.reset_enumerator();
	while(cMesh* mesh = mesh_list.next())
	{
		printf("    delete mesh\n");
		delete mesh;
		printf("    delete mesh ok\n");
	}
	mesh_list.free();

	printf("~cModel() ok\n");
}

void cModel::set_name(const char* n)
{
	if(name)delete[] name;

	int nlen = strlen(n);
	name = new char[nlen + 1];
	memcpy(name, n, nlen);
	name[nlen] = 0;
}

void cModel::AddMesh(cMesh* pMesh)
{
	mesh_list.push(pMesh);
}

void cModel::SetupShaderValues(bool use_projviewworld, bool use_viewworld, bool use_world)
{
	mesh_list.reset_enumerator();
	while(cMesh* mesh = mesh_list.next())
	{
		mesh->shader_controller.shaders_list.reset_enumerator();
		while(MSC::_shader_item* shader_item = mesh->shader_controller.shaders_list.next())
		{
			shader_item->tech_list.reset_enumerator();
			while(MSC::_tech_item* tech_item = shader_item->tech_list.next())
			{
				tech_item->pass_list.reset_enumerator();
				while(MSC::_pass_item* pass_item = tech_item->pass_list.next())
				{
					if(use_projviewworld){
						mesh->shader_controller.AddValue(
							shader_item->pshader,
							tech_item->tech_id,
							pass_item->pass_id,
							_VALUESEMANTIC_SPEC::VALSEM_PROJVIEWWORLDMX,
							&ProjViewWorldMx
						);
					}

					if(use_viewworld){
						mesh->shader_controller.AddValue(
							shader_item->pshader,
							tech_item->tech_id,
							pass_item->pass_id,
							_VALUESEMANTIC_SPEC::VALSEM_VIEWWORLDMX,
							&ViewWorldMx
						);
					}

					if(use_world){
						mesh->shader_controller.AddValue(
							shader_item->pshader,
							tech_item->tech_id,
							pass_item->pass_id,
							_VALUESEMANTIC_SPEC::VALSEM_WORLDMX,
							&ModelWorldMx
						);
					}
				}
			}
		}

		mesh->shader_controller.SetupValues();
	}
}

void cModel::RenderModel(cCamera* pCamera, mat4* pworld_mx)
{
	//printf(">>> cModel::RenderModel(%s) %f %f\n", name, (*pworld_mx)[3][0], (*pworld_mx)[3][2]);

	mesh_list.reset_enumerator();
	while(cMesh* mesh = mesh_list.next())
	{
		//printf(">>> mesh->shader_controller.UpdateValues()\n");

		ModelWorldMx = (*pworld_mx) * mesh->mesh_localmx * model_localmx;
		ViewWorldMx = pCamera->ViewMx * ModelWorldMx;
		ProjViewWorldMx = pCamera->ProjViewMx * ModelWorldMx;

		mesh->shader_controller.UpdateValues();

		mesh->shader_controller.shaders_list.reset_enumerator();
		while(MSC::_shader_item* shader_item = mesh->shader_controller.shaders_list.next())
		{
			shader_item->tech_list.reset_enumerator();
			while(MSC::_tech_item* tech_item = shader_item->tech_list.next())
			{
				//printf(">>> shader_item->pshader->Begin(%d)\n", tech_item->tech_id);
				shader_item->pshader->Begin(mesh, tech_item->tech_id);
			}
		}
	}

	//DBG_DrawVertexes(pworld_mx);
}

void cModel::Update()
{
}

void cModel::GathereShadowDepth(mat4* pworld_mx, mat4* plightmx)
{
}

void cModel::DBG_DrawVertexes(mat4* pworld_mx)
{
	DrawCross(
		vec3((*pworld_mx)[3][0], (*pworld_mx)[3][1], (*pworld_mx)[3][2]),
		0.1f,
		"\xff\x00\x00"
	);

	for(int mi = 0; mi < mesh_list.count; ++mi)
	{
		cMesh* pMesh = mesh_list[mi];

		uchar* vert_item_offset = (uchar*)pMesh->vert_buff;

		for(uint vert_index = 0; vert_index < pMesh->vnum; ++vert_index)
		{
			//printf("\tvert_index:%d\n", vert_index);
			vec3* p_vpos = 0;
			vec3* p_vnormal = 0;
			vec3* p_vbinormal = 0;
			vec3* p_vtangent = 0;

			for(int pi = 0; pi < pMesh->vert_def.items_count; ++pi)
			{
				switch(pMesh->vert_def.use_items[pi]){
					case _VALUESEMANTIC_SPEC::VALSEM_VERTEX:
						p_vpos = (vec3*)vert_item_offset;
						break;

					case _VALUESEMANTIC_SPEC::VALSEM_NORMAL:
						p_vnormal = (vec3*)vert_item_offset;
						break;

					case _VALUESEMANTIC_SPEC::VALSEM_BINORMAL:
						p_vbinormal = (vec3*)vert_item_offset;
						break;

					case _VALUESEMANTIC_SPEC::VALSEM_TANGENT:
						p_vtangent = (vec3*)vert_item_offset;
						break;
				}

				vert_item_offset += ShaderSemantics::vert_def[(int)pMesh->vert_def.use_items[pi]].part_size;
			}

			vec3 vp = (*pworld_mx) * vec4(*p_vpos, 1.0f);
			vec3 vn = mat3(*pworld_mx) * (*p_vnormal);
			//vec3 vp = (vec3)*&(world_mx * (vec4(*p_vpos, 1.0f)))[3];
			DrawLine(
				vp,
				vp + vn * 0.5f,
				"\x00\x00\xff"
			);

			if(p_vbinormal){
				vec3 vb = mat3(*pworld_mx) * (*p_vbinormal);
				DrawLine(
					vp,
					vp + vb * 0.5f,
					"\x00\xff\x00"
				);
			}

			if(p_vtangent){
				vec3 vt = mat3(*pworld_mx) * (*p_vtangent);
				DrawLine(
					vp,
					vp + vt * 0.5f,
					"\xff\x00\x00"
				);
			}
		}
	}
}