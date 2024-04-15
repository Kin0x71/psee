

#include "shader_file.h"

_file_version _CHECK_FILEVER_SHADER;

int _stat_list_file::reade_file(_FILE* pf)
{
	FileRead(&Register,4,pf);
	FileRead(&stat_num,4,pf);

	stat=new _stat_file[stat_num];

	for(int i=0;i<stat_num;i++)
	{
		FileRead(&stat[i].stat,4,pf);
		FileRead(&stat[i].value,4,pf);
	}

	return 0;
}

int _value_file::reade_file(_FILE* pf)
{
	FileRead(&name_len,1,pf);

	name=new char[name_len+1];

	FileRead(name,name_len,pf);
	name[name_len]=0;

	FileRead(&semantic,2,pf);

	if(semantic==0xffff)semantic=0;

	return 0;
}

int _function_file::read_file(_FILE* pf)
{
	FileRead(&name_len,1,pf);

	name=new char[name_len+1];
	FileRead(name,name_len,pf);
	name[name_len]=0;

	FileRead(&value_id_num,2,pf);

	value_id=new word[value_id_num];
	FileRead(value_id,value_id_num*2,pf);

	/*printf("value_id_num:[%d]\n",value_id_num);
	for(int i=0;i<value_id_num;++i)
	{
		printf("	[%d]\n",value_id[i]);
	}*/

	FileRead(&format,4,pf);
	FileRead(&size,4,pf);

	//printf("%s %d\n",name,size);
	buff=new unsigned char[size];
	FileRead(buff,size,pf);

	return 0;
}

int _pass_file::reade_file(_FILE *pf)
{
	FileRead(&name_len,1,pf);
	name=new char[name_len+1];
	FileRead(name,name_len,pf);
	name[name_len]=0;

	//printf("%s\n",name);

	FileRead(&function_Id,2,pf);

	FileRead(&RenderTarget,2,pf);

	FileRead(&renderstat_num,1,pf);

	if(renderstat_num)
	{
		renderstat=new _stat_file[renderstat_num];

		for(int i=0;i<renderstat_num;i++)
		{
			FileRead(&renderstat[i].stat,4,pf);
			FileRead(&renderstat[i].value,4,pf);
		}
	}

	FileRead(&samplestat_num,1,pf);

	if(samplestat_num)
	{
		samplestat=new _stat_list_file[samplestat_num];

		for(int i=0;i<samplestat_num;i++)
		{
			samplestat[i].reade_file(pf);
		}
	}

	return 0;
}

int _tech_file::read_file(_FILE* pf)
{
	FileRead(&name_len,1,pf);
	name=new char[name_len+1];
	FileRead(name,name_len,pf);
	name[name_len]=0;

	FileRead(&pass_num,1,pf);

	pass=new _pass_file[pass_num];

	//printf("%s\n",name);

	return 0;
}

int _shader_file::read_file(char* fname)
{
	_FILE* pf = files::pFilesObject->GetFile(fname);
	
	if(pf==0)return 0;

	_file_version checkfile={0,0,0,0,0,0,0};
				
	FileRead(checkfile.format,4,pf);
	FileRead(&checkfile.ver,4,pf);

	if(!_CHECK_FILEVER_SHADER.cmp(&checkfile)){
		printf("ERROR check version in file %s\n",fname);
		return 0;
	}
		
	FileRead(&value_num,4,pf);
	FileRead(&function_num,4,pf);
	FileRead(&tech_num,4,pf);
			
	if(tech_num<1 || (int)value_num<0 || (int)function_num<0){
		printf("ReadFile Error (%s)\n",fname);
		return -1;
	}

	value=new _value_file[value_num];
	function=new _function_file[function_num];
	tech=new _tech_file[tech_num];

	for(uint vi=0;vi<value_num;++vi)
	{
		value[vi].reade_file(pf);
	}

	for(uint fi=0;fi<function_num;++fi)
	{
		function[fi].read_file(pf);
	}
		
	for(uint ti=0;ti<tech_num;++ti)
	{
		tech[ti].read_file(pf);

		_pass_file* pass=tech[ti].pass;

		for(int pi=0;pi<tech[ti].pass_num;++pi)
		{
			pass[pi].reade_file(pf);
		}
	}
		
	return 0;
}

int SF_Value_reade_file(_FILE* pf,_value_file* pvf)
{
	FileRead(&pvf->name_len,1,pf);

	pvf->name=new char[pvf->name_len+1];

	FileRead(pvf->name,pvf->name_len,pf);
	pvf->name[pvf->name_len]=0;

	FileRead(&pvf->semantic,2,pf);

	if(pvf->semantic==0xffff)pvf->semantic=0;

	return 0;
}

