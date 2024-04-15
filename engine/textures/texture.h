
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <engine/common/base_types.h>
#include <engine/common/gl_head.h>
#include "libpng.h"

class cTexture
{
public:
	uint textureID;
	int WrapW;
	int WrapH;

	cTexture()
	{
		textureID = 0;
		WrapW = GL_REPEAT;
		WrapH = GL_REPEAT;
	}

	cTexture(uint teid, int wrapw, int wraph)
	{
		textureID = teid;
		WrapW = wrapw;
		WrapH = wraph;
	}

	~cTexture()
	{
		if(textureID){
			glDeleteTextures(1, &textureID);
		}
	}
};

class cTextureRT :public cTexture
{
public:
	uint fboID;
	uint rboID;

	cTextureRT();
};

namespace Textures
{
#define TEXTURE_ARRAY_START_SIZE 1024
#define TEXTURE_ARRAY_ADD_SIZE 1024

	cTexture* TextureCreateFromFilePNG(char* fname, int WrapW = GL_REPEAT, int WrapH = GL_REPEAT, int Filter = GL_LINEAR);

	bool DeleteTexture(cTexture* ppTexture);
};

#endif