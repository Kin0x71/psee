#include "shader_compiler.h"
#include <engine/shaders/ShaderSemantics.h>
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <engine/common/exception.h>
#include <engine/common/xglobals.h>
#include <engine/common/gl_head.h>

cShaderCompiler::cShaderCompiler()
{
}

int cShaderCompiler::ReadSemantics(char* fdat, int fsize)
{
	int i = shader_parser::find_str(fdat, "_VALUESEMANTIC");

	int endenum = shader_parser::find_char('}', fdat, i, fsize) + i;

	for(;; i++)
	{
		int ti = shader_parser::find_char('V', fdat, i, fsize);

		if(ti == -1 || i >= endenum){
			break;
		}

		i = ti;

		if(fdat[i + 1] == 'A' && !strncmp("LSEM_", &fdat[i + 2], 5)){
			char sem[256];
			int ofs = 0;
			for(int j = i; j < fsize; j++)
			{
				char jc = fdat[j + 7];
				sem[ofs] = jc;
				if(jc == ',' || jc == ' ' || jc == '	' || jc == '=' || jc == '\r'){
					sem[ofs] = 0;
					break;
				}
				ofs++;
			}

			i += ofs + 7;
			//printf("ADD SEMANTIC:%s\n",sem);
			//AddStrInList(Semantics, sem);
			Semantics.push(new shader_parser::_stritem(sem));
		}
	}

	return i;
}

int cShaderCompiler::ReadPassParams(char* fdat, int fsize)
{
	int i = shader_parser::find_str(fdat, "_PASSPARAMETER");
	int endenum = shader_parser::find_char('}', fdat, i, fsize) + i;

	for(;; i++)
	{
		int ti = shader_parser::find_char('P', fdat, i, fsize);

		if(ti == -1 || ti + i >= endenum){
			break;
		}

		i = ti;

		if(fdat[i + 1] == 'A' && !strncmp("SSPARAM_", &fdat[i + 2], 8)){
			char par[256];
			int ofs = 0;
			for(int j = i; j < fsize; j++)
			{
				char jc = fdat[j + 10];
				par[ofs] = jc;
				if(jc == ',' || jc == ' ' || jc == '	' || jc == '=' || jc == '\r'){
					par[ofs] = 0;
					break;
				}
				ofs++;
			}

			i += ofs + 10;

			//AddStrInList(PassParams, par);
			PassParams.push(new shader_parser::_stritem(par));
		}
	}

	return i;
}

int cShaderCompiler::GetFunctionFromList(char* nameVS, char* nameFS, cList<shader_parser::_str_shader_item*>* shaders_list, _vffunc_item** ppret_vffunc)
{
	printf("GetFunctionFromList() shaders_list->count:%d\n", shaders_list->count);
	char funcnames[512];
	funcnames[0] = 0;

	if(nameVS)strcat(funcnames, nameVS);
	strcat(funcnames, "//");
	if(nameFS)strcat(funcnames, nameFS);

	int namelen = strlen(funcnames);

	int index = FunctionsList.enumerate(
		[funcnames, namelen](_vffunc_item* item){
			//printf("\t%s==%s\n", item->name, funcnames);
			if(namelen == item->namelen && !strncmp(item->name, funcnames, namelen)){
				return true;
			}

			return false;
		}
	);

	if(index != -1){
		return 0;
	}

	_vffunc_item* new_item = new _vffunc_item();

	new_item->id = FunctionsList.count;
	shader_parser::str_cpy(funcnames, &new_item->name);
	new_item->namelen = namelen;

	FunctionsList.push(new_item);

	if(nameVS && (new_item->str_shader_item_vs = GetProgremmFromNeme(nameVS, 'v', shaders_list)) == 0){
		return 0;
	}

	if(nameFS && (new_item->str_shader_item_fs = GetProgremmFromNeme(nameFS, 'f', shaders_list)) == 0){
		return 0;
	}

	(*ppret_vffunc) = new_item;

	return 1;
}

