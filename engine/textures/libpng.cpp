#include "libpng.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <png.h>
#include <engine/files/file.h>

using namespace data_files;

void readFileCallback( png_structp png_ptr, png_bytep out, png_size_t count )
{
	png_voidp io_ptr = png_get_io_ptr( png_ptr );
  
	if( io_ptr == 0 )
	{
		return;
	}
 
	_FILE *pf = (_FILE*)io_ptr;
 
	FileRead(out,count,pf);
}

bool LoadPng(char* fname, _IMGSURF* pretsurf)
{
	char* data;
	int width;
	int height;
	int depth;
	int color_type;

	png_structp png_ptr;
	png_infop info_ptr;

	_FILE* pf = files::pFilesObject->GetFile(fname);
	if(!pf)return 0;

	char assinatura[8];

	FileRead(assinatura, 8, pf);

	if(!png_check_sig((png_bytep)assinatura, 8))
	{
		printf("!png_check_sig:(%s) %c%c%c\n", fname, pf->data[1], pf->data[2], pf->data[3]);
		return 0;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);

	if(png_ptr == 0)
	{
		printf("!png_create_read_struct:(%s)\n", fname);
		return 0;
	}

	info_ptr = png_create_info_struct(png_ptr);

	if(info_ptr == 0)
	{
		png_destroy_read_struct(&png_ptr, 0, 0);
		printf("!png_create_info_struct:(%s)\n", fname);
		return 0;
	}

	png_set_read_fn(png_ptr, (void*)pf, readFileCallback);

	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&width, (png_uint_32*)&height, &depth, &color_type, 0, 0, 0);

	if(color_type == PNG_COLOR_TYPE_PALETTE)png_set_palette_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	png_size_t cols = png_get_rowbytes(png_ptr, info_ptr);

	png_bytepp row_pp = new png_bytep[height];
	data = new char[cols * height];

	for(int ri = 0; ri < height; ++ri)
	{
		int pr = height - ri - 1;
		row_pp[pr] = (png_bytep)&data[ri * cols];
	}

	png_read_image(png_ptr, row_pp);
	png_read_end(png_ptr, info_ptr);

	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	delete[] row_pp;
	FileClose(pf);

	//printf(">>\tPNG depth:%d color_type:%d\n", depth, color_type);

	pretsurf->w = width;
	pretsurf->h = height;
	pretsurf->buff = (uchar*)data;
	pretsurf->pixel_size = 4;

	if(color_type == PNG_COLOR_TYPE_RGB){
		pretsurf->pixel_size = 3;
	}

	return true;
}