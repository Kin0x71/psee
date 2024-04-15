#include "mesh_shader_controller.h"
#include <engine/common/exception.h>
#include "ShaderSemantics.h"

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

MSC::_link_value::_link_value(c_shader_value* ShaderValuePtr, void* refLinkedData, int* refLinkedElementsCount)
{
	shader_value_ptr = ShaderValuePtr;
	linked_data = refLinkedData;
	linked_elements_count = refLinkedElementsCount;
}

MSC::_link_value::_link_value(c_shader_value* ShaderValuePtr)
{
	shader_value_ptr = ShaderValuePtr;
}

MSC::_link_value::~_link_value()
{
	printf("cMeshShaderController::~_link_value\n");
}

MSC::_pass_item::_pass_item(int PassId, c_function* ShaderFunc)
{
	pass_id = PassId;
	shader_func = ShaderFunc;
}

MSC::_pass_item::~_pass_item()
{
	printf("cMeshShaderController::~_pass_item\n");
	linked_values.enumerate(
		[](_link_value* link_value){
			printf("    delete link_value:%p\n", link_value);
			delete link_value;
			return false;
		}
	);

	linked_values.free();
}

c_shader_value* MSC::_pass_item::get_shader_value_ptr(_VALUESEMANTIC_SPEC Semantic)
{
	for(int vi = 0; vi < shader_func->ValueNum; ++vi)
	{
		if(!shader_func->ppSaderValues[vi])
			throw exception_ValueNotFound(__FUNCTION__, __FILE__, __LINE__);

		if(shader_func->ppSaderValues[vi]->Semantic == Semantic)
		{
			return shader_func->ppSaderValues[vi];
		}
	}

	return 0;
}

MSC::_tech_item::_tech_item(int TechId, c_tech* ShaderTech)
{
	tech_id = TechId;
	shader_tech = ShaderTech;

	for(int pi = 0; pi < ShaderTech->PassNum; ++pi)
	{
		pass_list.push(new _pass_item(pi, ShaderTech->Pass[pi].Func));
	}
}

MSC::_tech_item::~_tech_item()
{
	printf("cMeshShaderController::~_tech_item\n");
	pass_list.enumerate(
		[](_pass_item* pass_item){
			delete pass_item;
			return false;
		}
	);

	pass_list.free();
}

MSC::_shader_item::_shader_item(shaders_container* pShader)
{
	pshader = pShader;
}

MSC::_shader_item::~_shader_item()
{
	printf("cMeshShaderController::~_shader_item\n");
	tech_list.enumerate(
		[](_tech_item* tech_item){
			delete tech_item;
			return false;
		}
	);

	tech_list.free();
}

c_tech* MSC::_shader_item::get_shader_teck_ptr(int TechId)
{
	return &pshader->Tech[TechId];
}

MSC::~cMeshShaderController()
{
	printf("~cMeshShaderController() shaders_list size:%d\n", shaders_list.count);
	shaders_list.enumerate(
		[](_shader_item* shader_item){
			delete shader_item;
			return false;
		}
	);

	shaders_list.free();

	printf("~cMeshShaderController() ok\n");
};

void MSC::AddShader(shaders_container* pShader)
{
	shaders_list.push(new _shader_item(pShader));
}

void MSC::AddTech(shaders_container* pShader, int TechID)
{
	shaders_list.reset_enumerator();
	while(_shader_item* shader_item = shaders_list.next())
	{
		if(shader_item->pshader == pShader)
		{
			shader_item->tech_list.push(new _tech_item(TechID, shader_item->get_shader_teck_ptr(TechID)));
		}
	}
}

bool MSC::AddValue(shaders_container* pShader, int TechID, int PassId, _VALUESEMANTIC_SPEC LinkedSemantic, void* refLinkedData, int* refLinkedElementsCount)
{
	shaders_list.reset_enumerator();
	while(_shader_item* shader_item = shaders_list.next())
	{
		if(shader_item->pshader != pShader)continue;

		shader_item->tech_list.reset_enumerator();
		while(_tech_item* tech_item = shader_item->tech_list.next())
		{
			if(tech_item->tech_id != TechID)continue;

			tech_item->pass_list.reset_enumerator();
			while(_pass_item* pass_item = tech_item->pass_list.next())
			{
				if(pass_item->pass_id != PassId)continue;

				c_shader_value* shader_value_ptr = pass_item->get_shader_value_ptr(LinkedSemantic);
				//printf("sem:%d shader_value_ptr:%x\n", LinkedSemantic, shader_value_ptr);
				if(shader_value_ptr){
					pass_item->linked_values.push(
						new _link_value(
							shader_value_ptr,
							refLinkedData,
							refLinkedElementsCount
						)
					);
				}
				else{
					printf("ERROR! value not found >>> LinkedSemantic:%s\n", ShaderSemantics::values[(int)LinkedSemantic].name);
				}

				return true;
			}
		}
	}

	return false;
}

void MSC::SetupValues()
{
	shaders_list.reset_enumerator();
	while(_shader_item* shader_item = shaders_list.next())
	{
		shader_item->tech_list.reset_enumerator();
		while(_tech_item* tech_item = shader_item->tech_list.next())
		{
			tech_item->pass_list.reset_enumerator();
			while(_pass_item* pass_item = tech_item->pass_list.next())
			{
				for(int vi = 0; vi < pass_item->shader_func->ValueNum; ++vi)
				{
					c_shader_value* target_val = pass_item->shader_func->ppSaderValues[vi];

					_VALUESEMANTIC_LEVEL ValLevel = ShaderSemantics::values[(int)pass_item->shader_func->ppSaderValues[vi]->Semantic].ValLevel;

					if(ValLevel == _VALUE_LEVEL_MODEL || ValLevel == _VALUE_LEVEL_MESH)
					{
						int linked_id = pass_item->linked_values.enumerate(
							[target_val](_link_value* link_value){
								if(link_value->shader_value_ptr == target_val)return true;
								return false;
							}
						);

						if(linked_id == -1){
							_link_value* new_link_value = new _link_value(target_val);
							pass_item->linked_values.push(new_link_value);
						}
					}
				}
			}
		}
	}
}

void MSC::UpdateValues()
{
	shaders_list.reset_enumerator();
	while(_shader_item* shader_item = shaders_list.next())
	{
		shader_item->tech_list.reset_enumerator();
		while(_tech_item* tech_item = shader_item->tech_list.next())
		{
			tech_item->pass_list.reset_enumerator();
			while(_pass_item* pass_item = tech_item->pass_list.next())
			{
				pass_item->linked_values.reset_enumerator();
				while(_link_value* link_value = pass_item->linked_values.next())
				{
					c_shader_value* shader_value = link_value->shader_value_ptr;

					if(shader_value){
						shader_item->pshader->Values[shader_value->GId].ShaderValuePtr = (float*)link_value->linked_data;

						shader_item->pshader->Values[shader_value->GId].CountSet = 1;

						if(link_value->linked_elements_count){
							shader_item->pshader->Values[shader_value->GId].CountSet = *link_value->linked_elements_count;
						}
					}
					else{
						printf("ERROR! empty link>>> set ptr:%p count:%d\n", link_value->linked_data, link_value->linked_elements_count);
					}
				}
			}
		}
	}
}