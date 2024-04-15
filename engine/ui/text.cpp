
#include "text.h"

#include <glm/ext/matrix_transform.hpp>

#include <engine/engine.h>
//#include "../../headers/gl_head.h"
#include <engine/shaders/ShaderSemantics.h>
//#include "../debug.h"

cFont** TextBase::ppFonts = 0;
int TextBase::FontsNum = 0;
uint TextBase::IVertexesQuad = 0;
uint TextBase::IIndexesQuad = 0;
char* TextBase::TmpBuffer = 0;

extern shaders_container* pMainShadersContainer;
extern cCamera Camera;
//extern _VERTEX_DECLARATION* pVERTDECL[];

extern cTexture* pTexUiet;

cFont* LoadFontBF(const char* fname);

_FONT_CHAR::_FONT_CHAR()
{
	id = h = w = x = y = ofsx = ofsy = 0;
}

_FONTSURF::_FONTSURF()
{
	w = 0;
	h = 0;
	buff = 0;
}

_FONTSURF::~_FONTSURF()
{
	if(buff){
		delete[] buff;
		buff = 0;
	}
}

#pragma pack(push, 1)
struct _text_vert{
	vec3 pos;
	vec2 uv;
};
#pragma pack(pop)

void TextBase::Init(int fonts_num)
{
	_text_vert Vertexes[] = {
		{vec3(-0.5f,-0.5f,0.0f),vec2(0.0f,0.0f)},
		{vec3(-0.5f,0.5f,0.0f),vec2(0.0f,1.0f)},
		{vec3(0.5f,0.5f,0.0f),vec2(1.0f,1.0f)},
		{vec3(0.5f,-0.5f,0.0f),vec2(1.0f,0.0f)}
	};


	glGenBuffers(1, &IVertexesQuad);
	glBindBuffer(GL_ARRAY_BUFFER, IVertexesQuad);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(_text_vert), Vertexes, GL_STATIC_DRAW);

	word Indexes[] = { 2,1,0,2,0,3 };

	glGenBuffers(1, &IIndexesQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IIndexesQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(word), Indexes, GL_STATIC_DRAW);

	AllockFonts(fonts_num);

	TmpBuffer = new char[1024];
}

void TextBase::AllockFonts(int fonts_num)
{
	FontsNum = fonts_num;

	ppFonts = new cFont* [FontsNum];

	for(int i = 0; i < FontsNum; ++i)
	{
		ppFonts[i] = 0;
	}
}

void TextBase::Release()
{
	for(int i = 0; i < FontsNum; ++i)
	{
		if(ppFonts[i]){
			delete ppFonts[i];
		}
	}

	delete[] ppFonts;
	delete[] TmpBuffer;
}

void TextBase::LoadFont(uint FontId, const char* FileName, int SubFontTexturesNum)
{
	if((ppFonts[FontId] = LoadFontBF(FileName)) == (cFont*)-1)return;

	ppFonts[FontId]->FontTextures = new cFont::_font_tex[SubFontTexturesNum];

	for(int i = 0; i < SubFontTexturesNum; ++i)
	{
		ppFonts[FontId]->FontTextures[i].pTexture = new cTexture();
	}
}

