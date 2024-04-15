#ifndef _FONT_FILE_H_
#define _FONT_FILE_H_

#include "file.h"
#include <engine/ui/text.h>

struct _CHAR_FILE
{
	uint id;
	uchar h,w;
	char ofsx,ofsy;
	char* buff;

	~_CHAR_FILE()
	{
		if(buff)delete[] buff;
	}
};

struct _FONT_FILE
{
	char* FontName;
	uchar NameLen;
	uchar CharSpace;
	word CharsNum;
	_CHAR_FILE* Characters;

	~_FONT_FILE()
	{
		if(FontName)delete[] FontName;
		if(Characters)delete[] Characters;
	}
};

int BitsEnum(void* inbyte,char* bits,int size);
_FONT_FILE* ReadFontBF(const char* fname);
cFont* LoadFontBF(char* fname,uchar* color);

#endif