#ifndef _SHADER_COMPILER_H_
#define _SHADER_COMPILER_H_

#include <engine/common/base_types.h>
#include <engine/shaders/shaders.h>
#include "shader_file.h"

class cShaderCompiler{
public:

	cList<shader_parser::_stritem*> Semantics;
	cList<shader_parser::_stritem*> PassParams;

	char* FileResBuff = 0;
	uint FileResSize = 0;

	_valuelist* ShaderValues = 0;
	uint ValuesNum = 0;

	cList<_vffunc_item*> FunctionsList;

	int parser_initialized = false;

	cShaderCompiler();
	
	int ReadSemantics(char* fdat, int fsize);
	int ReadPassParams(char* fdat, int fsize);
	int GetFunctionFromList(char* nameVS, char* nameFS, cList<shader_parser::_str_shader_item*>* shaders_list, _vffunc_item** pprettech);
	word CompileFuncOGL(char* enterpointVS, char* enterpointFS, _pass_file* pass_file, cList<shader_parser::_str_shader_item*>* shaders_list, int Compile);
	_shader_file* EnumShaderStructure(shader_parser::_str_tech_container* tech_container, cList<shader_parser::_str_shader_item*>* shaders_list);
	int GetOGLValueAU(uint programm, int method, c_shader_value* pValue, int* pFValueLocation);
	int ConfigurateShader(shaders_container* pShader, _shader_file* shader_file);

	bool LoadSemantics(char* SearchFrom);
	void FreeStrings();
	int CompileShader(char*, shaders_container* pShaderContainerRet);
};

#endif