cTexture* TextBase::CreateFontTextuteColorMask(int FontId, int SubFontTextureId, uchar r, uchar g, uchar b)
{
	ppFonts[FontId]->FontTextures[SubFontTextureId].buff = new uchar[(ppFonts[FontId]->FontSurf.w * ppFonts[FontId]->FontSurf.h) * 4];

	int tpi = 0;
	for(int bi = 0; bi<int(ppFonts[FontId]->FontSurf.w * ppFonts[FontId]->FontSurf.h); ++bi)
	{
		if(ppFonts[FontId]->FontSurf.buff[bi])
		{
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 0] = r;
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 1] = g;
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 2] = b;
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 3] = 255;
		}
		else{
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 0] = 0;
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 1] = 0;
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 2] = 0;
			ppFonts[FontId]->FontTextures[SubFontTextureId].buff[tpi + 3] = 0;
		}

		tpi += 4;
	}

	ppFonts[FontId]->FontTextures[SubFontTextureId].w = ppFonts[FontId]->FontSurf.w;
	ppFonts[FontId]->FontTextures[SubFontTextureId].h = ppFonts[FontId]->FontSurf.h;

	glGenTextures(1, &ppFonts[FontId]->FontTextures[SubFontTextureId].pTexture->textureID);

	glBindTexture(GL_TEXTURE_2D, ppFonts[FontId]->FontTextures[SubFontTextureId].pTexture->textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ppFonts[FontId]->FontTextures[SubFontTextureId].w, ppFonts[FontId]->FontTextures[SubFontTextureId].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ppFonts[FontId]->FontTextures[SubFontTextureId].buff);

	glBindTexture(GL_TEXTURE_2D, 0);
	return 0;
}

bool TextBase::FontToTextureStretchRectColorMask(_FONTSURF* pSurfFrom, int fx, int fy, int fw, int fh, _IMGSURF* pSurfTo, int tx, int ty, uchar r, uchar g, uchar b)
{
	if(tx > (int)pSurfTo->w){
		tx = (int)pSurfTo->w;
	}
	else if(tx < 0){
		fx -= tx;
		fw += tx;
		tx = 0;
	}

	if(ty > (int)pSurfTo->h){
		ty = (int)pSurfTo->h;
	}
	else if(ty < 0){
		fy -= ty;
		fh += ty;
		ty = 0;
	}

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

	//printf("%d %d\n",fy,(int)pSurfFrom->h);

	uchar* ofsfrom = (&pSurfFrom->buff[pSurfFrom->w * (pSurfFrom->h - 1)]);
	uchar* ofsto = (&pSurfTo->buff[(pSurfTo->w * (pSurfTo->h - 1)) * pSurfTo->pixel_size]);

	int colsize = fw;
	int coloor = (tx + fw) - pSurfTo->w;

	int rowsize = fh;
	int rowoor = (ty + fh) - pSurfTo->h;

	if(coloor > 0)
	{
		colsize -= coloor;
	}

	if(rowoor > 0)
	{
		rowsize -= rowoor;
	}

	ofsfrom -= (pSurfFrom->w) * fy;
	//if(ofsfrom<pSurfFrom->buff)ofsfrom=pSurfFrom->buff;

	ofsto -= (pSurfTo->w * pSurfTo->pixel_size) * ty;
	//if(ofsto<pSurfTo->buff)ofsto=pSurfTo->buff;

	for(int hi = 0; hi < rowsize; ++hi)
	{
		uchar* rowfrom = ofsfrom;
		uchar* rowto = ofsto;

		rowfrom += fx;
		rowto += tx * pSurfTo->pixel_size;

		int ti = 0;
		for(int pi = 0; pi < colsize; ++pi)
		{
			if(rowfrom[pi])
			{
				rowto[ti + 0] = r;
				rowto[ti + 1] = g;
				rowto[ti + 2] = b;
				rowto[ti + 3] = 255;
			}
			else{
				rowto[ti + 0] = 0;
				rowto[ti + 1] = 0;
				rowto[ti + 2] = 0;
				rowto[ti + 3] = 0;
			}

			ti += pSurfTo->pixel_size;
		}

		ofsfrom -= pSurfFrom->w;
		ofsto -= pSurfTo->w * pSurfTo->pixel_size;
	}

	return true;
}

