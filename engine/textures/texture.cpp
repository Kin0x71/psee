#include "texture.h"
#include <iostream>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

cTexture* Textures::TextureCreateFromFilePNG(char* fname, int WrapW, int WrapH, int Filter)
{
	//printf("TextureCreateFromFilePNG(%s)\n", fname);
	_IMGSURF surf;
	if(!LoadPng(fname, &surf))return 0;

	uint TexId = 0;
	glGenTextures(1, &TexId);

	glBindTexture(GL_TEXTURE_2D, TexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapW);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter);

	/*for(int y = 0; y < surf.h; ++y)
	{
		for(int x = 0; x < surf.w; ++x)
		{
			uchar* px = (uchar*)&surf.buff[((y * surf.w) + x) * 4];
			px[0] = 255;
			px[1] = 255;
			px[2] = 255;
			px[3] = 255;

			printf("[%d %d %d]",px[0],px[1],px[2]);
		}
		printf("\n");
	}*/

	int color_type = GL_RGBA;

	if(surf.pixel_size==3){
		color_type = GL_RGB;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, surf.pixel_size, surf.w, surf.h, 0, color_type, GL_UNSIGNED_BYTE, surf.buff);
	//checkOpenGLerror("CREATE WEIGHTS TEXTURE BUFFER");

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] surf.buff;

	return new cTexture(TexId, WrapW, WrapH);
}

