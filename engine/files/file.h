#ifndef _FILES_H_
#define _FILES_H_

#include <engine/common/base_types.h>
#include <engine/common/xglobals.h>
#include <engine/common/list.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "file_head.h"

#include <cstring>
#include <cstdio>
#include <map>
#include <vector>

using namespace std;
using namespace glm;

#define MAX_PATH_LEN 1024

namespace data_files{

	struct _FILE_VERTEX
	{
		vec3 p;
		vec3 n;
		vec3 c;
		vec2 t;
	};

	struct _FILE_KEY_LIST
	{
		float Time;
		vec3 position;
		vec3 scale;
		quat rotation;
		word AnimMod;

		void reset();
		_FILE_KEY_LIST();
		~_FILE_KEY_LIST();
	};

	struct _FILE
	{
		char* name = 0;
		uchar* data = 0;
		uint size = 0;
		uint cur = 0;

		_FILE(char* n, uchar* d, uint s){
			set_name(n);
			data = d;
			size = s;
		}

		_FILE(const char* n, uchar* d, uint s){
			set_name((char*)n);
			data = d;
			size = s;
		}

		~_FILE(){
			printf("~_FILE()\n");
			delete[] name;
			delete[] data;
		}

		void set_name(char* n)
		{
			int nlen = strlen(n);
			name = new char[nlen + 1];
			memcpy(name, n, nlen);
			name[nlen] = 0;
		}
	};

	/*struct _file_list
	{
		_FILE* pFile;
		char* name;
		int namelen;
		_file_list* next;
		_file_list();
	};*/

	int _cpy_fdata(void* in, char* fdata, uint* fcur, uint size);
	int FileRead(void* in, uint size, _FILE* pf);
	int FileReadLine(char* in, _FILE* pf);
	int FileReadWord(char* in, _FILE* pf);
	int FileClose(_FILE* pf, bool free = false);

	FILE* OpenFileForWrite(char* fname);
	int CreateFolder(char* path);

	class files
	{
	public:
		char CurDir[MAX_PATH_LEN];
		//map<char*, _FILE*> file_map;
		cList<_FILE*> files_list;
		
		static files* pFilesObject;

		files()
		{
			xGetCurentDir(CurDir, MAX_PATH_LEN);
		}

		_FILE* Open(char* fname);
		_FILE* Open(const char* fname);
		_FILE* GetFile(char* fname);
		_FILE* GetFile(const char* fname);

		void DBG_PrintFiles();
	};
}

#endif

