void shaderLog(int shader_id, char* func_name)
{
	GLchar info_log[1024];
	//glGetProgramInfoLog(shader_id, 1024, NULL, info_log);
	glGetShaderInfoLog(shader_id, 1024, NULL, info_log);

	printf("compile shader error : %s\n%s\n", func_name, info_log);
}

int checkCompileErrors(GLuint shader, bool PROGRAM, char* func_name)
{
	GLint success;
	GLchar infoLog[1024];
	if(!PROGRAM)
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			printf("ERROR::SHADER_COMPILATION_ERROR in %s\n%s\n -- --------------------------------------------------- --\n", func_name, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			printf("ERROR::PROGRAM_LINKING_ERROR in %s\n%s\n -- --------------------------------------------------- --\n", func_name, infoLog);
		}
	}

	return success;
}

word cShaderCompiler::CompileFuncOGL(char* enterpointVS, char* enterpointFS, _pass_file* pass_file, cList<shader_parser::_str_shader_item*>* shaders_list, int Compile)
{
	printf("CompileFuncOGL(%s,%s)\n", enterpointVS, enterpointFS);

	_vffunc_item* vffunc = 0;
	int fok = GetFunctionFromList(enterpointVS, enterpointFS, shaders_list, &vffunc);

	if(!fok){
		printf("!fok GetFunctionFromList(%s,%s)\n", enterpointVS, enterpointFS);
		if(!vffunc){
			printf("Compile shader error: function not found\n");
			//xMessageBox("Compile shader error: function not found\n", "error");
			exit(0);
		}
		if(vffunc)return vffunc->id;
		return -1;
	}

	if(!pass_file){
		printf("!pass_file\n");
		return -1;
	}

	int ShaderVS = 0;
	int ShaderFS = 0;

	if(enterpointVS){
		ShaderVS = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(ShaderVS, 1, (const char**)&vffunc->str_shader_item_vs->programm, NULL);
		glCompileShader(ShaderVS);

		if(!checkCompileErrors(ShaderVS, false, enterpointVS)){
			return 65535;
		}
	}

	//checkOpenGLerror("glShaderSource VS");

	if(enterpointFS){
		ShaderFS = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(ShaderFS, 1, (const char**)&vffunc->str_shader_item_fs->programm, NULL);
		glCompileShader(ShaderFS);

		if(!checkCompileErrors(ShaderVS, false, enterpointFS)){
			return 65535;
		}
	}

	//checkOpenGLerror("glShaderSource FS");

	uint Program = glCreateProgram();

	if(ShaderVS)glAttachShader(Program, ShaderVS);
	if(ShaderFS)glAttachShader(Program, ShaderFS);

	glLinkProgram(Program);
	//checkOpenGLerror("glLinkProgram");
	checkCompileErrors(Program, true, pass_file->name);

	int AttribCount = 0;
	int UnifbCount = 0;
	glGetProgramiv(Program, GL_ACTIVE_ATTRIBUTES, &AttribCount);
	glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &UnifbCount);

	vffunc->value_id_num = AttribCount + UnifbCount;
	vffunc->value_id = new word[vffunc->value_id_num];
	int value_id_ofs = 0;

	for(int i = 0; i < AttribCount; ++i)
	{
		uint bufSize = 256;
		int length = 0;
		int size = 0;
		uint type = 0;
		char name[256];

		glGetActiveAttrib(Program, i, bufSize, &length, &size, &type, name);

		_valuelist* value = 0;
		bool newvalue = GetAddValueFromList(ShaderValues, name, &value);

		vffunc->value_id[value_id_ofs++] = value->id;

		if(newvalue)
		{
			shader_parser::_str_val_list* val_item_vs = vffunc->str_shader_item_vs->get_val_item(name);
			shader_parser::_str_val_list* val_item_fs = vffunc->str_shader_item_fs->get_val_item(name);

			if(!val_item_vs && !val_item_fs){
				printf("ERROR (SHADER DETECT ATTRIBUTE VALUE)\n");
				printf("ATTRIBUTE VALUE	[%s]\n", name);
			}

			if(val_item_vs)
			{
				value->semantic = GetIdByStr(&Semantics, val_item_vs->sem)-1;
				value->type = 'A';

				//if(value->semantic == -1)value->semantic = 0;

				ValuesNum++;
			}

			if(val_item_fs)
			{
				value->semantic = GetIdByStr(&Semantics, val_item_fs->sem)-1;
				value->type = 'A';

				//if(value->semantic == -1)value->semantic = 0;

				ValuesNum++;
			}
		}
	}

	for(int i = 0; i < UnifbCount; ++i)
	{
		uint bufSize = 256;
		int length = 0;
		int size = 0;
		uint type = 0;
		char name[256];

		glGetActiveUniform(Program, i, bufSize, &length, &size, &type, name);

		_valuelist* value = 0;
		bool newvalue = GetAddValueFromList(ShaderValues, name, &value);

		vffunc->value_id[value_id_ofs++] = value->id;

		if(newvalue)
		{
			shader_parser::_str_val_list* val_item_vs = vffunc->str_shader_item_vs->get_val_item(name);
			shader_parser::_str_val_list* val_item_fs = vffunc->str_shader_item_fs->get_val_item(name);

			if(val_item_vs)
			{
				value->semantic = GetIdByStr(&Semantics, val_item_vs->sem)-1;
				value->type = 'U';

				//if(value->semantic == -1)value->semantic = 0;

				ValuesNum++;
			}

			if(val_item_fs)
			{
				value->semantic = GetIdByStr(&Semantics, val_item_fs->sem)-1;
				value->type = 'U';

				//if(value->semantic == -1)value->semantic = 0;

				ValuesNum++;
			}

			if(!val_item_vs && !val_item_fs)
			{
				//cout << "ERROR [valnum]\n";
			}
		}
	}

	if(!Compile)
	{
		vffunc->format = 0;
		vffunc->buff = (unsigned char*)Program;
		vffunc->size = 0;

	}else{

		int  binaryLength = 0;
		uint binaryFormat = 0;
		void* binary = 0;

		glGetProgramiv(Program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

		if(binaryLength)
		{
			binary = malloc(binaryLength);
			glGetProgramBinary(Program, binaryLength, NULL, &binaryFormat, binary);
		}
		else{
			//cout << "FATAL ERROR! glGetProgramBinary" << endl;
		}

		vffunc->format = binaryFormat;
		vffunc->buff = (unsigned char*)binary;
		vffunc->size = binaryLength;
	}

	//checkOpenGLerror("GathereFuncOGL end");
	return vffunc->id;
}

