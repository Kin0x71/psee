#include "parser.h"
#include <string.h>
#include <ctype.h>
#include <engine/common/base_types.h>
#include <engine/common/exception.h>

using namespace shader_parser;

const char* chars4valname = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

const char* stypes[] = {
	"bool",
	"int",
	"float",
	"float2",
	"float3",
	"float4",
	"float1x1",
	"float1x2",
	"float1x3",
	"float1x4",
	"float2x1",
	"float2x2",
	"float2x3",
	"float2x4",
	"float3x1",
	"float3x2",
	"float3x3",
	"float3x4",
	"float4x1",
	"float4x2",
	"float4x3",
	"float4x4",
	"texture",
	"sampler",
	"sampler2D",
	"sampler3D",
	"technique",
	0
};

const char* stech[] = {
	"pass",
	"VertexShader",
	"FragnentShader",
	0
};

int sizetypelist = 0;
int sizetechlist = 0;

int* itypelens = 0;
int* itechlens = 0;

struct _TYPESTR
{
	const char* str;
	unsigned int num;
	int len;
};

_TYPESTR RENDERSTAT_TYPE[] =
{
	{"RENDER_TARGET",10,13},
	{0,0,0}
};

_TYPESTR RENDERSTAT_VALUE[] =
{
	{"SHADOW",20,6},
	{"LIGHT_DEPTH",40,11},
	{0,0,0}
};

_str_val_list* _str_shader_item::get_val_item(char* ValName)
{
	char val_name[256];

	int len = 0;

	for(int i = 0; i < ValName[i]; ++i)
	{
		if(ValName[i] == '[' && ValName[i + 1] == '0' && ValName[i + 2] == ']')
		{
			i += 3;
		}

		val_name[len++] = ValName[i];
	}

	val_name[len++] = 0;

	_str_val_list* cur = values;
	while(cur->next)
	{
		if(!strcmp(val_name, cur->name))return cur;
		cur = cur->next;
	};

	return 0;
}

void shader_parser::InitParser()
{
	sizetypelist = (sizeof(stypes) / ARCH_BYTES) - 1;
	itypelens = new int[sizetypelist];
	for(int i = 0; i < sizetypelist; i++){ itypelens[i] = strlen(stypes[i]); }

	sizetechlist = (sizeof(stech) / ARCH_BYTES) - 1;
	itechlens = new int[sizetechlist];
	for(int i = 0; i < sizetechlist; i++){ itechlens[i] = strlen(stech[i]); }
}

unsigned int GetIdTypeByStr(char* str, _TYPESTR* pstruct, unsigned int* offset = 0)
{
	int len = strlen(str);

	char strbuff[256];
	memcpy(strbuff, str, len);
	char* strup = _strupr(strbuff);

	for(unsigned int i = 0; pstruct[i].str[0]; i++)
	{
		if(pstruct[i].len == len && !strncmp(pstruct[i].str, strbuff, len)){
			if(offset)*offset = i;
			return pstruct[i].num;
		}
	}

	return -1;
}

_sampler_state* GetSamplerStatStruct(_sampler_str* pstruct, char* samplername)
{
	_sampler_str* sampler_cur = pstruct;

	while(sampler_cur->next)
	{
		if(!strcmp(sampler_cur->name, samplername))return sampler_cur->sampler_state;
		sampler_cur = sampler_cur->next;
	}

	return 0;
}

void PrintSamplerStatStruct(_sampler_str* pstruct)
{
	_sampler_str* sampler_cur = pstruct;

	while(sampler_cur->next)
	{
		sampler_cur = sampler_cur->next;
	}

	return;
}

_pass_str* shader_parser::GetPassStruct(_str_tech_container* shader, unsigned int tech, unsigned int pass)
{
	_str_tech_list* tech_cur = shader->tech;
	unsigned int nTech = 0;
	while(tech_cur)
	{
		_pass_str* pass_cur = tech_cur->pass;
		unsigned int nPass = 0;

		while(pass_cur)
		{
			if(nTech == tech && nPass == pass)
			{
				return pass_cur;
			}

			nPass++;
			pass_cur = pass_cur->next;
		}

		nTech++;
		tech_cur = tech_cur->next;
	}

	return 0;
}

