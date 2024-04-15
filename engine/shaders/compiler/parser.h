#ifndef _PARSER_H_
#define _PARSER_H_

#include <engine/common/list.h>

namespace shader_parser{

	inline int str_cpy(char* str, char** inppc);

	struct _stritem
	{
		char* str;
		unsigned short int len;

		_stritem(char* s);
		~_stritem();
	};

	struct _intlist
	{
		int id;
		short int num;
		struct _intlist* next;

		_intlist()
		{
			id = -1;
			num = -1;
			next = 0;
		}

		~_intlist()
		{
			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _sampler_state
	{
		unsigned int stat;
		unsigned int value;
		char* stat_str;
		char* value_str;
		_sampler_state* next;

		_sampler_state()
		{
			next = 0;
			stat = 0;
			value = 0;
			stat_str = 0;
			value_str = 0;
		}

		~_sampler_state()
		{
			if(stat_str){
				delete[] stat_str;
				stat_str = 0;
			}

			if(value_str){
				delete[] value_str;
				value_str = 0;
			}

			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _sampler_str
	{
		char* name;
		_sampler_state* sampler_state;
		_sampler_str* next;

		_sampler_str()
		{
			next = 0;
			name = 0;
			sampler_state = new _sampler_state();
		}

		~_sampler_str()
		{
			if(name){
				delete[] name;
				name = 0;
			}

			if(sampler_state){
				delete sampler_state;
				sampler_state = 0;
			}

			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _renderstat_str
	{
		unsigned int stat;
		unsigned int value;

		const char* stat_str;
		const char* value_str;

		_renderstat_str* next;

		_renderstat_str()
		{
			next = 0;
			stat = 0;
			value = 0;
			stat_str = 0;
			value_str = 0;
		}

		~_renderstat_str()
		{
			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _pass_str
	{
		char* name;
		char* VS_EnterPoint;
		char* PS_EnterPoint;

		char* RenderTarget;

		_renderstat_str* renderstat;

		_pass_str* next;

		_pass_str()
		{
			next = 0;
			name = 0;
			VS_EnterPoint = 0;
			PS_EnterPoint = 0;
			renderstat = new _renderstat_str();
			RenderTarget = 0;
		}

		~_pass_str()
		{
			if(name){
				delete[] name;
				name = 0;
			}

			if(VS_EnterPoint){
				delete[] VS_EnterPoint;
				VS_EnterPoint = 0;
			}

			if(PS_EnterPoint){
				delete[] PS_EnterPoint;
				PS_EnterPoint = 0;
			}

			if(RenderTarget){
				delete[] RenderTarget;
				RenderTarget = 0;
			}

			if(renderstat){
				delete renderstat;
				renderstat = 0;
			}

			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _str_tech_list
	{
		_str_tech_list* next;

		char* name;
		int passnum;
		_pass_str* pass;

		_str_tech_list()
		{
			next = 0;
			name = 0;
			passnum = 0;
			pass = new _pass_str();
		}

		~_str_tech_list()
		{
			if(name){
				delete[] name;
				name = 0;
			}

			if(pass){
				delete pass;
				pass = 0;
			}

			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _str_tech_container
	{
		_str_tech_list* tech;
		int technum;

		_str_tech_container()
		{
			technum = 0;
			tech = new _str_tech_list();
		}

		~_str_tech_container()
		{
			if(tech){
				delete tech;
				tech = 0;
			}
		}
	};


	struct _str_val_list
	{
		_str_val_list* next;

		char* name;
		char* sem;

		_str_val_list()
		{
			next = 0;
			name = 0;
			sem = 0;
		}

		~_str_val_list()
		{
			if(name){
				delete[] name;
				name = 0;
			}

			if(sem){
				delete[] sem;
				sem = 0;
			}

			if(next){
				delete next;
				next = 0;
			}
		}
	};

	struct _str_shader_item
	{
		//_str_shader_item* next;
		_str_val_list* values;
		char* name;
		char* type;
		char* programm;

		_str_shader_item()
		{
			//next = 0;
			values = 0;
			name = 0;
			type = 0;
			programm = 0;
		}

		~_str_shader_item()
		{
			if(values){
				delete values;
				values = 0;
			}

			if(name){
				delete[] name;
				name = 0;
			}

			if(type){
				delete[] type;
				type = 0;
			}

			if(programm){
				delete[] programm;
				programm = 0;
			}
		}

		_str_val_list* get_val_item(char* ValName);
	};
	/*
	struct _str_shader_container
	{
		_str_shader_item* str_shader_list;

		_str_shader_container()
		{
			str_shader_list = 0;
		}

		~_str_shader_container()
		{
			//if(str_shader_list)delete str_shader_list;
		}
	};*/

	int find_char(char c, char* fdata, int ofs, int fsize);
	int find_str(char* data, const char* str);
	
	bool AddStrInList(cList<_stritem*>* list, char* str, int* pretid = 0);
	_pass_str* GetPassStruct(_str_tech_container* shader, unsigned int tech, unsigned int pass);
	unsigned char GetSizeRenderStruct(_renderstat_str* struc);
	_str_shader_item* GetProgremmFromNeme(char* programm_name, unsigned int type, cList<_str_shader_item*>* str_shaders);
	int GetIdByStr(cList<_stritem*>* list, char* str);

	cList<_str_shader_item*>* GatherOGLProgramms(char* buff, int fsize, int ofs = 0, int* plastofs = 0);
	_str_tech_container* GatherOGLTech(char* fdata, int ofs, int fsize);

	void InitParser();
}

#endif