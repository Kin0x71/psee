#ifndef _MODEL_SHADER_CONTROLLER_H_
#define _MODEL_SHADER_CONTROLLER_H_

#include "shaders.h"
#include <engine/common/list.h>
#include <engine/common/exception.h>
//#include <engine/mesh/mesh.h>

class cMeshShaderController{
public:
	struct _link_value
	{
		c_shader_value* shader_value_ptr = 0;
		void* linked_data = 0;
		int* linked_elements_count = 0;

		_link_value(c_shader_value* ShaderValuePtr, void* refLinkedData, int* refLinkedElementsCount = 0);
		_link_value(c_shader_value* ShaderValuePtr);
		~_link_value();
	};

	struct _pass_item
	{
		c_function* shader_func = 0;
		int pass_id = -1;
		cList<_link_value*> linked_values;

		_pass_item(int PassId, c_function* ShaderFunc);
		~_pass_item();
		c_shader_value* get_shader_value_ptr(_VALUESEMANTIC_SPEC Semantic);
	};

	struct _tech_item
	{
		c_tech* shader_tech = 0;
		int tech_id = -1;
		cList<_pass_item*> pass_list;

		_tech_item(int TechId, c_tech* ShaderTech);
		~_tech_item();
	};

	struct _shader_item
	{
		shaders_container* pshader = 0;
		cList<_tech_item*> tech_list;

		_shader_item(shaders_container* pShader);
		~_shader_item();
		c_tech* get_shader_teck_ptr(int TechId);
	};

	cList<_shader_item*> shaders_list;

	~cMeshShaderController();
	void AddShader(shaders_container* pShader);
	void AddTech(shaders_container* pShader, int TechID);
	bool AddValue(shaders_container* pShader, int TechID, int PassId, _VALUESEMANTIC_SPEC LinkedSemantic, void* refLinkedData, int* refLinkedElementsCount = 0);
	void SetupValues();
	void UpdateValues();
};

typedef cMeshShaderController MSC;

#endif