unsigned char GetSizeSamplerStatStruct(_sampler_state* pstruct)
{
	_sampler_state* struc_cur = pstruct;
	int i = 0;
	for(; struc_cur->next;)
	{
		if(struc_cur->stat != -1 && struc_cur->value != -1)i++;

		struc_cur = struc_cur->next;
	}
	return i;
}

unsigned char shader_parser::GetSizeRenderStruct(_renderstat_str* struc)
{
	_renderstat_str* struc_cur = struc;
	int i = 0;
	for(; struc_cur->next; i++)
	{
		struc_cur = struc_cur->next;
	}
	return i;
}

int shader_parser::GetIdByStr(cList<_stritem*>* list, char* str)
{
	if(!str)return -1;

	int len = strlen(str);

	char strbuff[256];
	memcpy(strbuff, str, len+1);
	char* strup = _strupr(strbuff);

	return list->enumerate(
		[strup,len](_stritem* item){
			if(len == item->len && !strncmp(item->str, strup, len)){
				return true;
			}
			return false;
		}
	);
}

_stritem::_stritem(char* s)
{
	len = str_cpy(s, &str);
}

_stritem::~_stritem()
{
	if(str){
		delete[] str;
		str = 0;
	}
}

char* GetStrById(cList<_stritem*>* list, int id)
{
	return (*list)[id]->str;
}

inline int shader_parser::str_cpy(char* str, char** inppc)
{
	int len = strlen(str) + 1;
	*inppc = new char[len];
	memcpy(*inppc, str, len);

	return len - 1;
}

bool strupncmp(char* str1, char* str2, int len)
{
	for(int i = 0; i < len; i++)
	{
		char c1 = toupper(str1[i]);
		char c2 = toupper(str2[i]);

		if(c1 != c2)return true;
	}

	return false;
}

int shader_parser::find_char(char c, char* fdata, int ofs, int fsize)
{
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == c){
			return i;
		}
	}
	return -1;
}

int find_char_close(char findc, const char* closestr, char* fdata, int ofs, int fsize)
{
	int closelen = strlen(closestr);

	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == closestr[0] && !strncmp(&fdata[i], closestr, closelen)){
			return ofs;
		}

		if(fdata[i] == findc){
			return i;
		}
	}
	return ofs;
}

int shader_parser::find_str(char* data, const char* str)
{
	int len = strlen(str);

	for(int i = 0; data[i]; i++)
	{
		if(data[i] == str[0] && !strncmp(&data[i + 1], &str[1], len - 1)){
			return i + len;
		}
	}
	return -1;
}

int get_close_commentA_ofs(char* fdata, int ofs, int fsize)
{//<>
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '>'){
			return i + 1;
		}
	}
	return ofs + 1;
}

int get_close_commentB_ofs(char* fdata, int ofs, int fsize)
{/**/
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '*' && fdata[i + 1] == '/'){
			return i + 1;
		}
	}
	return ofs + 1;
}

int get_close_commentC_ofs(char* fdata, int ofs, int fsize)
{//
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '\r' && fdata[i + 1] == '\n'){

			if(fdata[i + 2] == '/' && fdata[i + 3] == '/'){
				return get_close_commentC_ofs(fdata, i + 2, fsize);
			}

			return i + 1;
		}
	}

	return ofs + 1;
}

int get_close_commentD_ofs(char* fdata, int ofs, int fsize)
{//{}
	int check = 1;
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '{'){
			check++;
		}

		if(fdata[i] == '}'){
			check--;

			if(check == 0){
				return i;
			}
		}
	}

	return ofs + 1;
}

int get_close_commentE_ofs(char* fdata, int ofs, int fsize)
{//()
	int check = 1;
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '('){
			check++;
		}

		if(fdata[i] == ')'){
			check--;

			if(check == 0){
				return i;
			}
		}
	}

	return ofs + 1;
}