int SF_Function_read_file(_FILE* pf,_function_file* pff)
{
	FileRead(&pff->name_len,1,pf);

	pff->name=new char[pff->name_len+1];
	FileRead(pff->name,pff->name_len,pf);
	pff->name[pff->name_len]=0;

	FileRead(&pff->value_id_num,2,pf);

	pff->value_id=new word[pff->value_id_num];
	FileRead(pff->value_id,pff->value_id_num*2,pf);

	/*printf("value_id_num:[%d]\n",value_id_num);
	for(int i=0;i<value_id_num;++i)
	{
		printf("	[%d]\n",value_id[i]);
	}*/

	FileRead(&pff->format,4,pf);
	FileRead(&pff->size,4,pf);

	//printf("%s %d\n",name,size);

	pff->buff=new unsigned char[pff->size];
	FileRead(pff->buff,pff->size,pf);

	return 0;
}

int SF_Pass_reade_file(_FILE* pf,_pass_file* ppf)
{
	FileRead(&ppf->name_len,1,pf);
	ppf->name=new char[ppf->name_len+1];
	FileRead(ppf->name,ppf->name_len,pf);
	ppf->name[ppf->name_len]=0;

	//printf("%s\n",name);

	FileRead(&ppf->function_Id,2,pf);

	FileRead(&ppf->RenderTarget,2,pf);

	FileRead(&ppf->renderstat_num,1,pf);

	if(ppf->renderstat_num)
	{
		ppf->renderstat=new _stat_file[ppf->renderstat_num];

		for(int i=0;i<ppf->renderstat_num;i++)
		{
			FileRead(&ppf->renderstat[i].stat,4,pf);
			FileRead(&ppf->renderstat[i].value,4,pf);
		}
	}

	FileRead(&ppf->samplestat_num,1,pf);

	if(ppf->samplestat_num)
	{
		ppf->samplestat=new _stat_list_file[ppf->samplestat_num];

		for(int i=0;i<ppf->samplestat_num;i++)
		{
			ppf->samplestat[i].reade_file(pf);
		}
	}

	return 0;
}

int SF_Shader_read_file(char* fname,_shader_file* psf)
{
	_FILE* pf = files::pFilesObject->GetFile(fname);

	if(pf==0)return 0;

	_file_version checkfile={0,0,0,0,0,0,0};

	FileRead(checkfile.format,4,pf);
	FileRead(&checkfile.ver,4,pf);

	if(!_CHECK_FILEVER_SHADER.cmp(&checkfile)){
		printf("ERROR check version in file %s\n",fname);
		return 0;
	}

	FileRead(&psf->value_num,4,pf);
	FileRead(&psf->function_num,4,pf);
	FileRead(&psf->tech_num,4,pf);

	if(psf->tech_num<1 || (int)psf->value_num<0 || (int)psf->function_num<0){
		printf("ReadFile Error (%s)\n",fname);
		return -1;
	}

	psf->value=new _value_file[psf->value_num];
	psf->function=new _function_file[psf->function_num];
	psf->tech=new _tech_file[psf->tech_num];

	for(uint vi=0;vi<psf->value_num;++vi)
	{
		SF_Value_reade_file(pf,&psf->value[vi]);
	}

	for(uint fi=0;fi<psf->function_num;++fi)
	{
		SF_Function_read_file(pf,&psf->function[fi]);
	}

	for(uint ti=0;ti<psf->tech_num;++ti)
	{
		psf->tech[ti].read_file(pf);

		_pass_file* pass=psf->tech[ti].pass;

		for(int pi=0;pi<psf->tech[ti].pass_num;++pi)
		{
			pass[pi].reade_file(pf);
		}
	}
	return 0;
}

bool GetAddValueFromList(_valuelist* list, char* name, _valuelist** retvalue)
{
	int len = strlen(name);
	_valuelist* cur = list;
	int i = 0;
	for(; cur->next; i++)
	{
		//cur->id=i;
		if(len == cur->namelen && !strncmp(cur->name, name, len)){
			*retvalue = cur;
			return false;
		}
		cur = cur->next;
	}

	cur->id = i;
	shader_parser::str_cpy(name, &cur->name);
	cur->namelen = len;

	*retvalue = cur;

	cur->next = new _valuelist();

	return true;
}