_shader_file* cShaderCompiler::EnumShaderStructure(shader_parser::_str_tech_container* tech_container, cList<shader_parser::_str_shader_item*>* shaders_list)
{
	_shader_file* shader_out = new _shader_file();

	shader_out->tech_num = tech_container->technum;
	shader_out->tech = new _tech_file[tech_container->technum];
	_tech_file* tech_out = shader_out->tech;

	shader_parser::_str_tech_list* curtech = tech_container->tech;
	for(int ti = 0; ti < tech_container->technum; ti++)
	{
		tech_out[ti].name_len = shader_parser::str_cpy((char*)curtech->name, &tech_out[ti].name);

		tech_out[ti].pass_num = curtech->passnum;

		tech_out[ti].pass = new _pass_file[curtech->passnum];
		_pass_file* pass_out = tech_out[ti].pass;

		uint Passes = 0;

		shader_parser::_pass_str* curpass = curtech->pass;
		for(int pi = 0; pi < curtech->passnum; ++pi)
		{
			pass_out[pi].name_len = shader_parser::str_cpy((char*)curpass->name, &pass_out[pi].name);

			shader_parser::_pass_str* pass_str = GetPassStruct(tech_container, ti, pi);

			if(pass_str)
			{
				unsigned char rsn = GetSizeRenderStruct(pass_str->renderstat);

				pass_out[pi].renderstat_num = rsn;
				pass_out[pi].renderstat = new _stat_file[rsn];
				_stat_file* rss = pass_out[pi].renderstat;

				shader_parser::_renderstat_str* rscur = pass_str->renderstat;
				for(int irs = 0; rscur->next; irs++)
				{
					rss[irs].stat = rscur->stat;
					rss[irs].value = rscur->value;
					rscur = rscur->next;
				}

				if(pass_str->VS_EnterPoint || pass_str->PS_EnterPoint)
				{
					pass_out[pi].function_Id = CompileFuncOGL(pass_str->VS_EnterPoint, pass_str->PS_EnterPoint, &pass_out[pi], shaders_list, false);

					if(pass_out[pi].function_Id == 0xffff){
						//ErrorCheck = true;
					}
				}
			}
			else{
			   //printf("	ERROR\n");
			   //printf("	ERROR\n");
			}

			curpass = curpass->next;
		}

		curtech = curtech->next;
	}

	return shader_out;
}

