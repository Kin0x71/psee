
#ifndef _TEXT_H_
#define _TEXT_H_

#include <engine/mesh/mesh.h>
#include <engine/shaders/mesh_shader_controller.h>
#include <engine/camera.h>
#include <stdarg.h>

#define MAX_CHARACTERS 256

#define _TEXT_ALIGN_LEFT 0
#define _TEXT_ALIGN_CENTER 1



struct _FONT_CHAR
{
	uint id;
	uint h, w;
	uint x, y;
	char ofsx, ofsy;

	_FONT_CHAR();
};

struct _FONTSURF
{
	uint w;
	uint h;
	uchar* buff;

	_FONTSURF();
	~_FONTSURF();
};

struct _TextItem
{
	uchar* data;
	vec4* vTexCoord;
	vec4 FontInfo;

	uint len;
	int TexCoordCount; //передаётся в шейдэр. должен быть len*2
	bool changed;
	uint FontId;

	uint Align;

	_TextItem();
	~_TextItem();

	void SetText(char* str);
	void SPrintF(const char* format, ...);
	void Update();
};

struct _font_texcoord
{
	vec4 a;
	vec4 b;
};

struct _font_table
{
	_font_texcoord TexCoord[MAX_CHARACTERS];
};

class cFont
{
public:
	struct _font_tex{
		cTexture* pTexture = 0;
		int w = 0;
		int h = 0;
		uchar* buff = 0;
	};

	_font_texcoord* pTexCoord;
	_font_tex* FontTextures;
	_FONTSURF FontSurf;
	int CharSpace;
	int CharNum;
	float w, h;
	_FONT_CHAR* pCharacters;

	cFont();
	~cFont();
};

namespace TextBase
{
	extern cFont** ppFonts;
	extern int FontsNum;
	extern uint IVertexesQuad;
	extern uint IIndexesQuad;
	extern char* TmpBuffer;

#pragma pack(push, 1)
	struct _font_vert{
		vec3 pos;
		vec3 normal;
		vec2 uv;
	};
#pragma pack(pop)

	void Init(int fonts_num);
	void AllockFonts(int fonts_num);
	void Release();
	void LoadFont(uint FontId, const char* FileName, int SubFontTexturesNum = 1);
	bool FontToTextureStretchRectColorMask(_FONTSURF* pSurfFrom, int fx, int fy, int fw, int fh, _IMGSURF* pSurfTo, int tx, int ty, uchar r, uchar g, uchar b);
	int ClearTexturePixelBufferColorMask(uint FontId, _IMGSURF* DestSurf, uint Xofs, uint Yofs, char* buff, int len, const char* color = "\x00\x00\x00\x00");
	int PrintToTexturePixelBufferColorMask(uint FontId, _IMGSURF* DestSurf, uint Xofs, uint Yofs, char* buff, int len, uchar r, uchar g, uchar b, bool clear = true);
	int PrintToTexture(uint FontId, _IMGSURF* DestSurf,uint TextureId, uint Xofs, uint Yofs, char* buff, int len);
	cTexture* CreateFontTextuteColorMask(int FontId, int SubFontTextureId, uchar r, uchar g, uchar b);
	void SizeOfText(uint FontId, char* buff, int len, float* pretw, float* preth);
};
/*
class cTextFly : public cTextBase
{
public:
	cObjectModel* pTextObject = 0;

	cList<_TextItem*> TextList;
	int FontId;
	cMeshShaderController IShaders;
	uchar color[3];

	cTextFly()
	{
		pTextObject = 0;
		FontId = 0;
		color[0] = color[1] = color[2] = 0;
	}

	void Init(int fid);

	_TextItem* AddLine(uint Count, uint FontId);

	void Update(_TextItem* pTextItem);
	//void Render(_TextItem* pTextItem);
	void Draw();
};*/
#endif