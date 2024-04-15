
#include "mesh_file.h"
#include "file.h"
#include <engine/textures/texture.h>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

using namespace data_files;

extern _file_version _CHECKER_FILEVER_MESH;
extern _file_version _CHECKER_FILEVER_SKELETON;
extern _file_version _CHECKER_FILEVER_ANIMATION;

bool LoadMeshlVss(const char* fname, cList<cMesh*>* pret_mesh_list)
{
	//printf("LoadModelVss(%s)\n", fname);
	_FILE* pf = files::pFilesObject->GetFile(fname);
	
	if(pf==0){
		printf("ERROR file not found %s\n",fname);
		return false;
	}

	_file_version checkfile={0,0,0,0,0,0,0};

	FileRead(checkfile.format,4,pf);
	FileRead(&checkfile.ver,4,pf);

	if(!_CHECKER_FILEVER_MESH.cmp(&checkfile)){
		printf("ERROR check version (file:%f!=%f) in file %s\n",checkfile.ver, _CHECKER_FILEVER_MESH.ver,fname);
		return false;
	}

	char path[1024];
	const char* fname_start_offs = strrchr(fname, '/');
	int path_len = fname_start_offs - fname;
	memcpy(path, fname, path_len);
	path[path_len] = 0;

	word textures_count = 0;
	FileRead(&textures_count, 2, pf);

	char** Texture_names = new char* [textures_count];
	cTexture** ppTextures = new cTexture * [textures_count];
	for(int i = 0; i < textures_count; ++i)
	{
		unsigned char texlen = 0;
		FileRead(&texlen, 1, pf);
		Texture_names[i] = new char[texlen + 1];
		FileRead(Texture_names[i], texlen, pf);
		Texture_names[i][texlen] = 0;

		//printf("Texture_names[%d]:%p:%s\n", i, Texture_names[i], Texture_names[i]);

		char tex_path[1024];
		sprintf(tex_path, "%s/%s", path, Texture_names[i]);
		//printf("Texture:%d %s\n", i, tex_path);
		ppTextures[i] = Textures::TextureCreateFromFilePNG(tex_path);
	}

	word meshes_count = 0;
	FileRead(&meshes_count, 2, pf);
	//printf("meshes_count:%d\n", meshes_count);

	for(int mi = 0; mi < meshes_count; ++mi)
	{
		cMesh* pMeshe = new cMesh();
		//printf("mesh ptr:%p\n", pMeshe);

		unsigned char nlen = 0;
		FileRead(&nlen, 1, pf);
		pMeshe->name = new char[nlen + 1];

		FileRead(pMeshe->name, nlen, pf);
		pMeshe->name[nlen] = 0;

		word TexId = 0;
		FileRead(&TexId, 2, pf);

		//printf("mesh %d name:%s TexId:%d\n", mi, pMeshes[mi].name, TexId);
		if(TexId != 65535){
			pMeshe->DefuseTextureId = TexId;
			//printf("\t set:%s %d\n", Texture_names[TexId], pMeshes[mi].pTextureD ? pMeshes[mi].pTextureD->textureID : -1);
		}

		uint flags = 0;
		FileRead(&flags, 4, pf);
		//printf("flags:%x\n", flags);

		vec3 vposition;
		vec3 vscale;
		quat qrotation;
		FileRead(&vposition, sizeof(vec3), pf);
		FileRead(&vscale, sizeof(vec3), pf);
		FileRead(&qrotation, sizeof(quat), pf);

		mat4 mx_transform =
			translate(mat4(1.0f), vposition) *
			mat4_cast(qrotation) *
			scale(mat4(1.0f), vscale);

		uint VertexesCount = 0;
		uint IndexesCount = 0;
		uint WeightsSize = 0;
		FileRead(&VertexesCount, 4, pf);
		FileRead(&IndexesCount, 4, pf);

		if(flags & MESH_FILE_SKIN){
			FileRead(&WeightsSize, 4, pf);
		}

		//printf("VertexesCount:%d\n", VertexesCount);
		//printf("IndexesCount:%d\n", IndexesCount);
		//printf("WeightsSize:%d\n", WeightsSize);

		void* out_vert_buffer = 0;
		word* pIndexes = 0;
		vec4* wbuff = 0;
		uint ofsA = 0;

		if(IndexesCount)
		{
			pIndexes = new word[IndexesCount];
			FileRead(pIndexes, IndexesCount * 2, pf);
		}

		int use_items_count = 0;
		_VALUESEMANTIC_SPEC use_item_types[9];
		use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_VERTEX;
		use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_NORMAL;
		use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_TEXCOORD0;

		if(flags & MESH_FILE_SKIN){
			use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_WEIGHTOFS;
			wbuff = new vec4[WeightsSize];
		}
		
		cMesh::init_vertex_def_container(use_item_types, use_items_count, &pMeshe->vert_def);
		
		out_vert_buffer = malloc(pMeshe->vert_def.struct_size * VertexesCount);

		uchar* vert_item_offset = (uchar*)out_vert_buffer;

		for(uint vert_index = 0; vert_index < VertexesCount; ++vert_index)
		{
			//printf("\tvert_index:%d\n", vert_index);
			for(int pi = 0; pi < use_items_count; ++pi)
			{
				if(use_item_types[pi] != _VALUESEMANTIC_SPEC::VALSEM_WEIGHTOFS)
				{
					FileRead(vert_item_offset, ShaderSemantics::vert_def[(int)use_item_types[pi]].part_size, pf);

					/*if(use_item_types[pi] == VALSEM_VERTEX){
						vec3* p_vpos = (vec3*)vert_item_offset;

						//(*p_vpos) = mat3(mx_transform) * (*p_vpos);
					}*/

					if(use_item_types[pi] == _VALUESEMANTIC_SPEC::VALSEM_NORMAL){
						
						/*vec3* p_vnormal = (vec3*)vert_item_offset;
						vec3 vn = *p_vnormal;
						p_vnormal->x = vn.x;
						p_vnormal->y = vn.z;
						p_vnormal->z = -vn.y;*/
						//*p_vnormal = -vn;
						//printf("%f %f %f\n", p_vnormal->x, p_vnormal->y, p_vnormal->z);
						//(*p_vpos) = mat3(mx_transform) * (*p_vpos);
					}
					
				}else{

					float weight_ofs = ofsA;
					memcpy(vert_item_offset, &weight_ofs, ShaderSemantics::vert_def[(int)_VALUESEMANTIC_SPEC::VALSEM_WEIGHTOFS].part_size);

					int bonenum = 0;
					FileRead(&bonenum, sizeof(word), pf);

					uint ofsB = 1;
					wbuff[ofsA][0] = (float)bonenum;

					for(int ib = 0; ib < bonenum; ++ib)
					{
						int BoneId = 0;
						float Weight = 0.0f;
						FileRead(&BoneId, sizeof(word), pf);
						FileRead(&Weight, sizeof(float), pf);

						wbuff[ofsA][ofsB] = (float)BoneId;

						ofsB++;
						if(ofsB == 4){
							ofsB = 0;
							ofsA++;
						}

						wbuff[ofsA][ofsB] = Weight;
						ofsB++;
						if(ofsB == 4){
							ofsB = 0;
							ofsA++;
						}

						if(ofsB >= 4){
							//DBG_LOG("ERROR 1 loading weight\n");
						}
					}
					ofsA++;
				}

				vert_item_offset += ShaderSemantics::vert_def[(int)use_item_types[pi]].part_size;
			}
		}

		pMeshe->vnum = VertexesCount;
		pMeshe->inum = IndexesCount;
		pMeshe->fnum = IndexesCount / 3;
		pMeshe->vert_buff = out_vert_buffer;
		pMeshe->index_buff = pIndexes;

		if(flags & MESH_FILE_SKIN)
		{
			cMesh::init_vertexes(pMeshe, out_vert_buffer, VertexesCount, wbuff, ofsA);
			//delete[] wbuff;
			pMeshe->wbuff = wbuff;
		}else{
			cMesh::init_vertexes(pMeshe, out_vert_buffer, VertexesCount);
		}

		cMesh::init_indexes(pMeshe, pIndexes, IndexesCount);

		pret_mesh_list->push(pMeshe);

		//delete[] pIndexes;
		//free(out_vert_buffer);
	}

	for(int i = 0; i < textures_count; ++i)
	{
		delete[] Texture_names[i];
	}

	delete[] Texture_names;
	delete[] ppTextures;

	FileClose(pf);

	return true;
}