int get_close_ofs(char c, char* fdata, int ofs, int fsize)
{

	switch(c)
	{
	case '<':
		return get_close_commentA_ofs(fdata, ofs, fsize);
		break;
	case '/':

		if(fdata[ofs] == '*')return get_close_commentB_ofs(fdata, ofs, fsize);
		if(fdata[ofs] == '/')return get_close_commentC_ofs(fdata, ofs, fsize);

		break;

	case '{':
		return get_close_commentD_ofs(fdata, ofs, fsize);
		break;
	case '(':
		return get_close_commentE_ofs(fdata, ofs, fsize);
		break;
	}

	char cmpc = 0;
	if(c == '{')cmpc = '}';
	else if(c == '<')cmpc = '>';
	else if(c == '(')cmpc = ')';
	else if(c == '/')cmpc = '*';

	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '*' && cmpc == '*'){
			warning_format_log(__FUNCTION__, __FILE__, __LINE__,"%d %c", i, fdata[i + 1]);
		}

		if(fdata[i] == cmpc){
			return i + 1;
		}
	}
	return ofs + 1;
}

bool is_char_4_value_name(char c)
{
	for(int i = 0; chars4valname[i]; i++)
	{
		if(c == chars4valname[i])return true;
	}
	return false;
}

int get_endstr_by_no_val_name_char(char* fdata, int ofs, int fsize)
{
	for(int i = ofs; i < fsize; i++)
	{
		if(!is_char_4_value_name(fdata[i])){
			return i;
		}
	}
	return -1;
}

int find_str_in_file(const char* str, char* fdata, int ofs, int fsize)
{
	int len = strlen(str);
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == str[0] && !strncmp(&fdata[i + 1], &str[1], len - 1)){
			return i;
		}
	}
	return -1;
}


int get_any_by_val_name_char(char* fdata, int ofs, int fsize)
{
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '/' && fdata[i + 1] == '/')i = shader_parser::find_char('\n', fdata, i, fsize);
		if(fdata[i] == '/' && fdata[i + 1] == '*')i = find_str_in_file("*/", fdata, i, fsize);

		if(is_char_4_value_name(fdata[i])){
			return i;
		}
	}
	return -1;
}


int find_next_ofs_space(char* fdata, int ofs, int fsize)
{
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == ' ' || fdata[i] == '	' || fdata[i] == '\r' || fdata[i] == '\n' || fdata[i] == '=' || fdata[i] == ';'){
			return i;
		}
	}
	return -1;
}

int find_next_ofs_not_space(char* fdata, int ofs, int fsize)
{
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] != ' ' && fdata[i] != '	'){
			return i;
		}
	}
	return -1;
}

int get_str_funcname(char* fdata, int ofs, int fsize, char* pretstr)
{
	int sbuffcur = 0;
	int seek = 0;
	bool ctr = false;
	for(int i = ofs; i < fsize; i++)
	{
		int tseek = 0;
		int lastseek = 0;
		if(fdata[i] == '{' || fdata[i] == '<'){
			lastseek = tseek = i;
			i = get_close_ofs(fdata[i], fdata, i + 1, fsize);
			tseek = i - tseek;
		}
		seek += tseek;

		if(fdata[i] == '=')ctr = true;

		if(!ctr && fdata[i] == ' ' && fdata[i] == '	' && fdata[i] != '('){
			pretstr[0] = 0;
			return i;
		}

		if(fdata[i] == '('){
			return i + 1;
		}

		pretstr[sbuffcur] = fdata[i];
		sbuffcur++;
		pretstr[sbuffcur] = 0;
	}

	return ofs + 1;
}

bool shader_parser::AddStrInList(cList<_stritem*>* list, char* str, int* pretid)
{
	int len = strlen(str);
	/*_stritem* cur = list;
	int i = 0;
	for(; cur->next; i++)
	{
		if(len == cur->len && !strncmp(cur->str, str, len)){
			if(pretid)*pretid = cur->id;
			return false;
		}
		cur = cur->next;
	}*/

	int str_id = list->enumerate(
		[str,len](_stritem* item){
			if(len == item->len && !strncmp(item->str, str, len)){
				return true;
			}
			return false;
		}
	);

	if(str_id != -1){
		if(pretid)*pretid = str_id;
		return false;
	}

	_stritem* new_item = new _stritem(str);
	printf("###\tAddStrInList #x %s\n", new_item->str, new_item->str);

	if(pretid)*pretid = list->count;

	list->push(new_item);

	return true;
}

bool PrintStrList(cList<_stritem*>* list)
{
	return true;
}