//extern c_file* pGFile;
/*
void StretchRect(_IMGSURF* pSurfFrom, int fx, int fy, int fw, int fh, _IMGSURF* pSurfTo, int tx, int ty)
{
	if(fx > (int)pSurfFrom->w){
		fx = (int)pSurfFrom->w;
	}
	else if(fx < 0){
		fx = 0;
	}

	if(fy > (int)pSurfFrom->h){
		fy = (int)pSurfFrom->h;
	}
	else if(fy < 0){
		fy = 0;
	}

	if(tx > (int)pSurfTo->w){
		tx = (int)pSurfTo->w;
	}
	else if(tx < 0){
		tx = 0;
	}

	if(ty > (int)pSurfTo->h){
		ty = (int)pSurfTo->h;
	}
	else if(ty < 0){
		ty = 0;
	}

	uchar* pBuffFrom = (&pSurfFrom->buff[(pSurfFrom->w * (pSurfFrom->h - 1)) * pSurfFrom->pixel_size]);
	uchar* pBuffTo = (&pSurfTo->buff[(pSurfTo->w * (pSurfTo->h - 1)) * pSurfTo->pixel_size]);

	uchar* ofsfrom = pBuffFrom;
	uchar* ofsto = pBuffTo;

	int rowsize = fw;//pSurfFrom->w;
	int rowoor = (tx + fw) - pSurfTo->w;

	int colsize = fh;
	int coloor = (ty + fh) - pSurfTo->h;

	//printf("%d\n",rowoor);

	if(rowoor > 0)
	{
		rowsize -= rowoor;
	}

	if(coloor > 0)
	{
		colsize -= coloor;
	}

	//colsize-=fy;

	//rowsize-=fw;
	rowsize *= pSurfFrom->pixel_size;

	ofsfrom -= (pSurfFrom->w * pSurfFrom->pixel_size) * fy;

	ofsto -= (pSurfTo->w * pSurfTo->pixel_size) * ty;

	//printf("rowsize:%d colsize:%d\n",rowsize/3,colsize);
	//system("pause");

	for(int hi = 0; hi < colsize; ++hi)
	{
		//_RGB_PIXEL* rowfrom=(_RGB_PIXEL*)ofsfrom;
		//_RGB_PIXEL* rowto=(_RGB_PIXEL*)ofsto;
		uchar* rowfrom = ofsfrom;
		uchar* rowto = ofsto;

		rowfrom += fx * pSurfFrom->pixel_size;
		rowto += tx * pSurfTo->pixel_size;

		memcpy(rowto, rowfrom, rowsize);

		ofsfrom -= pSurfFrom->w * pSurfFrom->pixel_size;
		ofsto -= pSurfTo->w * pSurfTo->pixel_size;
	}
}

void ClearRect(_IMGSURF* pSurfTo, int x, int y, int w, int h)
{
	if(x > (int)pSurfTo->w){
		x = (int)pSurfTo->w;
	}
	else if(x < 0){
		x = 0;
	}

	if(y > (int)pSurfTo->h){
		y = (int)pSurfTo->h;
	}
	else if(y < 0){
		y = 0;
	}

	//printf("%d,%d %d,%d\n",x,y,pSurfTo->w,pSurfTo->h);

	uchar* pBuffTo = (&pSurfTo->buff[(pSurfTo->w * (pSurfTo->h - 1)) * pSurfTo->pixel_size]);

	uchar* ofsto = pBuffTo;

	int rowsize = w;//pSurfFrom->w;
	int rowoor = (x + w) - pSurfTo->w;

	int colsize = h;
	int coloor = (y + h) - pSurfTo->h;

	//printf("%d\n",rowoor);

	if(rowoor > 0)
	{
		rowsize -= rowoor;
	}

	if(coloor > 0)
	{
		colsize -= coloor;
	}

	ofsto -= (pSurfTo->w * pSurfTo->pixel_size) * y;

	for(int hi = 0; hi < colsize; ++hi)
	{
		uchar* rowto = ofsto;

		rowto += x * pSurfTo->pixel_size;

		memset(rowto, 0, rowsize * pSurfTo->pixel_size);

		ofsto -= pSurfTo->w * pSurfTo->pixel_size;
	}
}

void ClearRect(_IMGSURF* pSurfTo, int x, int y, int w, int h, char* color)
{
	if(x > (int)pSurfTo->w){
		x = (int)pSurfTo->w;
	}
	else if(x < 0){
		x = 0;
	}

	if(y > (int)pSurfTo->h){
		y = (int)pSurfTo->h;
	}
	else if(y < 0){
		y = 0;
	}

	uchar* pBuffTo = (&pSurfTo->buff[(pSurfTo->w * (pSurfTo->h - 1)) * pSurfTo->pixel_size]);

	uchar* ofsto = pBuffTo;

	int rowsize = w;//pSurfFrom->w;
	int rowoor = (x + w) - pSurfTo->w;

	int colsize = h;
	int coloor = (y + h) - pSurfTo->h;

	//printf("%d\n",rowoor);

	if(rowoor > 0)
	{
		rowsize -= rowoor;
	}

	if(coloor > 0)
	{
		colsize -= coloor;
	}

	ofsto -= (pSurfTo->w * pSurfTo->pixel_size) * y;

	for(int hi = 0; hi < colsize; ++hi)
	{
		uchar* rowto = ofsto;

		rowto += x * pSurfTo->pixel_size;

		//memset(rowto,0,rowsize*pSurfTo->pixel_size);
		for(uint wi = 0; wi < rowsize * pSurfTo->pixel_size; wi += pSurfTo->pixel_size)
		{
			memcpy(&rowto[wi], color, pSurfTo->pixel_size);
		}

		ofsto -= pSurfTo->w * pSurfTo->pixel_size;
	}
}

_IMGSURF* ReadCanalJTP(uint Size, _FILE* pf)
{
	uchar* Buff = new uchar[Size];
	FileRead(Buff, Size, pf);

	_IMGSURF* RetSurf = JpegDecompress(Buff, Size);

	delete[] Buff;
	return RetSurf;
}

_IMGSURF* CombineColorAlphaCanals(_IMGSURF* pColor, _IMGSURF* pAlpha)
{
	_IMGSURF* RetSurf = new _IMGSURF;
	RetSurf->w = pColor->w;
	RetSurf->h = pColor->h;
	RetSurf->pixel_size = 4;
	RetSurf->buff = new uchar[(RetSurf->w * RetSurf->h) * 4];

	uchar* pPixC = pColor->buff;
	uchar* pPixA = pAlpha->buff;
	uchar* pPixRet = RetSurf->buff;

	for(uint i = 0; i < RetSurf->w * RetSurf->h; ++i)
	{
		uchar* RGBA = pPixRet;
		uchar* RGB = pPixC;
		uchar* A = pPixA;

		RGBA[0] = RGB[0];
		RGBA[1] = RGB[1];
		RGBA[2] = RGB[2];
		RGBA[3] = A[0];

		pPixRet += 4;
		pPixC += 3;
		pPixA += 1;
	}

	return RetSurf;
}
*/
/*
bool cTexture::Init(bool FreeBuff, float border_color_r, float border_color_g, float border_color_b, float border_color_a)
{
	if(!surf.buff)return false;

	glBindTexture(GL_TEXTURE_2D, textureID);
	//printf("glGenTextures:%d\n",textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapW);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapH);

	float color[] = { border_color_r, border_color_g, border_color_b, border_color_a };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	if(surf.pixel_size == 3){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surf.w, surf.h, 0, GL_RGB, GL_UNSIGNED_BYTE, surf.buff);
	}
	else if(surf.pixel_size == 4){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf.w, surf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf.buff);
	}
	else{
		glBindTexture(GL_TEXTURE_2D, 0);
		return false;
	}

	if(FreeBuff){
		//printf("delete[] %d\n",surf.buff);
		delete[] surf.buff;
		//printf("ok\n");
		surf.buff = 0;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}*/

