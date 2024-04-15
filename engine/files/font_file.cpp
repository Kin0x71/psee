#include "font_file.h"

using namespace data_files;

void FontStretchRect(_FONTSURF* pSurfFrom,int fx,int fy,int fw,int fh,_FONTSURF* pSurfTo,int tx,int ty)
{
	if(tx>pSurfTo->w || ty>pSurfTo->h)return;

	uchar* pBuffFrom=(&pSurfFrom->buff[(pSurfFrom->w*(pSurfFrom->h-1))]);
	uchar* pBuffTo=(&pSurfTo->buff[(pSurfTo->w*(pSurfTo->h-1))]);
	
	uchar* ofsfrom=pBuffFrom;
	uchar* ofsto=pBuffTo;

	int rowsize=fw;//pSurfFrom->w;
	int rowoor=(tx+fw)-pSurfTo->w;

	int colsize=fh;
	int coloor=(ty+fh)-pSurfTo->h;

	//printf("%d\n",rowoor);
	
	if(rowoor>0)
	{
		rowsize-=rowoor;
	}
	
	if(coloor>0)
	{
		colsize-=coloor;
	}

	//colsize-=fy;
	
	//rowsize-=fw;

	ofsfrom -= (pSurfFrom->w)*fy;

	ofsto -= (pSurfTo->w)*ty;

	//printf("rowsize:%d colsize:%d\n",rowsize/3,colsize);
	//system("pause");

	for(int hi=0;hi<colsize;++hi)
	{
		//_RGB_PIXEL* rowfrom=(_RGB_PIXEL*)ofsfrom;
		//_RGB_PIXEL* rowto=(_RGB_PIXEL*)ofsto;
		uchar* rowfrom=ofsfrom;
		uchar* rowto=ofsto;

		rowfrom+=fx;
		rowto+=tx;

		memcpy(rowto,rowfrom,rowsize);

		ofsfrom -= pSurfFrom->w;
		ofsto -= pSurfTo->w;
	}
}

int BitsEnum(void* inbyte,char* bits,int size)
{
	if(!size)return 0;
	
	uchar* bytes=(uchar*)inbyte;
	
	uint ofs=0;
	for(uint i=0;i<size;++i)
	{
		uchar byte=bytes[i];
		bits[ofs++]=(byte&128)>>7;
		bits[ofs++]=(byte&64)>>6;
		bits[ofs++]=(byte&32)>>5;
		bits[ofs++]=(byte&16)>>4;
		bits[ofs++]=(byte&8)>>3;
		bits[ofs++]=(byte&4)>>2;
		bits[ofs++]=(byte&2)>>1;
		bits[ofs++]=(byte&1)>>0;
	}
	
	return 0;
}

_FONT_FILE* ReadFontBF(const char* fname)
{
	printf("ReadFontBF(%s)\n",fname);
	_FILE* pf = files::pFilesObject->GetFile(fname);

	if(pf==0)return 0;

	_FONT_FILE* pFontFile=new _FONT_FILE;

	pf->cur=8;

	FileRead(&pFontFile->NameLen,sizeof(uchar),pf);

	pFontFile->FontName=new char[pFontFile->NameLen+1];
	FileRead(pFontFile->FontName,pFontFile->NameLen,pf);

	pFontFile->FontName[pFontFile->NameLen]=0;

	FileRead(&pFontFile->CharSpace,sizeof(uchar),pf);
	
	FileRead(&pFontFile->CharsNum,sizeof(word),pf);

	pFontFile->Characters=new _CHAR_FILE[pFontFile->CharsNum];

	for(int i=0;i<pFontFile->CharsNum;++i)
	{
		FileRead(&pFontFile->Characters[i].id,sizeof(uint),pf);
		FileRead(&pFontFile->Characters[i].w,sizeof(uchar),pf);
		FileRead(&pFontFile->Characters[i].h,sizeof(uchar),pf);
		FileRead(&pFontFile->Characters[i].ofsx,sizeof(char),pf);
		FileRead(&pFontFile->Characters[i].ofsy,sizeof(char),pf);

		int count=pFontFile->Characters[i].w*pFontFile->Characters[i].h;
		int bytescount=count/8;
		if(count%8)++bytescount;//++bytescount;

		//printf("%d:%d\n",i,pFontFile->characters[i].id);
		//printf("%d\n",bytescount);

		pFontFile->Characters[i].buff=new char[bytescount];
		memset(pFontFile->Characters[i].buff,0,bytescount);
		
		FileRead(pFontFile->Characters[i].buff,sizeof(char)*bytescount,pf);
	}

	FileClose(pf);

	return pFontFile;
}