int find_renderstats(char* fdata, int ofs, int fsize, _renderstat_str* currs)
{
	char tmpstr[256];
	for(int ofst = ofs; ofst < fsize; ofst++)
	{

		if(fdata[ofst] == '/' && fdata[ofst + 1] == '/')ofst = shader_parser::find_char('\n', fdata, ofst, fsize);

		if(fdata[ofst] == '/' && fdata[ofst + 1] == '*')ofst = find_str_in_file("*/", fdata, ofst, fsize);

		if(fdata[ofst] == '}')break;

		for(int it = 0; RENDERSTAT_TYPE[it].str != 0; ++it)
		{
			const char* typestr = RENDERSTAT_TYPE[it].str;
			int typelen = RENDERSTAT_TYPE[it].len;

			if(fdata[ofst] == typestr[0] &&
				!strncmp(&fdata[ofst + 1], &typestr[1], typelen - 1) &&
				(fdata[ofst + typelen] == ' ' || fdata[ofst + typelen] == '	' || fdata[ofst + typelen] == '=')
				)
			{
				ofst += typelen;

				ofst = shader_parser::find_char('=', fdata, ofst, fsize);

				if(ofst == -1){
					return -1;
				}

				++ofst;

				int nexti = get_endstr_by_no_val_name_char(fdata, ofst, fsize);

				if(nexti == ofst)++ofst;

				nexti = find_next_ofs_space(fdata, ofst, fsize);
				int len = (nexti - ofst);

				memcpy(tmpstr, &fdata[ofst], len);
				tmpstr[len] = 0;

				unsigned int offsetvalue = 0;
				currs->stat = RENDERSTAT_TYPE[it].num;
				currs->value = GetIdTypeByStr(tmpstr, RENDERSTAT_VALUE, &offsetvalue);

				currs->stat_str = RENDERSTAT_TYPE[it].str;
				currs->value_str = RENDERSTAT_VALUE[offsetvalue].str;

				currs->next = new _renderstat_str();
				currs = currs->next;

				ofs = ofst = nexti;
			}

		}

	}

	return ofs;
}

int tech_analysis(char* fdata, int ofs, int fsize, char* pretstr, _str_tech_list* tech)
{
	_pass_str* curpass = 0;

	int blockcheck = 0;
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '/' && fdata[i + 1] == '/')i = shader_parser::find_char('\n', fdata, i, fsize);

		if(fdata[i] == '/' && fdata[i + 1] == '*')i = find_str_in_file("*/", fdata, i, fsize);

		for(int ti = 0; stech[ti] != 0; ti++)
		{
			if(fdata[i] == stech[ti][0] &&
				!strncmp(&fdata[i + 1], &stech[ti][1], itechlens[ti] - 1) &&
				(fdata[i + itechlens[ti]] == ' ' || fdata[i + itechlens[ti]] == '	' || fdata[i + itechlens[ti]] == '=')
				)
			{
				if(ti == 0){

					if(curpass != 0){
						curpass->next = new _pass_str();
						curpass = curpass->next;
					}
					else{
						curpass = tech->pass;
					}

					i += itechlens[ti];
					i = get_endstr_by_no_val_name_char(fdata, i, fsize) + 1;
					int nexti = find_next_ofs_space(fdata, i, fsize);
					int len = (nexti - i);
					curpass->name = new char[len + 1];
					memcpy(curpass->name, &fdata[i], len);
					curpass->name[len] = 0;

					tech->passnum++;
					i = nexti;

					i = shader_parser::find_char('{', fdata, i, fsize);

					find_renderstats(fdata, i, fsize, curpass->renderstat);
				}

				if(ti == 1 || ti == 2){
					i += itechlens[ti];

					i = shader_parser::find_char('=', fdata, i, fsize);

					if(i == -1){
						return -1;
					}

					i = get_endstr_by_no_val_name_char(fdata, i + 1, fsize);

					if(!strncmp(&fdata[i + 1], "compile", 7)){
						goto L_CompileOk;
					}
					else if(!strncmp(&fdata[i + 1], "NULL", 4)){
						goto L_NoCompile;
					}
					else{
						return -2;
					}
				L_CompileOk:;
					i += 7;

					i = get_endstr_by_no_val_name_char(fdata, i + 1, fsize);

					if(i == -1){
						return -3;
					}

					i++;


					if(fdata[i] == ' ' || fdata[i] == '	'){
						i = find_next_ofs_not_space(fdata, i, fsize);
					}


					char strEnterPoint[256];
					i = get_str_funcname(fdata, i, fsize, strEnterPoint);

					if(ti == 1){

						if(curpass->VS_EnterPoint == 0){

							shader_parser::str_cpy(strEnterPoint, &curpass->VS_EnterPoint);
							printf("###\ttech_analysis A #x %s\n", curpass->VS_EnterPoint, curpass->VS_EnterPoint);
						}else warning_format_log(__FUNCTION__, __FILE__, __LINE__, "#11 %s [%s %s]",strEnterPoint,tech->name,curpass->name);

					}
					else if(ti == 2){

						if(curpass->PS_EnterPoint == 0){

							shader_parser::str_cpy(strEnterPoint, &curpass->PS_EnterPoint);
							printf("###\ttech_analysis B #x %s\n", curpass->PS_EnterPoint, curpass->PS_EnterPoint);
						}else warning_format_log(__FUNCTION__, __FILE__, __LINE__, "#12 %s [%s %s]",strEnterPoint,tech->name,curpass->name);
					}

				L_NoCompile:;
				}

			}
		}

		if(fdata[i] == '{')blockcheck++;
		else if(fdata[i] == '}')blockcheck--;

		if(fdata[i] == '{' || fdata[i] == '}'){
			if(blockcheck == 0)return i;
		}

	}

	return ofs + 1;
}

