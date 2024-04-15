#ifndef _LIBPNG_H_
#define _LIBPNG_H_

#include <engine/common/base_types.h>
#include <string.h>

struct _IMGSURF{
	uchar* buff = 0;
	int w = 0;
	int h = 0;
	int pixel_size = 0;

	_IMGSURF()
	{
		buff = 0;
		w = 0;
		h = 0;
		pixel_size = 0;
	}

	_IMGSURF(int width, int height, uchar* d, int ps)
	{
		buff = d;
		w = width;
		h = height;
		pixel_size = ps;
	}

	void ClearRect(int X, int Y, int W, int H, const char* Color="\x00\x00\x00\x00")
	{
		if(X > w || Y > h)return;

		uchar* pBuffTo = (&buff[(w * (h - 1)) * pixel_size]);

		uchar* ofsto = pBuffTo;

		int rowsize = W;//pSurfFrom->w;
		int rowoor = (X + W) - w;

		int colsize = H;
		int coloor = (Y + H) - h;

		if(rowoor > 0)
		{
			rowsize -= rowoor;
		}

		if(coloor > 0)
		{
			colsize -= coloor;
		}

		ofsto -= (w * pixel_size) * Y;

		for(int hi = 0; hi < colsize; ++hi)
		{
			uchar* rowto = ofsto;

			rowto += X * pixel_size;

			for(int wi = 0; wi < rowsize * pixel_size; wi += pixel_size)
			{
				memcpy(&rowto[wi], Color, pixel_size);
			}

			ofsto -= w * pixel_size;
		}
	}
};

bool LoadPng(char* fname, _IMGSURF* pretsurf);
_IMGSURF* LoadPng(char* fname);

#endif