int cShaderCompiler::GetOGLValueAU(uint programm, int method, c_shader_value* pValue, int* pFValueLocation)
{
	int Count = 0;

	if(method){
		glGetProgramiv(programm, GL_ACTIVE_ATTRIBUTES, &Count);
	}
	else{
		glGetProgramiv(programm, GL_ACTIVE_UNIFORMS, &Count);
	}

	for(int ai = 0; ai < Count; ++ai)
	{
		uint bufSize = 256;
		int length = 0;
		int size = 0;
		uint type = 0;
		char name[256];

		if(method){
			glGetActiveAttrib(programm, ai, bufSize, &length, &size, &type, name);
		}
		else{
			glGetActiveUniform(programm, ai, bufSize, &length, &size, &type, name);
		}

		if(pValue && pValue->Name && name && !strcmp(pValue->Name, name))
		{
			pValue->Count = size;

			if(method){
				*pFValueLocation = glGetAttribLocation(programm, name);
			}
			else{
				*pFValueLocation = glGetUniformLocation(programm, name);
			}

			return 1;
		}

	}
	return 0;
}

int cShaderCompiler::ConfigurateShader(shaders_container* pShader, _shader_file* shader_file)
{
	printf("ConfigurateShader(%s)\n", pShader->Name);

	pShader->Values = new c_shader_value[shader_file->value_num];
	pShader->ValueNum = shader_file->value_num;

	_value_file* value_file = shader_file->value;
	c_shader_value* shaderValue = pShader->Values;

	for(uint i = 0; i < pShader->ValueNum; i++)
	{
		shaderValue[i].GId = i;

		shaderValue[i].Name = value_file[i].name;
		shaderValue[i].Semantic = (_VALUESEMANTIC_SPEC)value_file[i].semantic;

		if(value_file[i].type == 'A')
		{
			shaderValue[i].RegisterType = 1;
		}
		else{
			shaderValue[i].RegisterType = 0;
		}

		_VALUESEMANTIC_SPEC sem = (_VALUESEMANTIC_SPEC)shaderValue[i].Semantic;

		if(sem != _VALUESEMANTIC_SPEC::VALSEM_NON && sem != (_VALUESEMANTIC_SPEC)65535)
		{
			printf(">>> RegisterValueLevel(sem:%d valname:%s ValLevel:%d id:%d)\n",sem, shaderValue[i].Name, ShaderSemantics::values[(int)sem].ValLevel,i);
			pShader->RegisterValueLevel(ShaderSemantics::values[(int)sem].ValLevel, shaderValue[i].Name, i, sem);
		}
	}

	pShader->FunctionNum = shader_file->function_num;
	pShader->Function = new c_function[shader_file->function_num];

	_function_file* function_file = shader_file->function;
	c_function* Function = pShader->Function;

	for(uint fi = 0; fi < pShader->FunctionNum; fi++)
	{
		Function[fi].Name = function_file[fi].name;

		Function[fi].programm = (uint)function_file[fi].buff;

		Function[fi].ValueNum = function_file[fi].value_id_num;
		Function[fi].ppSaderValues = new c_shader_value* [Function[fi].ValueNum];
		Function[fi].pValueLocation = new int[Function[fi].ValueNum];

		c_shader_value** ppValue = Function[fi].ppSaderValues;
		int value_id_num = function_file[fi].value_id_num;
		unsigned short* value_id = function_file[fi].value_id;

		for(int vidi = 0; vidi < Function[fi].ValueNum; ++vidi)
		{
			ppValue[vidi] = &shaderValue[value_id[vidi]];

			if(ppValue[vidi]->RegisterType)
			{
				GetOGLValueAU(Function[fi].programm, 1, ppValue[vidi], &Function[fi].pValueLocation[vidi]);
			}
			else{
				GetOGLValueAU(Function[fi].programm, 0, ppValue[vidi], &Function[fi].pValueLocation[vidi]);
			}
		}

	}

	pShader->Tech = new c_tech[shader_file->tech_num];
	pShader->TechNum = shader_file->tech_num;

	_tech_file* tech_file = shader_file->tech;
	c_tech* Tech = pShader->Tech;

	for(uint it = 0; it < pShader->TechNum; ++it)
	{
		Tech[it].Name = tech_file[it].name;

		Tech[it].Pass = new c_pass[tech_file[it].pass_num];
		Tech[it].PassNum = tech_file[it].pass_num;

		_pass_file* pass_file = tech_file[it].pass;
		c_pass* Pass = Tech[it].Pass;

		for(int ip = 0; ip < Tech[it].PassNum; ++ip)
		{
			Pass[ip].Name = pass_file[ip].name;

			Pass[ip].RenderStatNum = pass_file[ip].renderstat_num;
			Pass[ip].RenderStat = new _render_stat[Pass[ip].RenderStatNum];

			for(uint ssi = 0; ssi < pass_file[ip].renderstat_num; ++ssi)
			{
				Pass[ip].RenderStat[ssi].Stat = pass_file[ip].renderstat[ssi].stat;
				Pass[ip].RenderStat[ssi].Value = pass_file[ip].renderstat[ssi].value;
			}

			uint function_Id = pass_file[ip].function_Id;

			if(function_Id < 0x7fff){
				Pass[ip].Func = &Function[function_Id];
			}
			else if(function_Id > 0x7fff && function_Id < 0xffff){
				Pass[ip].Func = 0;
			}
		}
	}

	return 0;
}