_model_file_container* LoadModelVss(const char* fname, int Wrapp, int Filter)
{
	//printf("LoadModelVss(%s)\n", fname);
	_FILE* pf = files::pFilesObject->GetFile(fname);

	if(pf == 0){
		printf("ERROR file not found %s\n", fname);
		return 0;
	}

	_file_version checkfile = { 0,0,0,0,0,0,0 };

	FileRead(checkfile.format, 4, pf);
	FileRead(&checkfile.ver, 4, pf);

	if(!_CHECKER_FILEVER_MESH.cmp(&checkfile)){
		printf("ERROR check version (file:%f!=%f) in file %s\n", checkfile.ver, _CHECKER_FILEVER_MESH.ver, fname);
		return 0;
	}

	char path[1024];
	const char* fname_start_offs = strrchr(fname, '/');
	int path_len = fname_start_offs - fname;
	memcpy(path, fname, path_len);
	path[path_len] = 0;

	cModel* Model = new cModel();
	Model->set_name(&fname[path_len + 1]);

	uint model_flags = 0;
	FileRead(&model_flags, 4, pf);

	word textures_count = 0;
	FileRead(&textures_count, 2, pf);

	char** Texture_names = new char* [textures_count];

	Model->defuse_textures_count = textures_count;
	Model->ppdefuse_textures = new cTexture* [textures_count];

	for(int i = 0; i < textures_count; ++i)
	{
		unsigned char texlen = 0;
		FileRead(&texlen, 1, pf);
		Texture_names[i] = new char[texlen + 1];
		FileRead(Texture_names[i], texlen, pf);
		Texture_names[i][texlen] = 0;

		//printf("Texture_names[%d]:%p:%s\n", i, Texture_names[i], Texture_names[i]);

		char tex_path[1024];
		sprintf(tex_path, "%s/%s", path, Texture_names[i]);
		//printf("Texture:%d %s\n", i, tex_path);
		Model->ppdefuse_textures[i] = Textures::TextureCreateFromFilePNG(tex_path, Wrapp, Wrapp, Filter);
	}

	word meshes_count = 0;
	FileRead(&meshes_count, 2, pf);
	//printf("meshes_count:%d\n", meshes_count);

	for(int mi = 0; mi < meshes_count; ++mi)
	{
		cMesh* pMeshe = new cMesh();

		unsigned char nlen = 0;
		FileRead(&nlen, 1, pf);
		pMeshe->name = new char[nlen + 1];

		FileRead(pMeshe->name, nlen, pf);
		pMeshe->name[nlen] = 0;

		word TexId = 0;
		FileRead(&TexId, 2, pf);

		word NmapId = 0;
		FileRead(&NmapId, 2, pf);

		//printf("mesh %d name:%s TexId:%d\n", mi, pMeshe->name, TexId);
		if(TexId != 65535){
			pMeshe->DefuseTextureId = TexId;
			//printf("\t set:%s\n", Texture_names[TexId]);
		}

		if(NmapId != 65535){
			pMeshe->NormalmapTextureId = NmapId;
			//printf("\t set:%s\n", Texture_names[NmapId]);
		}

		uint mesh_flags = 0;
		FileRead(&mesh_flags, 4, pf);
		//printf("flags:%x\n", flags);

		vec3 vposition;
		vec3 vscale;
		quat qrotation;
		FileRead(&vposition, sizeof(vec3), pf);
		FileRead(&vscale, sizeof(vec3), pf);
		FileRead(&qrotation, sizeof(quat), pf);

		//vec3 vposition = vec3(tvposition.x, tvposition.z, tvposition.y);
		//vec3 vscale = vec3(tvscale.x, tvscale.z, tvscale.y);
		//quat qrotation = quat(tqrotation.w, -tqrotation.x, tqrotation.z, tqrotation.y);

		//qrotation = rotate(qrotation, pi<float>() / 2.0f, vec3(1.0f, 0.0f, 0.0f));

		mat4 localmx =
			translate(mat4(1.0f), vposition) *
			mat4_cast(qrotation) *
			scale(mat4(1.0f), vscale);

		pMeshe->mesh_localmx = localmx;

		uint VertexesCount = 0;
		uint IndexesCount = 0;
		uint WeightsSize = 0;
		FileRead(&VertexesCount, 4, pf);
		FileRead(&IndexesCount, 4, pf);

		if(mesh_flags & MESH_FILE_SKIN){
			FileRead(&WeightsSize, 4, pf);
		}

		//printf("VertexesCount:%d\n", VertexesCount);
		//printf("IndexesCount:%d\n", IndexesCount);
		//printf("WeightsSize:%d\n", WeightsSize);

		void* out_vert_buffer = 0;
		word* pIndexes = 0;
		vec4* wbuff = 0;
		uint ofsA = 0;

		if(IndexesCount)
		{
			pIndexes = new word[IndexesCount];
			FileRead(pIndexes, IndexesCount * 2, pf);
		}

		int use_items_count = 0;
		_VALUESEMANTIC_SPEC use_item_types[9];
		use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_VERTEX;
		use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_NORMAL;
		use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_TEXCOORD0;

		if(mesh_flags & MESH_FILE_SKIN){
			use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_WEIGHTOFS;
			wbuff = new vec4[WeightsSize];
		}

		cMesh::init_vertex_def_container(use_item_types, use_items_count, &pMeshe->vert_def);

		out_vert_buffer = malloc(pMeshe->vert_def.struct_size * VertexesCount);

		uchar* vert_item_offset = (uchar*)out_vert_buffer;

		for(uint vert_index = 0; vert_index < VertexesCount; ++vert_index)
		{
			//printf("\tvert_index:%d\n", vert_index);
			for(int pi = 0; pi < use_items_count; ++pi)
			{
				if(use_item_types[pi] != _VALUESEMANTIC_SPEC::VALSEM_WEIGHTOFS)
				{
					FileRead(vert_item_offset, ShaderSemantics::vert_def[(int)use_item_types[pi]].part_size, pf);
				}
				else{

					float weight_ofs = ofsA;
					memcpy(vert_item_offset, &weight_ofs, ShaderSemantics::vert_def[(int)_VALUESEMANTIC_SPEC::VALSEM_WEIGHTOFS].part_size);

					int bonenum = 0;
					FileRead(&bonenum, sizeof(word), pf);

					uint ofsB = 1;
					wbuff[ofsA][0] = (float)bonenum;

					for(int ib = 0; ib < bonenum; ++ib)
					{
						int BoneId = 0;
						float Weight = 0.0f;
						FileRead(&BoneId, sizeof(word), pf);
						FileRead(&Weight, sizeof(float), pf);

						wbuff[ofsA][ofsB] = (float)BoneId;

						ofsB++;
						if(ofsB == 4){
							ofsB = 0;
							ofsA++;
						}

						wbuff[ofsA][ofsB] = Weight;
						ofsB++;
						if(ofsB == 4){
							ofsB = 0;
							ofsA++;
						}

						if(ofsB >= 4){
							//DBG_LOG("ERROR 1 loading weight\n");
						}
					}
					ofsA++;
				}

				vert_item_offset += ShaderSemantics::vert_def[(int)use_item_types[pi]].part_size;
			}
		}

		pMeshe->vnum = VertexesCount;
		pMeshe->inum = IndexesCount;
		pMeshe->fnum = IndexesCount / 3;
		pMeshe->vert_buff = out_vert_buffer;
		pMeshe->index_buff = pIndexes;

		if(mesh_flags & MESH_FILE_SKIN)
		{
			cMesh::init_vertexes(pMeshe, out_vert_buffer, VertexesCount, wbuff, ofsA);
			//delete[] wbuff;
			pMeshe->wbuff = wbuff;
		}
		else{
			cMesh::init_vertexes(pMeshe, out_vert_buffer, VertexesCount);
		}

		cMesh::init_indexes(pMeshe, pIndexes, IndexesCount);

		Model->AddMesh(pMeshe);

		//delete[] pIndexes;
		//free(out_vert_buffer);
	}

	for(int i = 0; i < textures_count; ++i)
	{
		delete[] Texture_names[i];
	}

	delete[] Texture_names;

	_model_file_container* ret_container = new _model_file_container();

	ret_container->model = Model;

	if(model_flags & MODEL_FILE_COLLISION_MESH)
	{
		ret_container->collide_mesh = new _model_file_container::_collide_mesh();
		FileRead(&ret_container->collide_mesh->vertices_count, 4, pf);
		FileRead(&ret_container->collide_mesh->indexes_count, 4, pf);

		ret_container->collide_mesh->vertices = new vec3[ret_container->collide_mesh->vertices_count];
		ret_container->collide_mesh->indexes = new word[ret_container->collide_mesh->indexes_count];

		FileRead(ret_container->collide_mesh->vertices, ret_container->collide_mesh->vertices_count * sizeof(vec3), pf);
		FileRead(ret_container->collide_mesh->indexes, ret_container->collide_mesh->indexes_count * 2, pf);
	}

	FileClose(pf);

	return ret_container;
}