cFont* LoadFontBF(const char* fname)
{
	_FONT_FILE* pFontFile=ReadFontBF(fname);
	
	_FONT_CHAR* pFontCharacters=new _FONT_CHAR[pFontFile->CharsNum];
	
	cFont* pcFont=new cFont();
	
	int CharsNum=pFontFile->CharsNum;
	_CHAR_FILE* pChars=pFontFile->Characters;

	int maxW=0;
	int maxH=0;
	int maxOfsX=0;
	int maxOfsY=0;
	//int MaxRw=0;
	//int Rw=0;
	//int Rh=0;
	for(int i=0;i<CharsNum;++i)
	{
		if(pChars[i].w>maxW)maxW=pChars[i].w;
		if(pChars[i].h>maxH)maxH=pChars[i].h;

		int ofsx=pChars[i].ofsx;
		int ofsy=pChars[i].ofsy;
		
		if(ofsx<0)ofsx=-ofsx;
		if(ofsy<0)ofsy=-ofsy;
		if(ofsx>maxOfsX)maxOfsX=pChars[i].ofsx;
		if(ofsy>maxOfsY)maxOfsY=pChars[i].ofsy;
		/*
		if(!((i+1)%16)){
			printf("{%d}\n",i);
			if(Rw>MaxRw)MaxRw=Rw;
		}
		Rw+=pChars[i].w;
		Rh+=pChars[i].h;*/
	}

	maxH+=(maxOfsY/2);

//	printf("CharsNum:%d\n",CharsNum);
//	printf("maxW:%d maxH:%d maxOfsX:%d maxOfsY:%d\n",maxW,maxH,maxOfsX,maxOfsY);
	//printf("%d %d:%d\n",CharsNum,CharsNum/16,CharsNum%16);

	int Rows=CharsNum/16;
	if(CharsNum%16)++Rows;

//	printf("CharsNum%%16:%d\n",CharsNum%16);
//	printf("Rows:%d\n",Rows);

	//pFontSurf->w=(maxW+(maxOfsX/2))*CharsNum;
	pcFont->FontSurf.w=(maxW*16);
	pcFont->FontSurf.h=(maxH*Rows);

	//pFontSurf->w+=pFontSurf->w%2;
	//pFontSurf->h+=pFontSurf->h%2;

//	printf("FONT SIZE\n");
//	printf("%dx%d\n",pFontSurf->w,pFontSurf->h);
//	printf("%d/%d\n",pFontSurf->w%2,pFontSurf->h%2);

	pcFont->FontSurf.buff=new uchar[pcFont->FontSurf.w*pcFont->FontSurf.h];
	memset(pcFont->FontSurf.buff,0x0,pcFont->FontSurf.w*pcFont->FontSurf.h);

	_FONTSURF *pCharSurf=new _FONTSURF;
	pCharSurf->w=maxW;
	pCharSurf->h=maxH;

	pCharSurf->buff=new uchar[(pCharSurf->w*pCharSurf->h)];
	memset(pCharSurf->buff,0x0,(pCharSurf->w*pCharSurf->h));

	int BytesNum=maxW*maxH;
	if(BytesNum%8)++BytesNum;

	char* BytesBuff=new char[BytesNum+1];

	uint PosX=0;//+(maxOfsX/2);
	uint PosY=1;

	float texXofs=0.00;
	float texYofs=(1.0/float(Rows))-0.000;
	float texX1=texXofs;
	float texY1=1.0;
	float texY2=1.0-texYofs;

	_font_texcoord* pTexCoord=new _font_texcoord[CharsNum];
	
	for(int i=0;i<CharsNum;++i)
	{
		pCharSurf->w=pChars[i].w;
		pCharSurf->h=pChars[i].h;

		BytesNum=pCharSurf->w*pCharSurf->h;
		
		if(BytesNum%8){
			BytesNum+=8-(BytesNum%8);
		}
		
		memset(BytesBuff,0x0,BytesNum);
		BitsEnum(pChars[i].buff,BytesBuff,BytesNum/8);

		//_RGBA_PIXEL* pPixels=(_RGBA_PIXEL*)pCharSurf->buff;
		//if(pChars[i].id=='t' || pChars[i].id=='M')
/*		{
			//BytesBuff[(pCharSurf->w*pCharSurf->h)-1]=0;
			//BytesBuff[(pCharSurf->w*pCharSurf->h)-2]=0;
			
			printf("%c\n",pChars[i].id);
			int chx=0;
			for(int bi=0;bi<pCharSurf->w*pCharSurf->h;++bi)
			{
				if(BytesBuff[bi]==1)
				{
					printf("x");
				}else{
					printf(".");
				}
				if(++chx==pChars[i].w){chx=0;printf("\n");}
			}
			printf("\n");
		}
*/
		for(int bi=0;bi<pCharSurf->w*pCharSurf->h;++bi)
		{
			if(BytesBuff[bi]==1)
			{
				pCharSurf->buff[bi]=255;
			}else{
				pCharSurf->buff[bi]=0;
			}
		}

		int cx=PosX+pChars[i].ofsx;
		
		uint cposx=cx+((maxW-(pCharSurf->w+pChars[i].ofsx))/2);
		uint cposy=PosY+pChars[i].ofsy;
		
		FontStretchRect(pCharSurf,0,0,pCharSurf->w,pCharSurf->h,&pcFont->FontSurf,
			cposx,
			cposy
		);

		memset(pCharSurf->buff,0x0,pCharSurf->w*pCharSurf->h);

		pTexCoord[i].a=vec4(
			texX1,texY2,
			texX1,texY1
			);

		pTexCoord[i].b=vec4(
				texX1+0.0625,texY1,
				texX1+0.0625,texY2
			);

		texX1+=0.0625;

		PosX+=maxW;
		if(PosX>=maxW*16){
			PosX=0;//+(maxOfsX/2);
			PosY+=maxH;
			
			texX1=texXofs;
			texY1-=texYofs;
			texY2-=texYofs;
		}
		
		pFontCharacters[i].id=pChars[i].id;
		pFontCharacters[i].w=pChars[i].w;
		pFontCharacters[i].h=pChars[i].h;
		pFontCharacters[i].ofsx=pChars[i].ofsx;
		pFontCharacters[i].ofsy=pChars[i].ofsy;
		pFontCharacters[i].x=cposx;
		pFontCharacters[i].y=cposy;
	}
	//###########################
/*	_IMGSURF *pLineSurf=new _IMGSURF;
	pLineSurf->w=pFontSurf->w+pFontSurf->h;
	pLineSurf->h=pFontSurf->w+pFontSurf->h;

	pLineSurf->pixel_size=3;
	pLineSurf->buff=new UCHAR[(pLineSurf->w*pLineSurf->h)*pLineSurf->pixel_size];
	memset(pLineSurf->buff,0xff,(pLineSurf->w*pLineSurf->h)*pLineSurf->pixel_size);

	for(int hi=0;hi<Rows+1;++hi)
	{
		StretchRect(pLineSurf,0,0,pFontSurf->w,1,pFontSurf,0,hi*maxH);
	}

	for(int wi=0;wi<16+1;++wi)
	{
		StretchRect(pLineSurf,0,0,1,pFontSurf->h,pFontSurf,wi*maxW,0);
	}*/
	//############################
	
	//pcFont->pTexture->Init(true);

	pcFont->CharNum=CharsNum;
	pcFont->w=maxW;
	pcFont->h=maxH;
	pcFont->CharSpace=pFontFile->CharSpace;
	pcFont->pTexCoord=pTexCoord;
	
	pcFont->pCharacters=pFontCharacters;
	
	delete pCharSurf;
	delete[] BytesBuff;
	
	delete pFontFile;

	return pcFont;
}