_str_tech_container* shader_parser::GatherOGLTech(char* fdata, int ofs, int fsize)
{
	_str_tech_container* str_tech_container = new _str_tech_container();
	_str_tech_list* curtech = str_tech_container->tech;

	int truefsize = fsize;
	int ti = 0;
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '{' || fdata[i] == '<' || fdata[i] == '('){
			i = get_close_ofs(fdata[i], fdata, i + 1, fsize);
		}

		if(fdata[i] == '/' && fdata[i + 1] == '/')i = shader_parser::find_char('\n', fdata, i, fsize);

		if(fdata[i] == '/' && fdata[i + 1] == '*')i = find_str_in_file("*/", fdata, i, fsize);

		if(fdata[i] == 't' &&
			!strncmp(&fdata[i + 1], "echnique", 8) &&
			(fdata[i + 9] == ' ' || fdata[i + 9] == '	')
			)
		{
			char valbuff[256];

			int Aofs = i + 9 + 1;
			int Bofs = get_endstr_by_no_val_name_char(fdata, Aofs, fsize);
			memcpy(valbuff, &fdata[Aofs], Bofs - Aofs);
			valbuff[Bofs - Aofs] = 0;
			i = Aofs;
			i += (Bofs - Aofs);

			shader_parser::str_cpy(valbuff, &curtech->name);
			printf("###\tGatherOGLTech #x %s\n", curtech->name, curtech->name);
			i = tech_analysis(fdata, i, fsize, valbuff, curtech);

			if(i < 0)return 0;
			curtech->next = new _str_tech_list();
			curtech = curtech->next;

			str_tech_container->technum = ++ti;
		}
	}
	return str_tech_container;
}

int filter_file(char* fdata, int fsize, char* retbuff)
{
	int buffofs = 0;

	for(int i = 0; i < fsize; i++)
	{
		if((fdata[i] == '/' && fdata[i + 1] == '*') || (fdata[i] == '/' && fdata[i + 1] == '/')){
			i = get_close_ofs(fdata[i], fdata, i + 1, fsize);
		}
		retbuff[buffofs] = fdata[i];
		buffofs++;
	}
	retbuff[buffofs] = 0;
	return buffofs;
}

