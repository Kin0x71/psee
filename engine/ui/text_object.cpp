#include "text_object.h"
#include <engine/engine.h>
#include <engine/debug/primitives.h>

cMesh* FontCreateGeometryVertexes(int CellsCount)
{
	printf("FontCreateGeometryVertexes(%d)\n", CellsCount);

	uint VertNum = CellsCount * 4;
	uint IndNum = ((CellsCount * 2) * 3) + 1;

	TextBase::_font_vert* pVertBuff = new TextBase::_font_vert[VertNum];
	word* pIndexes = new word[IndNum];

	float SizeX = 2.0f;
	float SizeY = 1.0f;
	float HalfSizeX = SizeX / 2.0f;
	float HalfSizeY = SizeY / 2.0f;

	vec3 vp(HalfSizeX, HalfSizeY, 0);

	uint vid = 0;
	uint iid = 0;

	for(int ci = 0; ci < CellsCount; ++ci)
	{
		int vid0 = vid;
		int vid1 = vid + 1;
		int vid2 = vid + 2;
		int vid3 = vid + 3;

		float offs = (float)ci;

		pVertBuff[vid0].pos.x = vp.x - HalfSizeX;
		pVertBuff[vid0].pos.y = vp.y - HalfSizeY;
		pVertBuff[vid0].pos.z = vp.z;
		pVertBuff[vid0].normal.x = offs;
		pVertBuff[vid0].normal.y = 0.0f;
		pVertBuff[vid0].normal.z = 0.0f;
		pVertBuff[vid0].uv.x = 0.0f;
		pVertBuff[vid0].uv.y = 0.0f;

		pVertBuff[vid1].pos.x = vp.x - HalfSizeX;
		pVertBuff[vid1].pos.y = vp.y + HalfSizeY;
		pVertBuff[vid1].pos.z = vp.z;
		pVertBuff[vid1].normal.x = offs;
		pVertBuff[vid1].normal.y = 1.0f;
		pVertBuff[vid1].normal.z = 0.0f;
		pVertBuff[vid1].uv.x = 0.0f;
		pVertBuff[vid1].uv.y = 1.0f;

		pVertBuff[vid2].pos.x = vp.x + HalfSizeX;
		pVertBuff[vid2].pos.y = vp.y + HalfSizeY;
		pVertBuff[vid2].pos.z = vp.z;
		pVertBuff[vid2].normal.x = offs;
		pVertBuff[vid2].normal.y = 2.0f;
		pVertBuff[vid2].normal.z = 0.0f;
		pVertBuff[vid2].uv.x = 1.0f;
		pVertBuff[vid2].uv.y = 1.0f;

		pVertBuff[vid3].pos.x = vp.x + HalfSizeX;
		pVertBuff[vid3].pos.y = vp.y - HalfSizeY;
		pVertBuff[vid3].pos.z = vp.z;
		pVertBuff[vid3].normal.x = offs;
		pVertBuff[vid3].normal.y = 3.0f;
		pVertBuff[vid3].normal.z = 0.0f;
		pVertBuff[vid3].uv.x = 1.0f;
		pVertBuff[vid3].uv.y = 0.0f;

		pIndexes[iid++] = vid2;
		pIndexes[iid++] = vid1;
		pIndexes[iid++] = vid0;

		pIndexes[iid++] = vid3;
		pIndexes[iid++] = vid2;
		pIndexes[iid++] = vid0;

		//printf("[%d %d %d : %d %d %d]",pIndexes[vid-1],pIndexes[vid-2],pIndexes[vid-3],pIndexes[vid-4],pIndexes[vid-5],pIndexes[vid-6]);
		//разброс
		vp.x += 2.0f;

		vid += 4;
	}

	cMesh* pMesh = new cMesh();

	pMesh->vnum = VertNum;
	pMesh->inum = IndNum;
	pMesh->fnum = IndNum / 3;

	int use_items_count = 0;
	_VALUESEMANTIC_SPEC use_item_types[9];
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_VERTEX;
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_NORMAL;
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_TEXCOORD0;

	cMesh::init_vertex_def_container(use_item_types, use_items_count, &pMesh->vert_def);

	cMesh::init_vertexes(pMesh, pVertBuff, VertNum);
	cMesh::init_indexes(pMesh, pIndexes, IndNum);

	delete[] pVertBuff;
	pMesh->index_buff = pIndexes;
	//-------------------------------------------------------------

	pMesh->DefuseTextureId = 0;
	pMesh->NormalmapTextureId = 0;

	return pMesh;
}