cTextureRT::cTextureRT()
{
	fboID = 0;
	rboID = 0;
}
/*
cTexture** cTexturesControl::NewTexture(int count)
{
	uint* textureIDs = new uint[count];
	glGenTextures(count, textureIDs);

	for(int i = 0; i < count; ++i)
	{
		uint textureID = textureIDs[i];

		if(textureID >= Textures_array_size)
		{
			cTexture** newTextures_array = new cTexture * [Textures_array_size + Textures_array_add_size];
			memcpy(newTextures_array, Textures_array, Textures_array_size * sizeof(cTexture*));

			Textures_array_size += Textures_array_add_size;
		}

		Textures_array[textureID] = new cTexture();
		Textures_array[textureID]->textureID = textureID;
	}

	uint first_id = textureIDs[0];

	delete[] textureIDs;

	return &Textures_array[first_id];
}*/
/*
bool cTexturesControl::DeleteTexture(cTexture** ppTexture)
{
	if(!ppTexture || !Textures_array[(*ppTexture)->textureID])return false;

	uint textureID = (*ppTexture)->textureID;

	delete Textures_array[textureID];

	Textures_array[textureID] = 0;

	return true;
}

bool cTexturesControl::TextureCreateFromFileJPG(char* fname, cTexture* pTexture, int WrapW, int WrapH)
{
	if(!fname || !pTexture)return false;

	char path_fname[MAX_PATH_LEN];

#if defined(WINDOWS_SPACE)
	strcpy(path_fname, fname);
#elif defined(LINUX_SPACE)
	char path_cur[MAX_PATH_LEN];
	xGetCurentDir(path_cur, MAX_PATH_LEN);
	sprintf(path_fname, "%s/%s", path_cur, fname);
#elif defined(OTHER_SPACE)
	//
#endif

	if(!LoadJpeg(path_fname, &pTexture->surf))return false;

	pTexture->Init(true);
	return true;
}

bool cTexturesControl::TextureCreateFromFileJPG(char* fnameC, char* fnameA, cTexture* pTexture, int WrapW, int WrapH)
{
	if(!fnameC || !fnameA || !pTexture)return false;

	char path_fnameC[MAX_PATH_LEN];
	char path_fnameA[MAX_PATH_LEN];

#if defined(WINDOWS_SPACE)
	strcpy(path_fnameC, fnameC);
	strcpy(path_fnameA, fnameA);
#elif defined(LINUX_SPACE)
	char path_cur[MAX_PATH_LEN];
	xGetCurentDir(path_cur, MAX_PATH_LEN);
	sprintf(path_fnameC, "%s/%s", path_cur, fnameC);
	sprintf(path_fnameA, "%s/%s", path_cur, fnameA);
#elif defined(OTHER_SPACE)
	//
#endif

	if(LoadJpegRGBA(path_fnameC, path_fnameA, &pTexture->surf))return false;

	pTexture->Init(true);
	return true;
}

cTexture** cTexturesControl::TextureCreateFromFileJPG(char* fname, int WrapW, int WrapH, float border_color_r, float border_color_g, float border_color_b, float border_color_a)
{
	if(!fname)return 0;

	_IMGSURF* surf = new _IMGSURF;

	char path_fname[MAX_PATH_LEN];

#if defined(WINDOWS_SPACE)
	strcpy(path_fname, fname);
#elif defined(LINUX_SPACE)
	char path_cur[MAX_PATH_LEN];
	xGetCurentDir(path_cur, MAX_PATH_LEN);
	sprintf(path_fname, "%s/%s", path_cur, fname);
#elif defined(OTHER_SPACE)
	//
#endif

	if(!LoadJpeg(path_fname, surf)){
		printf("ERROR LoadJpeg(%s)\n", path_fname);
		return 0;
	}

	cTexture** ppTexture = NewTexture();

	(*ppTexture)->surf = *surf;
	(*ppTexture)->WrapW = WrapW;
	(*ppTexture)->WrapH = WrapH;
	(*ppTexture)->Init(true, border_color_r, border_color_g, border_color_b, border_color_a);
	surf->buff = 0;
	delete surf;

	return ppTexture;
}

cTexture** cTexturesControl::TextureCreateFromFileJPG(char* fnameC, char* fnameA, int WrapW, int WrapH)
{
	if(!fnameC || !fnameA)return 0;

	_IMGSURF* surf = new _IMGSURF;

	char path_fnameC[MAX_PATH_LEN];
	char path_fnameA[MAX_PATH_LEN];

#if defined(WINDOWS_SPACE)
	strcpy(path_fnameC, fnameC);
	strcpy(path_fnameA, fnameA);
#elif defined(LINUX_SPACE)
	char path_cur[MAX_PATH_LEN];
	xGetCurentDir(path_cur, MAX_PATH_LEN);
	sprintf(path_fnameC, "%s/%s", path_cur, fnameC);
	sprintf(path_fnameA, "%s/%s", path_cur, fnameA);
	//printf("path_cur(%s)\n",path_cur);
#elif defined(OTHER_SPACE)
	//
#endif

	int ret = LoadJpegRGBA(path_fnameC, path_fnameA, surf);

	if(ret != 0){
		printf("LoadJpegRGBA ERROR [%d] (%s,%s)\n", ret, path_fnameC, path_fnameA);
		return 0;
	}

	cTexture** ppTexture = NewTexture();

	(*ppTexture)->surf = *surf;
	(*ppTexture)->WrapW = WrapW;
	(*ppTexture)->WrapH = WrapH;
	(*ppTexture)->Init(true);
	surf->buff = 0;
	delete surf;

	return ppTexture;
}
*/