int programm_analysis(char* fdata, int ofs, int fsize, char* pretstr, _str_tech_list* tech)
{
	_pass_str* curpass = 0;
	int blockcheck = 0;
	for(int i = ofs; i < fsize; i++)
	{
		if(fdata[i] == '/' && fdata[i + 1] == '/')i = shader_parser::find_char('\n', fdata, i, fsize);

		if(fdata[i] == '/' && fdata[i + 1] == '*')i = find_str_in_file("*/", fdata, i, fsize);

		for(int ti = 0; stech[ti] != 0; ti++)
		{
			if(fdata[i] == stech[ti][0] &&
				!strncmp(&fdata[i + 1], &stech[ti][1], itechlens[ti] - 1) &&
				(fdata[i + itechlens[ti]] == ' ' || fdata[i + itechlens[ti]] == '	' || fdata[i + itechlens[ti]] == '=')
				)
			{
				if(ti == 0){

					if(curpass != 0){
						curpass->next = new _pass_str();
						curpass = curpass->next;
					}
					else{
						curpass = tech->pass;
					}

					i += itechlens[ti];
					i = get_endstr_by_no_val_name_char(fdata, i, fsize) + 1;
					int nexti = find_next_ofs_space(fdata, i, fsize);
					int len = (nexti - i);
					curpass->name = new char[len + 1];
					memcpy(curpass->name, &fdata[i], len);
					curpass->name[len] = 0;

					tech->passnum++;
					i = nexti;

					i = shader_parser::find_char('{', fdata, i, fsize);

					find_renderstats(fdata, i, fsize, curpass->renderstat);
				}

				if(ti == 1 || ti == 2){
					i += itechlens[ti];

					i = shader_parser::find_char('=', fdata, i, fsize);

					if(i == -1){
						return -1;
					}

					i = get_endstr_by_no_val_name_char(fdata, i + 1, fsize);

					if(!strncmp(&fdata[i + 1], "compile", 7)){
						goto L_CompileOk;
					}
					else if(!strncmp(&fdata[i + 1], "NULL", 4)){
						goto L_NoCompile;
					}
					else{
						return -2;
					}
				L_CompileOk:;
					i += 7;

					i = get_endstr_by_no_val_name_char(fdata, i + 1, fsize);

					if(i == -1){
						return -3;
					}

					i++;


					if(fdata[i] == ' ' || fdata[i] == '	'){
						i = find_next_ofs_not_space(fdata, i, fsize);
					}


					char strEnterPoint[256];
					i = get_str_funcname(fdata, i, fsize, strEnterPoint);

					if(ti == 1){

						if(curpass->VS_EnterPoint == 0){

							shader_parser::str_cpy(strEnterPoint, &curpass->VS_EnterPoint);
							printf("###\tprogramm_analysis A #x %s\n", curpass->VS_EnterPoint, curpass->VS_EnterPoint);
						}else warning_format_log(__FUNCTION__, __FILE__, __LINE__, "#11 %s [%s %s]\n",strEnterPoint,tech->name,curpass->name);

					}
					else if(ti == 2){

						if(curpass->PS_EnterPoint == 0){

							shader_parser::str_cpy(strEnterPoint, &curpass->PS_EnterPoint);
							printf("###\tprogramm_analysis B #x %s\n", curpass->PS_EnterPoint, curpass->PS_EnterPoint);
						}else warning_format_log(__FUNCTION__, __FILE__, __LINE__, "#12 %s [%s %s]\n",strEnterPoint,tech->name,curpass->name);
					}
				L_NoCompile:;
				}
			}
		}

		if(fdata[i] == '{')blockcheck++;
		else if(fdata[i] == '}')blockcheck--;

		if(fdata[i] == '{' || fdata[i] == '}'){
			if(blockcheck == 0)return i;
		}

	}

	return ofs + 1;
}

int get_block(char* buff, int fsize, int ofs = 0)
{
	int blockcheck = 0;
	for(int i = ofs; i < fsize; i++)
	{
		if(buff[i] == '{')blockcheck++;
		else if(buff[i] == '}')blockcheck--;

		if(buff[i] == '{' || buff[i] == '}'){
			if(blockcheck == 0)return i;
		}
	}
	return ofs;
}

int find_end_name(char* buff, int fsize, int ofs)
{
	for(int i = ofs; i < fsize; ++i)
	{
		if(buff[i] == ' ' || buff[i] == '	' || buff[i] == '\r' || buff[i] == '\n' || buff[i] == '=' || buff[i] == ';' || buff[i] == ':'){
			return i;
		}
	}
	return ofs;
}

int get_name(char* buff, int fsize, int ofs, char* retbuff)
{
	int ti = get_endstr_by_no_val_name_char(buff, ofs, fsize) + 1;

	int nexti = find_end_name(buff, fsize, ti);
	int len = (nexti - ti);

	if(len < 1)return ofs;

	memcpy(retbuff, &buff[ti], len);
	retbuff[len] = 0;
	return nexti;
}