cSkeleton* LoadSkeletonKbh(const char *fname)
{
	printf("LoadSkeletonKbh(%s)\n",fname);

	_FILE* pf = files::pFilesObject->GetFile(fname);

	if(pf==0)return 0;

	_file_version checkfile={0,0,0,0,0,0,0};

	FileRead(checkfile.format,4,pf);
	FileRead(&checkfile.ver,4,pf);

	if(!_CHECKER_FILEVER_SKELETON.cmp(&checkfile)){
		printf("ERROR check version in file %s\n",fname);
		return 0;
	}

	word BonesCount = 0;
	FileRead(&BonesCount, 2, pf);

	//DBG_LOG("BonesCount:%d\n",BonesCount);

	cSkeleton* pSkeleton = new cSkeleton(BonesCount);

	cBone* pTree = pSkeleton->Bones;

	char** ppNames=new char*[BonesCount];
	word* pParents=new word[BonesCount];

	for(int i = 0; i < BonesCount; ++i)
	{
		unsigned char nlen = 0;
		FileRead(&nlen, 1, pf);
		ppNames[i] = new char[nlen + 1];

		FileRead(ppNames[i], nlen, pf);
		ppNames[i][nlen] = 0;

		FileRead(&pParents[i], 2, pf);

		//printf("%d BoneName:%s parent:%d\n", i, ppNames[i], pParents[i]);

		vec3 VWP;
		vec3 VP;
		quat QR;
		vec3 VS;
		FileRead(&VWP, sizeof(vec3), pf);
		FileRead(&VP, sizeof(vec3), pf);
		FileRead(&QR, sizeof(quat), pf);
		FileRead(&VS, sizeof(vec3), pf);
		//printf("\tQR:%f %f %f %f\n", QR.x, QR.y, QR.z, QR.w);

		if(pParents[i] != 65535)
		{
			pTree[i].ParentId = pParents[i];

			if(!pTree[pParents[i]].pChildren){
				pTree[pParents[i]].pChildren = &pTree[i];
			}
			else{

				cBone* SiblingCur = pTree[pParents[i]].pChildren;

				while(SiblingCur->pSibling)
				{
					SiblingCur = SiblingCur->pSibling;
				}

				SiblingCur->pSibling = &pTree[i];
			}
		}
		else if(i){
			//на случай если какие-то части скелета в отдельной иерархии
			if(!pTree[0].pSibling){
				pTree[0].pSibling = &pTree[i];
			}
			else{
				cBone* SiblingCur = pTree[0].pSibling;

				while(SiblingCur->pSibling)
				{
					SiblingCur = SiblingCur->pSibling;
				}
				SiblingCur->pSibling = &pTree[i];
			}
		}

		pTree[i].Name = ppNames[i];
		pTree[i].Id = i;

		pTree[i].world_position =VWP;
		pTree[i].position = VP;
		pTree[i].rotation = QR;
		pTree[i].scale = VS;
	}
	
	delete[] ppNames;
	delete[] pParents;

	FileClose(pf);

	pSkeleton->CalculateWorldTransforms();

	return pSkeleton;
}

