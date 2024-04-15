
#ifndef _SHADER_H_
#define _SHADER_H_

#include <engine/common/base_types.h>
#include "ShaderSemantics.h"
#include <engine/textures/texture.h>
#include <engine/common/list.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;

struct _vert_def_cont;
class cMesh;

struct _shader_values_level_item
{
	char *name;
	int id;
	_VALUESEMANTIC_SPEC sem;

	_shader_values_level_item()
	{
		name=0;
		id=-1;
		sem= _VALUESEMANTIC_SPEC::VALSEM_NON;
	}

	~_shader_values_level_item()
	{
		if(name){
			delete[] name;
			name = 0;
		}
	}
};

class  c_shader_value
{
public:
	int GId;
	char *Name;
	_VALUESEMANTIC_SPEC Semantic;
	uint RegisterType;
	uint Count;
	uint CountSet;
	void* ShaderValuePtr;

	c_shader_value()
	{
		GId = -1;
		Name = 0;
		Semantic = _VALUESEMANTIC_SPEC::VALSEM_NON;
		RegisterType = 0;
		Count = 0;
		ShaderValuePtr = 0;
		CountSet = 0;
	}

	~c_shader_value()
	{
		if(Name){
			delete[] Name;
			Name = 0;
		}
	}
};

class c_function
{
public:
	char* Name;
	int ValueNum;
	c_shader_value** ppSaderValues;
	int* pValueLocation;

	int programm;

	c_function()
	{
		Name = 0;
		ValueNum = 0;
		ppSaderValues = 0;
		pValueLocation = 0;
		programm = 0;
	}

	~c_function()
	{
		if(Name){
			delete[] Name;
			Name = 0;
		}

		if(ppSaderValues){
			delete[] ppSaderValues;
			ppSaderValues = 0;
		}

		if(pValueLocation){
			delete[] pValueLocation;
			pValueLocation = 0;
		}

		if(programm){
			glDeleteProgram(programm);
			programm = 0;
		}
	}
	
	bool ValuesUpdate(_vert_def_cont* vdc);
};

struct _render_stat
{
	uchar Stat;
	uchar Value;
};

class c_pass
{
public:
	char* Name;
	c_function* Func;
	
	int RenderStatNum;
	_render_stat* RenderStat;

	c_pass()
	{
		Name=0;
		Func=0;
		
		RenderStatNum=0;
		RenderStat=0;
	}

	~c_pass()
	{
		if(Name){
			delete[] Name;
			Name = 0;
		}

		//Func должен был быть удалён в ~shaders_container

		if(RenderStat){
			delete RenderStat;
			RenderStat = 0;
		}
	}

	bool Begin(_vert_def_cont* vdc);
};

class c_tech
{
public:
	char *Name;
	int PassNum;
	c_pass *Pass;

	c_tech()
	{
		Name=0;
		PassNum=0;
		Pass=0;
	}

	~c_tech()
	{
		if(Name){
			delete[] Name;
			Name = 0;
		}

		if(Pass){
			delete[] Pass;
			Pass = 0;
		}
	}

	bool Begin(cMesh* pmesh);
	bool BeginPoints(cMesh* pmesh);
//	bool SetVertexSize(uint VertexSize);
};

class shaders_container
{
public:
	cList<_shader_values_level_item*> valuesL_engine;
	cList<_shader_values_level_item*> valuesL_model;
	cList<_shader_values_level_item*> valuesL_mesh;

	char *Name;

	uint iVertevBuffer;
	uint iIndexBuffer;
	
	c_shader_value* Values;
	uint ValueNum;
	uint FunctionNum;
	c_function* Function;
	uint TechNum;
	c_tech* Tech;

	shaders_container()
	{
		Name=0;
		Values=0;
		ValueNum=0;
		FunctionNum=0;
		Function=0;
		TechNum=0;
		Tech=0;

		iVertevBuffer=0;
		iIndexBuffer=0;
	}

	~shaders_container()
	{
		if(Name){
			delete[] Name;
			Name = 0;
		}

		valuesL_engine.enumerate([](_shader_values_level_item* item)->bool{ delete item; return false; });
		valuesL_engine.free();

		valuesL_model.enumerate([](_shader_values_level_item* item)->bool{ delete item; return false; });
		valuesL_model.free();

		valuesL_mesh.enumerate([](_shader_values_level_item* item)->bool{ delete item; return false; });
		valuesL_mesh.free();

		if(Values){
			delete[] Values;
			Values = 0;
		}

		if(Function){
			delete[] Function;
			Function = 0;
		}

		if(Tech){
			delete[] Tech;
			Tech = 0;
		}
	}

	bool Begin(cMesh* pmesh, int tech_id);

	int RegisterValueLevel_Ex(cList<_shader_values_level_item*>* plevelvalues,char* name,int id,_VALUESEMANTIC_SPEC sem);
	int RegisterValueLevel(int level,char* name,int id,_VALUESEMANTIC_SPEC sem);
	
	//bool BeginPoints(int TechId,int vertnum,_VERTEXTYPE VertType);
	int GetValueByName(char* name);
	int GetValueByName(int tech, int pass, char* name);
	int GetValueBySemantic(_VALUESEMANTIC_SPEC valsem);
	int GetTechniqueByName(char* name);
	int GetPassByName(int tech, char* name);
	bool SetMatrix(uint id, mat4* pmx);
	//bool SetWeightsBuffer(uint id,int iw);
	bool SetVertexAttribPointer(uint id);
	bool SetFloat(uint id, float f);
	bool SetVector3(uint id, float x, float y, float z);
	bool SetInt(uint id, int i);
	bool SetVector(uint id, vec4* pvec);
	bool SetVector(uint id, vec3* pvec);
	bool SetVector(uint id, vec2* pvec);
	bool SetTexture(uint id, cTexture* ptexture);
	bool SetSampler(uint id, uint texid);
	bool SetVectorArray(uint id, vec4* pvec, uint count);
	bool SetVectorArray(uint id, vec3* pvec, uint count);
	bool SetVectorArray(uint id, vec2* pvec, uint count);
	bool SetMatrixArray(uint id, mat4* pmx, uint count);
	bool SetFloatArray(uint id, float* pf, uint count);
	bool SetTextureArray(uint id, cTexture* ptexture, uint count);
	bool SetSamplerArray(uint id, uint* psamplers, uint count);
};

#endif