cTextObject::cTextObject(bool sprite)
{
	is_sprite = sprite;

	set_name("object_text");

	TextModel = new cModel();
	TextModel->set_name("text_model");

	AddModel(TextModel);
}

int cTextObject::AddRow(int max_cells, vec3 color)
{
	cMesh* pMesh = FontCreateGeometryVertexes(max_cells);

	pMesh->set_name("text_mesh");

	pMesh->DefuseTextureId = 0;
	pMesh->NormalmapTextureId = 0;

	_TextItem* pTextItem = new _TextItem();

	pTextItem->FontInfo = vec4(TextBase::ppFonts[pTextItem->FontId]->w / 50.0f, color.r, color.g, color.b);

	int tech_id = Engine::pMainShadersContainer->GetTechniqueByName((char*)"tech_text");

	pMesh->shader_controller.AddShader(Engine::pMainShadersContainer);
	pMesh->shader_controller.AddTech(Engine::pMainShadersContainer, tech_id);

	pMesh->shader_controller.AddValue(
		Engine::pMainShadersContainer,
		tech_id,
		0,
		_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
		&TextBase::ppFonts[pTextItem->FontId]->FontTextures[0].pTexture->textureID
	);

	pMesh->shader_controller.AddValue(
		Engine::pMainShadersContainer,
		tech_id,
		0,
		_VALUESEMANTIC_SPEC::VALSEM_MESH_REF_VEC4_A,
		&pTextItem->vTexCoord,
		&pTextItem->TexCoordCount
	);

	pMesh->shader_controller.AddValue(
		Engine::pMainShadersContainer,
		tech_id,
		0,
		_VALUESEMANTIC_SPEC::VALSEM_WEIGHTS,
		(void*)&pTextItem->FontInfo
	);

	TextModel->AddMesh(pMesh);

	TextModel->SetupShaderValues(true, false, false);

	int line_id = TextLines.push(pTextItem);

	pMesh->mesh_localmx[3][1] = -line_id;

	return line_id;
}

void cTextObject::SetText(int line_id, const char* str)
{
	if(line_id > TextLines.count)return;

	TextLines[line_id]->SetText((char*)str);
	TextLines[line_id]->Update();
}

void cTextObject::Update()
{
	TextLines.reset_enumerator();
	int i = 0;
	while(_TextItem* text_item = TextLines.next())
	{
		text_item->Update();

		models_list[0]->mesh_list[i]->inum = text_item->len * 2 * 3;

		glDeleteBuffers(1, &models_list[0]->mesh_list[i]->IGLBidI);

		cMesh::init_indexes(models_list[0]->mesh_list[i], models_list[0]->mesh_list[i]->index_buff, models_list[0]->mesh_list[i]->inum);

		++i;
	}
}

void cTextObject::Draw(cCamera* pCamera)
{
	mat4 worldmx = WorldMx;

	if(is_sprite)
	{
		int max_len = 0;
		float max_w = 0.0f;
		TextLines.reset_enumerator();
		while(_TextItem* ti = TextLines.next())
		{
			if(ti->len > max_len)max_len = ti->len;

			if(TextBase::ppFonts[ti->FontId]->w > max_w)max_w = TextBase::ppFonts[ti->FontId]->w;
		}

		float s = scaling.x < 1.0f ? 1.0f + scaling.x : 1.0f;

		mat4 CameraInverseMx = pCamera->ViewMx;
		CameraInverseMx[3][0] = ((float(max_len) * (max_w / 25.0f)) / 2.0f);
		CameraInverseMx[3][1] = 0.0f;
		CameraInverseMx[3][2] = 0.0f;
		CameraInverseMx = inverse(CameraInverseMx);

		vec3 pos = vec3(worldmx[3][0], worldmx[3][1], worldmx[3][2]);

		//mat4 ViewProjwordMx = (CameraInverseMx * worldmx) * pCamera->ProjViewMx;

		worldmx[3][0] = pos.x;
		worldmx[3][1] = pos.y + (1.0f * (TextLines.count - 1)) * scaling.y;
		worldmx[3][2] = pos.z;

		worldmx *= CameraInverseMx;
	}

	int i = 0;
	models_list.reset_enumerator();
	while(cModel* pmodel = models_list.next())
	{
		pmodel->RenderModel(pCamera, &worldmx);
		DrawCross(vec3(WorldMx[3]),0.3f,"\xff\x00\x00");

		++i;
	}
}