cAnimation* LoadAnimationAkf(const char* fname)
{
	//printf("LoadAnimationAkf(%s)\n",fname);

	_FILE* pf = files::pFilesObject->GetFile(fname);

	if(pf == 0)return 0;

	_file_version checkfile = { 0, 0, 0, 0, 0, 0, 0 };

	FileRead(checkfile.format, 4, pf);
	FileRead(&checkfile.ver, 4, pf);

	if(!_CHECKER_FILEVER_ANIMATION.cmp(&checkfile)){
		printf("ERROR check version in file %s\n", fname);
		return 0;
	}

	word FrameBonesCount = 0;
	float tStart = 0;
	float tEnd = 0;

	FileRead(&FrameBonesCount, 2, pf);
	FileRead(&tStart, 4, pf);
	FileRead(&tEnd, 4, pf);

	//printf("FramesCount:%d tStart:%d tEnd:%d\n", FramesCount, tStart, tEnd);

	cAnimation* Animation = new cAnimation(FrameBonesCount, tStart, tEnd);

	const char* name = strrchr(fname, '/') + 1;
	Animation->set_name((char*)name);

	_bone_frame* FrameBones = Animation->BonesFrames;

	float time_end = -1.0f;
	for(int bi = 0; bi < FrameBonesCount; ++bi)
	{
		word NodeId = 0;
		word KeyNum = 0;

		FileRead(&NodeId, 2, pf);
		FileRead(&KeyNum, 2, pf);
		//printf(" bone:%d KeyNum:%d\n", bi, KeyNum);

		cKeyP* KeysX = 0;
		cKeyP* KeysY = 0;
		cKeyP* KeysZ = 0;
		cKeyR* KeysR = 0;
		cKeyS* KeysS = 0;
		uint KeysXNum = 0;
		uint KeysYNum = 0;
		uint KeysZNum = 0;
		uint KeysRNum = 0;
		uint KeysSNum = 0;

		_FILE_KEY_LIST* KeyList=new _FILE_KEY_LIST[KeyNum];

		for(int ki = 0; ki < KeyNum; ++ki)
		{
			float kt = 0;
			unsigned short am = 0;

			FileRead(&kt, 4, pf);
			FileRead(&am, 2, pf);
			//printf("    key:%d time:%f am:[%x]\n", ki, kt, am);
			if(kt > time_end)time_end = kt;

			float PosX = 0;
			float PosY = 0;
			float PosZ = 0;
			quat Rotate = quat(0, 0, 0, 1);
			vec3 Scale = vec3(1, 1, 1);

			if(am & word(_ANIM_MOD::X))
			{
				FileRead(&PosX, sizeof(float), pf);
				//printf("    x:%f\n", PosX);
				++KeysXNum;
			}

			if(am & word(_ANIM_MOD::Y))
			{
				FileRead(&PosY, sizeof(float), pf);
				//printf("    y:%f\n", PosY);
				++KeysYNum;
			}

			if(am & word(_ANIM_MOD::Z))
			{
				FileRead(&PosZ, sizeof(float), pf);
				//printf("    z:%f\n", PosZ);
				++KeysZNum;
			}

			if(am & word(_ANIM_MOD::R))
			{
				FileRead(&Rotate, sizeof(quat), pf);
				//printf("    r:%f %f %f %f\n", Rotate.x, Rotate.y, Rotate.z, Rotate.w);
				++KeysRNum;
			}

			if(am & word(_ANIM_MOD::S))
			{
				FileRead(&Scale, sizeof(vec3), pf);
				//printf("    s:%f %f %f\n", Scale.x, Scale.y, Scale.z);
				++KeysSNum;
			}

			float tPosX = PosX;
			float tPosY = PosY;
			float tPosZ = PosZ;
			
			quat tRotate;

			tRotate.x = Rotate.x;
			tRotate.y = Rotate.y;
			tRotate.z = Rotate.z;
			tRotate.w = Rotate.w;

			KeyList[ki].AnimMod = am;
			KeyList[ki].Time = kt;
			KeyList[ki].position = vec3(tPosX, tPosY, tPosZ);
			KeyList[ki].scale = Scale;
			KeyList[ki].rotation = tRotate;
		}

		if(KeysXNum)KeysX = new cKeyP[KeysXNum];
		if(KeysYNum)KeysY = new cKeyP[KeysYNum];
		if(KeysZNum)KeysZ = new cKeyP[KeysZNum];
		if(KeysRNum)KeysR = new cKeyR[KeysRNum];
		if(KeysSNum)KeysS = new cKeyS[KeysSNum];

		uint ipx = 0;
		uint ipy = 0;
		uint ipz = 0;
		uint ir = 0;
		uint is = 0;

		for(int ki = 0; ki < KeyNum; ++ki)
		{
			if(KeyList[ki].AnimMod & word(_ANIM_MOD::X))
			{
				KeysX[ipx].time = KeyList[ki].Time;
				KeysX[ipx].v = KeyList[ki].position.x;
				++ipx;
			}

			if(KeyList[ki].AnimMod & word(_ANIM_MOD::Y))
			{
				KeysY[ipy].time = KeyList[ki].Time;
				KeysY[ipy].v = KeyList[ki].position.y;
				++ipy;
			}

			if(KeyList[ki].AnimMod & word(_ANIM_MOD::Z))
			{
				KeysZ[ipz].time = KeyList[ki].Time;
				KeysZ[ipz].v = KeyList[ki].position.z;
				++ipz;
			}

			if(KeyList[ki].AnimMod & word(_ANIM_MOD::R))
			{
				KeysR[ir].time = KeyList[ki].Time;
				KeysR[ir].v = KeyList[ki].rotation;

				//if(bi==23)printf("    k:%d t:%f r:%f %f %f %f\n",ir , KeysR[ir].Time, KeysR[ir].v.x, KeysR[ir].v.y, KeysR[ir].v.z, KeysR[ir].v.w);

				++ir;
			}

			if(KeyList[ki].AnimMod & word(_ANIM_MOD::S))
			{
				KeysS[is].time = KeyList[ki].Time;
				KeysS[is].v = KeyList[ki].scale;
				++is;
			}
		}

		FrameBones[bi].Id = bi;

		FrameBones[bi].KeysXNum = KeysXNum;
		FrameBones[bi].KeysYNum = KeysYNum;
		FrameBones[bi].KeysZNum = KeysZNum;
		FrameBones[bi].KeysRNum = KeysRNum;
		FrameBones[bi].KeysSNum = KeysSNum;

		FrameBones[bi].KeysX = KeysX;
		FrameBones[bi].KeysY = KeysY;
		FrameBones[bi].KeysZ = KeysZ;
		FrameBones[bi].KeysR = KeysR;
		FrameBones[bi].KeysS = KeysS;

		delete[] KeyList;
	}

	FileClose(pf);

	Animation->TimeEnd = time_end;

	Animation->_setup_default_frame();

	return Animation;
}
/*
cAnimation* LoadAnimationAbc(char* fname)
{
	_FILE* pf = files::pFilesObject->GetFile(fname);

	if(pf == 0)return 0;

	_file_version checkfile = { 0, 0, 0, 0, 0, 0, 0 };

	FileRead(checkfile.format, 4, pf);
	FileRead(&checkfile.ver, 4, pf);

	if(!_CHECKER_FILEVER_ANIMATION.cmp(&checkfile)){
		printf("ERROR check version in file %s\n", fname);
		return 0;
	}

	word BonesCount = 0;
	float tStart = 0;
	float tEnd = 0;

	FileRead(&BonesCount, 2, pf);
	FileRead(&tStart, 4, pf);
	FileRead(&tEnd, 4, pf);

	printf("FramesCount:%d tStart:%d tEnd:%d\n", BonesCount, tStart, tEnd);

	cAnimation* Animation = new cAnimation(BonesCount, tStart, tEnd);

	Animation->BoneCurves = new _bone_curves[BonesCount];
	_bone_curves* bone_curves = Animation->BoneCurves;

	for(int bi = 0; bi < BonesCount; ++bi)
	{
		FileRead(&bone_curves[bi].pxn, 4, pf);
		FileRead(&bone_curves[bi].pyn, 4, pf);
		FileRead(&bone_curves[bi].pzn, 4, pf);
		FileRead(&bone_curves[bi].rxn, 4, pf);
		FileRead(&bone_curves[bi].ryn, 4, pf);
		FileRead(&bone_curves[bi].rzn, 4, pf);
		FileRead(&bone_curves[bi].sxn, 4, pf);
		FileRead(&bone_curves[bi].syn, 4, pf);
		FileRead(&bone_curves[bi].szn, 4, pf);

		printf(
			"\tbid:%d [%d %d %d] [%d %d %d] [%d %d %d]\n",
			bi,
			bone_curves[bi].pxn, bone_curves[bi].pyn, bone_curves[bi].pzn,
			bone_curves[bi].rxn, bone_curves[bi].ryn, bone_curves[bi].rzn,
			bone_curves[bi].sxn, bone_curves[bi].syn, bone_curves[bi].szn
		);

		auto _axis_reader = [pf](int count){
			_curve* ret_curve = new _curve[count];
			for(int ci = 0; ci < count; ++ci)
			{
				FileRead(&ret_curve[ci].time, 4, pf);
				FileRead(&ret_curve[ci].value, 4, pf);

				printf(
					"\tk:%d t:%f v:%f\n",
					ci,
					ret_curve[ci].time,
					ret_curve[ci].value
				);
			}
			return ret_curve;
		};

		printf("PX\n");
		bone_curves[bi].px = _axis_reader(bone_curves[bi].pxn);
		printf("PY\n");
		bone_curves[bi].py = _axis_reader(bone_curves[bi].pyn);
		printf("PZ\n");
		bone_curves[bi].pz = _axis_reader(bone_curves[bi].pzn);

		printf("RX\n");
		bone_curves[bi].rx = _axis_reader(bone_curves[bi].rxn);
		printf("RY\n");
		bone_curves[bi].ry = _axis_reader(bone_curves[bi].ryn);
		printf("RZ\n");
		bone_curves[bi].rz = _axis_reader(bone_curves[bi].rzn);

		printf("SX\n");
		bone_curves[bi].sx = _axis_reader(bone_curves[bi].sxn);
		printf("SY\n");
		bone_curves[bi].sy = _axis_reader(bone_curves[bi].syn);
		printf("SZ\n");
		bone_curves[bi].sz = _axis_reader(bone_curves[bi].szn);
	}

	return Animation;
}*/