cList<_str_shader_item*>* shader_parser::GatherOGLProgramms(char* buff, int fsize, int ofs, int* plastofs)
{
	cList<_str_shader_item*>* ret_list = new cList<_str_shader_item*>();
	
	//_sscontainer->str_shader_list = sslist_cur;

	char tmpstr[256];

	int truefsize = fsize;

	int ti = ofs;
	for(int i = ofs; i < fsize; i++)
	{
		if(buff[i] == '{' || buff[i] == '<' || buff[i] == '('){
			i = get_close_ofs(buff[i], buff, i + 1, fsize);
		}

		if(buff[i] == '/' && buff[i + 1] == '/')i = shader_parser::find_char('\n', buff, i, fsize);

		if(buff[i] == '/' && buff[i + 1] == '*')i = find_str_in_file("*/", buff, i, fsize);

		int sofs = shader_parser::find_str(&buff[i], "GL_SHADER") + i;

		int nexti = get_name(buff, fsize, sofs, tmpstr);
		if(nexti == sofs){
			break;
		}

		int len = nexti - sofs;
		char* name = new char[len];
		memcpy(name, tmpstr, len);

		sofs = nexti;
		tmpstr[0] = 0;

		nexti = get_name(buff, fsize, sofs, tmpstr);
		if(nexti == sofs){
			return 0;
		}

		len = nexti - sofs;
		char* type = new char[len];
		memcpy(type, tmpstr, len);

		i = nexti;

		_str_shader_item* str_shader_item = new _str_shader_item();

		str_shader_item->name = name;
		str_shader_item->type = type;

		_str_val_list* svlist_cur = new _str_val_list();

		str_shader_item->values = svlist_cur;

		for(int vi = nexti; nexti < fsize; ++vi)
		{
			tmpstr[0] = 0;

			int ic = get_any_by_val_name_char(buff, vi, fsize) - 1;
			nexti = get_name(buff, fsize, ic, tmpstr);

			if(nexti == sofs){
				return 0;
			}

			if(!strcmp(tmpstr, "programm"))
			{
				int to = get_block(buff, fsize, nexti);

				int from = find_char_close('{', "}", buff, nexti, fsize);

				int len = (to - from) - 1;
				char* programm = new char[len + 1];
				memcpy(programm, &buff[from + 1], len);
				programm[len] = 0;

				str_shader_item->programm = programm;

				nexti = to;
				break;
			}
			else{

				int len = nexti - ic;
				char* name = new char[len];
				memcpy(name, tmpstr, len);

				svlist_cur->name = name;

				ic = find_char_close(':', "\n", buff, nexti, fsize);

				if(buff[ic] == ':'){
					ic = get_any_by_val_name_char(buff, ic, fsize) - 1;
					nexti = get_name(buff, fsize, ic, tmpstr);

					int len = nexti - ic;
					char* sem = new char[len];
					memcpy(sem, tmpstr, len);

					svlist_cur->sem = sem;
				}

				svlist_cur->next = new _str_val_list();
				svlist_cur = svlist_cur->next;
			}

			vi = nexti;
		}

		i = nexti;

		plastofs ? *plastofs = i : 0;

		ret_list->push(str_shader_item);
	}

	return ret_list;
}

_str_shader_item* shader_parser::GetProgremmFromNeme(char* programm_name, unsigned int type, cList<_str_shader_item*>* str_shaders)
{
	//_str_shader_item* cur = shaders_container->str_shader_list;
	printf("GetProgremmFromNeme(%s)\n", programm_name);

	const char* sType = 0;

	const char* sTypeV = "VERTEX_SHADER";
	const char* sTypeF = "FRAGMENT_SHADER";

	type == 'v' ? sType = sTypeV : sType = sTypeF;

	int index=str_shaders->enumerate(
		[programm_name, sType](_str_shader_item* item){
			//printf("\t%s==%s\n", programm_name, item->name);
			if(!strcmp(programm_name, item->name) && !strcmp(sType, item->type)){
				return true;
			}
			return false;
		}
	);

	if(index != -1){
		return (*str_shaders)[index];
	}

	printf("\t%s not found\n", programm_name);
	return 0;
}