bool cShaderCompiler::LoadSemantics(char* SearchFrom)
{
	char fname[1024];
	int sflen = strlen(SearchFrom);
	strcpy(fname, SearchFrom);

	strcpy(&fname[sflen], "engine/shaders/ShaderSemantics.h");
	printf("%s\n", fname);
	if(xIsFileExist(fname))goto L_pora;

	strcpy(&fname[sflen], "../engine/shaders/ShaderSemantics.h");

	if(xIsFileExist(fname))goto L_pora;

	printf("Semantics File not found (search from \"%s\")\n", SearchFrom);
	char cur_dir[1024];
	GetCurrentDirectoryA(1024, cur_dir);
	printf("CurrentDirectory (\"%s\")\n", cur_dir);
	return false;
L_pora:;

	FILE* fp = fopen(fname, "rb");
	if(!fp){
		printf("Semantics File Open Error (%s)\n", fname);
		return false;
	}

	printf("LoadSemantics from %s\n", fname);

	fseek(fp, 0, SEEK_END);

	int fsize = ftell(fp);
	fseek(fp, 0, 0);

	char* fdat = new char[fsize + 1];

	fread(fdat, 1, fsize, fp);

	fclose(fp);

	fdat[fsize] = 0;
	int lastofs = ReadSemantics(fdat, fsize);

	shader_parser::InitParser();

	ReadPassParams(&fdat[lastofs], fsize - lastofs);

	parser_initialized = true;

	return true;
}

void cShaderCompiler::FreeStrings()
{
	Semantics.enumerate([](shader_parser::_stritem* item){delete[] item->str; return false; });
	Semantics.free();

	PassParams.enumerate([](shader_parser::_stritem* item){delete[] item->str; return false; });
	PassParams.free();

	parser_initialized = false;
}

