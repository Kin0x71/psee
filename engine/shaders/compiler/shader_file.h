
#ifndef _SHADER_FILE_H_
#define _SHADER_FILE_H_

#include <engine/files/file.h>
#include "parser.h"

using namespace data_files;

struct _stat_file
{
	uint stat;
	uint value;

	_stat_file()
	{
		stat=0;
		value=0;
	}
};

struct _stat_list_file
{
	uint Register;
	_stat_file *stat;
	word stat_num;

	_stat_list_file()
	{
		Register=0;
		stat=0;
		stat_num=0;
	}

	~_stat_list_file()
	{
		if(stat){
			delete[] stat;
			stat = 0;
		}
	}

	int reade_file(_FILE *pf);
};

struct _value_file
{
	unsigned char name_len;
	char *name;
	word semantic;
	char type;
	
	_value_file()
	{
		semantic=-1;
		name=0;
		name_len=0;
		type='X';
	}

	int reade_file(_FILE *pf);
};

struct _function_file
{
	unsigned char name_len;
	char *name;
	word value_id_num;
	word *value_id;
	uint size;
	uint format;
	unsigned char *buff;
	
	_function_file()
	{
		name=0;
		name_len=0;
		value_id_num=0;
		value_id=0;
		size=0;
		format=0;
		buff=0;
	}

	~_function_file()
	{
		if(value_id){
			delete[] value_id;
			value_id = 0;
		}

		if(format && buff){
			delete[] buff;
			buff = 0;
		}
	}

	int read_file(_FILE *pf);
};

struct _pass_file
{
	unsigned char name_len;
	char *name;
	word function_Id;
	word RenderTarget;
	unsigned char renderstat_num;
	_stat_file *renderstat;
	unsigned char samplestat_num;
	_stat_list_file *samplestat;

	_pass_file()
	{
		name_len=0;
		name=0;
		function_Id=-1;
		RenderTarget=0;
		renderstat_num=0;
		renderstat=0;
		samplestat_num=0;
		samplestat=0;
	}

	~_pass_file()
	{
		if(renderstat){
			delete[] renderstat;
			renderstat = 0;
		}

		if(samplestat){
			delete[] samplestat;
			samplestat = 0;
		}
	}

	int reade_file(_FILE *pf);
};

struct _tech_file
{
	unsigned char name_len;
	char *name;
	unsigned char pass_num;
	_pass_file *pass;

	_tech_file()
	{
		pass_num=0;
		name_len=0;
		name=0;
		pass=0;
	}

	~_tech_file()
	{
		if(pass){
			delete[] pass;
			pass = 0;
		}
	}

	int read_file(_FILE *pf);
};

struct _shader_file
{
	uint value_num;
	uint function_num;
	_value_file *value;
	_function_file *function;
	uint tech_num;
	_tech_file *tech;

	_shader_file()
	{
		tech_num=0;
		tech=0;

		value_num=0;
		function_num=0;
		value=0;
		function=0;
	}

	~_shader_file()
	{
		if(value){
			delete[] value;
			value = 0;
		}

		if(function){
			delete[] function;
			function = 0;
		}

		if(tech){
			delete[] tech;
			tech = 0;
		}
	}

	int read_file(char *fname);
};

struct _valuelist
{
	_valuelist* next;
	int id;
	char* name;
	uint semantic;
	char type;

	word namelen;
	_valuelist()
	{
		next = 0;
		id = -1;
		semantic = -1;
		name = 0;
		namelen = 0;
		type = 'X';
	}

	~_valuelist()
	{
		if(name){
			delete[] name;
			name = 0;
		}

		if(next){
			delete next;
			next = 0;
		}
	}
};

struct _vffunc_item
{
	shader_parser::_str_shader_item* str_shader_item_vs;
	shader_parser::_str_shader_item* str_shader_item_fs;
	int id;
	char* name;
	word value_id_num;
	word* value_id;
	uint size;
	unsigned char* buff;
	uint format;
	word namelen;

	_vffunc_item()
	{
		str_shader_item_vs = 0;
		str_shader_item_fs = 0;
		id = -1;
		name = 0;
		namelen = 0;
		value_id_num = 0;
		value_id = 0;
		size = 0;
		buff = 0;
		format = 0;
	}

	~_vffunc_item()
	{

		if(name){
			delete[] name;
		}

		/*
		удаляется в ~_str_shader_item()
		if(str_shader_item_vs){
			delete str_shader_item_vs;
			str_shader_item_vs = 0;
		}*/
		/*
		* удаляется в ~_str_shader_item()
		if(str_shader_item_fs){
			delete str_shader_item_fs;
			str_shader_item_fs = 0;
		}*/
		
		/*if(value_id){
			delete[] value_id;
			value_id = 0;
		}*/
		
		/*if(buff && size){
			delete[] buff;
			buff = 0;
		}*/
	}
};

bool GetAddValueFromList(_valuelist* list, char* name, _valuelist** retvalue);
#endif