int TextBase::ClearTexturePixelBufferColorMask(uint FontId, _IMGSURF* DestSurf, uint Xofs, uint Yofs, char* buff, int len, const char* color)
{
	_FONT_CHAR* pCharacters = ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf = &ppFonts[FontId]->FontSurf;

	int xpos = Xofs;
	int ypos = Yofs;
	for(int i = 0; i < len; ++i)
	{
		int cid = uchar(buff[i]) - 32;

		if(cid > -1)
		{
			if(cid < ppFonts[FontId]->CharNum)
			{
				DestSurf->ClearRect(
					xpos,
					ypos,
					(int)(pCharacters[cid].ofsx + pCharacters[cid].w) + ppFonts[FontId]->CharSpace,
					(int)(pCharacters[cid].ofsy + pCharacters[cid].h),
					(char*)color
				);
			}

			xpos += pCharacters[cid].w + pCharacters[cid].ofsx + ppFonts[FontId]->CharSpace;
		}
	}
	return 0;
}

int TextBase::PrintToTexturePixelBufferColorMask(uint FontId, _IMGSURF* DestSurf, uint Xofs, uint Yofs, char* buff, int len, uchar r, uchar g, uchar b, bool clear)
{
	_FONT_CHAR* pCharacters = ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf = &ppFonts[FontId]->FontSurf;

	//memset(&ToSurf->buff[Xofs],0,(ToSurf->w*ToSurf->h)*ToSurf->pixel_size);

	int xpos = Xofs;
	int ypos = Yofs;
	for(int i = 0; i < len; ++i)
	{
		int cid = uchar(buff[i]) - 32;

		if(cid > -1)
		{
			if(cid < ppFonts[FontId]->CharNum)
			{
				if(clear){
					DestSurf->ClearRect(
						xpos,
						ypos,
						(pCharacters[cid].ofsx + pCharacters[cid].w) + ppFonts[FontId]->CharSpace,
						(pCharacters[cid].ofsy + pCharacters[cid].h)
					);
				}

				FontToTextureStretchRectColorMask(
					FromSurf,
					pCharacters[cid].x, pCharacters[cid].y,
					pCharacters[cid].w, pCharacters[cid].h,
					DestSurf,
					xpos + pCharacters[cid].ofsx, ypos + pCharacters[cid].ofsy,
					r, g, b
				);
			}

			xpos += pCharacters[cid].w + pCharacters[cid].ofsx + ppFonts[FontId]->CharSpace;
		}
		/*
		switch(cid)
		{
			case 0:
				//xpos+=TextBase.ppFonts[FontId]->w;
			break;
			case -22:
				ypos+=TextBase.ppFonts[FontId]->h;
				xpos=Xofs;
			break;
		}
		 */
	}
	return 0;
}

int TextBase::PrintToTexture(uint FontId, _IMGSURF* DestSurf, uint TextureId, uint Xofs, uint Yofs, char* buff, int len)
{
	PrintToTexturePixelBufferColorMask(FontId, DestSurf, Xofs, Yofs, buff, len, 0, 0, 0);

	glBindTexture(GL_TEXTURE_2D, TextureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DestSurf->w, DestSurf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, DestSurf->buff);

	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}

void TextBase::SizeOfText(uint FontId, char* buff, int len, float* pretw, float* preth)
{
	if(!pretw || !preth)return;

	_FONT_CHAR* pCharacters = ppFonts[FontId]->pCharacters;

	int w = 0;
	int h = 0;
	for(int i = 0; i < len; ++i)
	{
		int cid = uchar(buff[i]) - 32;

		if(cid > -1)
		{
			w += pCharacters[cid].w + pCharacters[cid].ofsx + ppFonts[FontId]->CharSpace;

			if(h < pCharacters[cid].h + pCharacters[cid].ofsy)h = pCharacters[cid].h + pCharacters[cid].ofsy;
		}
	}

	*pretw = w;
	*preth = h;
}

_TextItem::_TextItem()
{
	data = 0;
	len = 0;
	TexCoordCount = 0;
	vTexCoord = 0;
	changed = true;
	FontId = 0;

	Align = _TEXT_ALIGN_LEFT;
}

_TextItem::~_TextItem()
{
	if(data)delete[] data;
	if(vTexCoord)delete[] vTexCoord;
}