int cShaderCompiler::CompileShader(char* ShaderPath, shaders_container* pShaderContainerRet)
{
	printf("cShaderCompiler::CompileShader: %s\n", ShaderPath);

	if(!parser_initialized){
		throw exception_box(__FUNCTION__, __FILE__, __LINE__, "shader parser not initialized.");
	}

	if(glCreateShader == 0)
	{
		GLenum glew_status = glewInit();
		if(GLEW_OK != glew_status)
		{
			throw exception_box(__FUNCTION__, __FILE__, __LINE__, "Error glewInit: %s\n", glewGetErrorString(glew_status));
			return 1;
		}
	}

	FILE* fp = fopen(ShaderPath, "rb");
	if(!fp){
		throw exception_box(__FUNCTION__, __FILE__, __LINE__, "CompileShader(%s): File Open Error\n", ShaderPath);
	}

	fseek(fp, 0, SEEK_END);

	int fsize = ftell(fp);
	fseek(fp, 0, 0);

	char* fdat = new char[fsize + 1];

	fread(fdat, 1, fsize, fp);

	fclose(fp);

	fdat[fsize] = 0;

	FileResBuff = fdat;
	FileResSize = fsize;

	for(int i = 0; i < fsize; ++i)
	{
		if(fdat[i] == 9){ fdat[i] = 32; }
	}

	int last_ofs = 0;
	cList<shader_parser::_str_shader_item*>* sShaderList = shader_parser::GatherOGLProgramms(fdat, fsize, 0, &last_ofs);
	if(!sShaderList){
		printf("error GatherOGLProgramms\n");
		return 0;
	}

	shader_parser::_str_tech_container* sTechContainer = shader_parser::GatherOGLTech(fdat, last_ofs, fsize);
	if(!sTechContainer){
		printf("error GatherOGLTech\n");
		return 0;
	}

	ShaderValues = new _valuelist();
	ValuesNum = 0;

	_shader_file* shader_file = EnumShaderStructure(sTechContainer, sShaderList);

	shader_file->value = new _value_file[ValuesNum];
	_value_file* value_out = shader_file->value;

	_valuelist* Values_cur = ShaderValues;

	uint vi = 0;
	for(; vi < ValuesNum && Values_cur->next; vi++)
	{
		value_out[vi].name_len = shader_parser::str_cpy(Values_cur->name, &value_out[vi].name);
		value_out[vi].semantic = Values_cur->semantic;
		value_out[vi].type = Values_cur->type;
		Values_cur = Values_cur->next;
	}

	shader_file->value_num = vi;

	shader_file->function_num = FunctionsList.count;

	shader_file->function = new _function_file[shader_file->function_num];
	_function_file* function_file = shader_file->function;

	uint fi = 0;
	auto enumerator = [&fi, function_file](_vffunc_item* item){
		function_file[fi].name_len = shader_parser::str_cpy(item->name, &function_file[fi].name);

		function_file[fi].value_id_num = item->value_id_num;
		function_file[fi].value_id = item->value_id;

		function_file[fi].format = item->format;
		function_file[fi].size = item->size;
		function_file[fi].buff = item->buff;
		++fi;
		return false;
	};

	printf("FunctionsList.count:%d ENUMERATE\n", FunctionsList.count);
	FunctionsList.enumerate(enumerator);

	if(pShaderContainerRet->Name){
		delete[] pShaderContainerRet->Name;
	}

	int nlen = strlen(ShaderPath);
	pShaderContainerRet->Name = new char[nlen + 1];
	memcpy(pShaderContainerRet->Name, ShaderPath, nlen + 1);

	//--------------------------------
	//shader_file->value_num
	//--------------------------------

	ConfigurateShader(pShaderContainerRet, shader_file);

	sShaderList->enumerate([](shader_parser::_str_shader_item* item){
		delete item;
		return false;
	});

	delete sShaderList;
	delete sTechContainer;

	delete ShaderValues;

	FunctionsList.enumerate([](_vffunc_item* item){
		delete item;
		return false;
	});

	FunctionsList.free();

	delete[] fdat;

	delete shader_file;

	return 0;
}