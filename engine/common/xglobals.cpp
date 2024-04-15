#include "xglobals.h"
#include <windows.h>

bool xIsFileExist(char* fname)
{
	char curdir[1024];

	GetCurrentDirectoryA(1024, curdir);

	if(GetFileAttributesA(fname) != -1)return true;

	return false;
}

bool xGetCurentDir(char* buff, int len)
{
	return GetCurrentDirectoryA(len, buff);
}