cFont::cFont()
{
	CharSpace = 0;
	CharNum = 0;
	pTexCoord = 0;
	w = h = 0;
	pCharacters = 0;
	FontTextures = 0;
}

cFont::~cFont()
{
	if(pTexCoord){
		delete[] pTexCoord;
	}

	if(FontTextures)
	{
		delete[] FontTextures;
	}

	if(pCharacters){
		delete[] pCharacters;
	}
}

void _TextItem::SetText(char* str)
{
	int l = strlen(str);
	if(len != l){
		if(data)delete[] data;
		if(vTexCoord)delete[] vTexCoord;

		len = l;
		TexCoordCount = len * 2;

		data = new uchar[len + 1];
		vTexCoord = new vec4[TexCoordCount];
	}

	memcpy(data, str, len + 1);
	data[len] = 0;

	changed = true;
}

void _TextItem::SPrintF(const char* format, ...)
{
	va_list marker;
	va_start(marker, format);
	vsprintf(TextBase::TmpBuffer, format, marker);
	SetText(TextBase::TmpBuffer);
	va_end(marker);
}

void _TextItem::Update()
{
	if(changed)
	{
		for(uint i = 0; i < len; ++i)
		{
			uint ofs = i * 2;
			uchar ccode = data[i] - 32;
			//printf("%c\n", data[i]);
			//printf("%d\n",ccode);

			if(ccode > 159 && ccode < 224)
			{
				ccode -= 59;
			}

			if(ccode < TextBase::ppFonts[FontId]->CharNum)
			{
				//uint id = 0;
				//memcpy(&id, &ccode, 1);

				vTexCoord[ofs + 0] = TextBase::ppFonts[FontId]->pTexCoord[ccode].a;
				vTexCoord[ofs + 1] = TextBase::ppFonts[FontId]->pTexCoord[ccode].b;

				//printf("[%f %f] [%f %f]\n", vTexCoord[ofs + 0].x, vTexCoord[ofs + 0].z, vTexCoord[ofs + 1].x, vTexCoord[ofs + 1].z);
				//printf("%d\n",pTextItem->data[i]);
			}
		}
		changed = false;
	}
}
/*
void cTextFly::Init(int fid)
{
	FontId = fid;

	cMesh* pMesh = FontCreateGeometryVertexes(512);
	
	pMesh->shader_controller.AddShader(pMainShadersContainer);
	pMesh->shader_controller.AddTech(pMainShadersContainer, pMainShadersContainer->GetTechniqueByName((char*)"tech_text"));

	pMesh->shader_controller.AddValue(
		pMainShadersContainer,
		pMainShadersContainer->GetTechniqueByName((char*)"tech_text"),
		0,
		_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
		&TextBase.ppFonts[0]->FontTextures[0].pTexture->textureID,
		1
	);

	pTextObject = new cObjectModel();

	pTextObject->AddMesh(pMesh);

	pTextObject->SetupShaderValues();
}

_TextItem* cTextFly::AddLine(uint Count, uint FontId)
{
	_TextItem* ptext = new _TextItem();

	ptext->FontId = FontId;

	ptext->len = 0;

	ptext->count = Count;

	ptext->data = new uchar[Count];
	ptext->vTexCoord = new vec4[Count * 2];

	TextList.push(ptext);

	return ptext;
}

void cTextFly::Update(_TextItem* pTextItem)
{
	if(pTextItem->changed)
	{
		if(pTextItem->len < pTextItem->count)
		{
			for(uint i = 0; i < pTextItem->len; ++i)
			{
				uint ofs = i * 2;
				uchar ccode = pTextItem->data[i] - 32;

				if(ccode > 159 && ccode < 224)
				{
					ccode -= 59;
				}

				if(ccode < TextBase.ppFonts[pTextItem->FontId]->CharNum)
				{
					uint id = 0;
					memcpy(&id, &ccode, 1);

					pTextItem->vTexCoord[ofs + 0] = TextBase.ppFonts[pTextItem->FontId]->pTexCoord[id].a;
					pTextItem->vTexCoord[ofs + 1] = TextBase.ppFonts[pTextItem->FontId]->pTexCoord[id].b;
				}
			}
		}
		pTextItem->changed = false;
	}
}
/*
void cTextFly::Render(_TextItem* pTextItem)
{
	Update(pTextItem);

	mat4 CameraInverseMx = Camera.ViewMx;
	CameraInverseMx[3][0] = ((float(pTextItem->len) * (TextBase.ppFonts[pTextItem->FontId]->w / 25.0f)) / 2.0f);
	CameraInverseMx[3][1] = 0.0f;
	CameraInverseMx[3][2] = 0.0f;
	CameraInverseMx=inverse(CameraInverseMx);

	mat4 WorldMx = pTextItem->Transform;

	mat4 ViewProjwordMx = (CameraInverseMx * WorldMx) * Camera.ProjViewMx;

	vec4 FontInfo = vec4(TextBase.ppFonts[pTextItem->FontId]->w / 50.0f, 1.0f, 1.0f, 1.0f);

	_shader_list* curshader = IShaders.pShaderList;

	while(curshader->next)
	{
		if(curshader->ShaderID != -1)
		{
			c_shader* pshader = &G_SHADER[curshader->ShaderID];

			_shader_tech_list* curtech = curshader->pTechList;
			//printf("%s\n",pshader->Name);
			//printf("	%d\n",curtech->TechID);
			while(curtech->next)
			{
				_shader_select_values_list* cursel_modelval = curtech->select_model_values;

				while(cursel_modelval->next)
				{
					c_shader_value* pval = cursel_modelval->pval;
					//printf("	%s [%d]",pval->Name,pval->Semantic);
					switch(pval->Semantic)
					{
						case VALSEM_WORLDVIEWPROJMX:
							pshader->SetMatrix(pval->GId, &ViewProjwordMx);
							break;
						case VALSEM_WORLDMX:
							pshader->SetMatrix(pval->GId, pTextItem->Transform.pWorldMx);
							break;
						case VALSEM_VEC4_MODEL_A:
						case VALSEM_VEC4ARRAYL1:
							pshader->SetVectorArray(pval->GId, pTextItem->vTexCoord, pTextItem->len * 2);
							break;
						default:
							printf("switch default: valuesLObject:%d %s (cTextFly::Render)\n", pval->Semantic, pval->Name);
					}
					cursel_modelval = cursel_modelval->next;
				}

				_shader_select_values_list* cursel_meshval = curtech->select_mesh_values;

				while(cursel_meshval->next)
				{
					c_shader_value* pval = cursel_meshval->pval;

					if(pval)
					{
						//printf("	%s [%d]",pval->Name,pval->Semantic);
						switch(pval->Semantic)
						{
							case VALSEM_SAMPLER0:
								pshader->SetTexture(pval->GId, TextBase.ppFonts[pTextItem->FontId]->FontTextures[0].pTexture->textureID);
							case VALSEM_WEIGHTS://font info
								pshader->SetVectorArray(pval->GId, &FontInfo, 1);
								break;
							default:
								printf("switch default: valuesL_mesh:%x %s (cTextFly::Render)", pval->Semantic, pval->Name);
						}
					}

					cursel_meshval = cursel_meshval->next;
				}

				pshader->SetVertexBuffer(pMesh->IGLBidV);
				pshader->SetIndexBuffer(pMesh->IGLBidI);

				pshader->Begin(curtech->TechID, pTextItem->len * 4, pTextItem->len * 6, _VERTTYPE_A);

				curtech = curtech->next;
			}
		}

		curshader = curshader->next;
	}
}

void cTextFly::Draw()
{
	TextList.reset_enumerator();
	while(_TextItem* text = TextList.next())
	{
	}
}*/