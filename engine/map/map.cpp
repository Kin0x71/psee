#include "map.h"
#include <engine/engine.h>
#include <engine/files/file.h>

#include <engine/ui/text_object.h>

#include <BulletDynamics/Featherstone/btMultiBodyLinkCollider.h>

#include <engine/debug/primitives.h>

#include <glm/gtx/compatibility.hpp>//vec3 lerp

using namespace data_files;
using namespace Engine;

extern cTextObject* DebugText;

cTexture* plandscape_texture = 0;
cTexture* plandscape_nm_texture = 0;

void CalcBinormalAndTangent(const vec3& normal, vec3* pbinormal, vec3* ptangent, const vec3& vc1, const vec3& vc2)
{
	vec3 vA;
	vec3 vB;
	vec3 c1 = normal;
	vec3 c2 = normal;
	c1 = cross(c1, vc1);//0.0, 0.0, 1.0
	c2 = cross(c2, vc2);//0.0, 1.0, 0.0
	//c1=Vec3Cross(c1,vec3(0.0, 0.0, -1.0));
	//c2=Vec3Cross(c2,vec3(0.0, 1.0, 0.0));

	if(c1.length() > c2.length())
	{
		vA = c1;
	}
	else
	{
		vA = c2;
	}

	vA = normalize(vA);

	vB = normal;
	vB = cross(vB, vA);
	vB = normalize(vB);

	//vB=-vB;

	//vA.y=-vA.y;

	*pbinormal = vA;
	*ptangent = vB;

	//printf("%f %f %f: %f %f %f\n",vA.x,vA.y,vA.z,vB.x,vB.y,vB.z);
}

void make_tangents(word* Indices, int IndicesCount, _plane_vert* p_plane_verts, int VertexesCount)
{
	for(int i = 0; i < VertexesCount; ++i){
		p_plane_verts[i].tangent = vec3(0.0f);
	}

	for(int i = 0; i < IndicesCount; ++i)
	{
		uint32_t l = Indices[i];
		uint32_t j = Indices[(i + 1) % 3 + i / 3 * 3];
		uint32_t k = Indices[(i + 2) % 3 + i/ 3 * 3];
		vec3 n = p_plane_verts[l].normal;
		vec3 v1 = p_plane_verts[j].pos - p_plane_verts[l].pos, v2 = p_plane_verts[k].pos - p_plane_verts[l].pos;
		vec2 t1 = p_plane_verts[j].tex - p_plane_verts[l].tex, t2 = p_plane_verts[k].tex - p_plane_verts[l].tex;

		float uv2xArea = t1.x * t2.y - t1.y * t2.x;
		if(std::abs(uv2xArea) < 0x1p-20)continue;

		float flip = uv2xArea > 0 ? 1 : -1;

		//if(tangents[l].w != 0 && tangents[l].w != -flip)++inconsistentUvs;

		//tangent_w[l] = -flip;

		v1 -= n * dot(v1, n);
		v2 -= n * dot(v2, n);
		vec3 s = normalize((t2.y * v1 - t1.y * v2) * flip);

		float angle = std::acos(dot(v1, v2) / (length(v1) * length(v2)));
		p_plane_verts[l].tangent += vec3(s * angle);
	}

	for(int i = 0; i < IndicesCount; ++i)
	{
		int vi = Indices[i];
		vec3& t = p_plane_verts[vi].tangent;
		t = normalize(t);

		p_plane_verts[vi].binormal = -normalize(
			cross(p_plane_verts[vi].normal, p_plane_verts[vi].tangent)
		);
	}
}
/*
void cMap::_zone_file::allock()
{
	walls = new _line[walls_count];
	p_tiles_vt = new vec4[tiles_vcount];
	p_tiles_i = new word[tiles_icount];
	plates = new _quad[plates_count];
	cells = new _cell_file[cells_count];
	joints = new int[joints_count * 2];
}

cMap::_zone_file::~_zone_file()
{
	delete[] walls;
	delete[] p_tiles_vt;
	delete[] p_tiles_i;
	delete[] plates;
	delete[] cells;
	delete[] joints;
}

cMap::_map_file::_map_file(int zc, int dc, _zone_file* pzf, _door_file* pd, const vec2 pp)
{
	zones_count = zc;
	doors_count = dc;
	zones = pzf;
	pdoors = pd;
	player_pos = pp;
}

cMap::_map_file::~_map_file()
{
	if(zones)delete[] zones;
	if(pdoors)delete[] pdoors;
}
*/

int cLandscapeObject::InitGeometrix(_plane_vert* vertices, int vertices_count, word* indexes, int indexes_count, btTriangleMesh* triangle_mesh, bool init_ogl_mesh)
{
	cMesh* pMesh = new cMesh();
	pMesh->set_name("landscepe");

	int use_items_count = 0;
	_VALUESEMANTIC_SPEC use_item_types[9];
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_VERTEX;
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_NORMAL;
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_BINORMAL;
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_TANGENT;
	use_item_types[use_items_count++] = _VALUESEMANTIC_SPEC::VALSEM_TEXCOORD0;

	cMesh::init_vertex_def_container(use_item_types, use_items_count, &pMesh->vert_def);

	make_tangents(indexes, indexes_count, vertices, vertices_count);

	pMesh->vnum = vertices_count;
	pMesh->inum = indexes_count;
	pMesh->fnum = indexes_count / 3;
	pMesh->vert_buff = vertices;
	pMesh->index_buff = indexes;

	//cMesh::init_vertexes(pMesh, vert_buffer, vertices_count);
	//cMesh::init_indexes(pMesh, indexes, indexes_count);

	cModel* pModel = new cModel();
	pModel->set_name("landscape");

	pModel->defuse_textures_count = 1;
	pModel->normalmap_textures_count = 1;
	pModel->ppdefuse_textures = new cTexture * [1];
	pModel->ppnormalmap_textures = new cTexture * [1];
	pModel->ppdefuse_textures[0] = plandscape_texture;
	pModel->ppnormalmap_textures[0] = plandscape_nm_texture;

	pMesh->DefuseTextureId = 0;
	pMesh->NormalmapTextureId = 0;

	pMesh->shader_controller.AddShader(pMainShadersContainer);
	pMesh->shader_controller.AddTech(pMainShadersContainer, pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"));

	pMesh->shader_controller.AddValue(
		pMainShadersContainer,
		pMainShadersContainer->GetTechniqueByName((char*)"tech_vertex"),
		0,
		_VALUESEMANTIC_SPEC::VALSEM_SAMPLER,
		&pModel->ppdefuse_textures[pMesh->DefuseTextureId]->textureID
	);

	if(init_ogl_mesh){
		cMesh::init_vertexes(pMesh, pMesh->vert_buff, pMesh->vnum);
		cMesh::init_indexes(pMesh, pMesh->index_buff, pMesh->inum);
	}

	pModel->AddMesh(pMesh);

	pModel->SetupShaderValues();

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triangle_mesh, true, true);

	btTransform SphereTransform;
	SphereTransform.setIdentity();

	btVector3 blocalInertia(0.0f, 0.0f, 0.0f);

	btRigidBody* RigidBody = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0.0f, new btDefaultMotionState(SphereTransform), shape, blocalInertia));

	RigidBody->setCollisionFlags(RigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	RigidBody->setFriction(1.0f);

	allocate_rigidbodys(1);
	CollideGroups[0] = COLLISION_GROUP_LANDSCAPE;
	CollideFilters[0] = COLLISION_GROUP_NOTHING;

	WorldMx = mat4(1.0f);
	set_name("Landscepe");
	is_collided = true;
	RigidBodys[0] = RigidBody;

	RigidBody->setUserPointer(this);

	AddModel(pModel);

	return 0;
}

void cHouseObject::InitCollision()
{
	btTriangleMesh* pTriangleMesh = new btTriangleMesh();

	models_list.reset_enumerator();
	while(cModel* model = models_list.next())
	{
		model->mesh_list.reset_enumerator();
		while(cMesh* mesh = model->mesh_list.next())
		{
			vec3* verts=new vec3[mesh->vnum];

			uchar* vert_item_offset = (uchar*)mesh->vert_buff;

			for(uint vi = 0; vi < mesh->vnum; ++vi)
			{
				for(int pi = 0; pi < mesh->vert_def.items_count; ++pi)
				{
					if(mesh->vert_def.use_items[pi] == _VALUESEMANTIC_SPEC::VALSEM_VERTEX){
						verts[vi] = *(vec3*)vert_item_offset;
					}

					vert_item_offset += ShaderSemantics::vert_def[(int)mesh->vert_def.use_items[pi]].part_size;
				}
			}

			for(uint index = 0; index < mesh->inum; index += 3)
			{
				btVector3 vertex_1(verts[mesh->index_buff[index]].x, verts[mesh->index_buff[index]].y, verts[mesh->index_buff[index]].z);
				btVector3 vertex_2(verts[mesh->index_buff[index + 1]].x, verts[mesh->index_buff[index + 1]].y, verts[mesh->index_buff[index + 1]].z);
				btVector3 vertex_3(verts[mesh->index_buff[index + 2]].x, verts[mesh->index_buff[index + 2]].y, verts[mesh->index_buff[index + 2]].z);

				//printf("(%f %f %f) (%f %f %f) (%f %f %f)\n", vertex_1.x(), vertex_1.y(), vertex_1.z(), vertex_2.x(), vertex_2.y(), vertex_2.z(), vertex_3.x(), vertex_3.y(), vertex_3.z());

				pTriangleMesh->addTriangle(vertex_1, vertex_2, vertex_3);
			}

			delete[] verts;
		}
	}

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(pTriangleMesh, true, true);

	btTransform SphereTransform;
	SphereTransform.setIdentity();

	btVector3 blocalInertia(0.0f, 0.0f, 0.0f);

	allocate_rigidbodys(1);

	CollideGroups[0] = COLLISION_GROUP_LANDSCAPE;
	CollideFilters[0] = COLLISION_GROUP_NOTHING;

	RigidBodys[0] = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0.0f, new btDefaultMotionState(SphereTransform), shape, blocalInertia));

	RigidBodys[0]->setCollisionFlags(RigidBodys[0]->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

	RigidBodys[0]->setFriction(1.0f);

	RigidBodys[0]->setUserPointer(this);

	is_collided = true;
}

cMap::~cMap()
{
	if(Name)delete[] Name;
}

void cMap::set_name(char* n)
{
	int nlen = strlen(n);
	Name = new char[nlen + 1];
	memcpy(Name, n, nlen);
	Name[nlen] = 0;
}

bool cMap::InitZones(const char* landscape_height_map_file, uint field_rows, uint field_cells)
{
	int nlen = strlen(landscape_height_map_file);
	LandscapeHeightMapFile = new char[nlen + 1];
	memcpy(LandscapeHeightMapFile, landscape_height_map_file, nlen);
	LandscapeHeightMapFile[nlen] = 0;

	Navigation.ZoneRows = field_rows;
	Navigation.ZoneCells = field_cells;

	FILE* pf = fopen(LandscapeHeightMapFile, "rb");

	if(!pf){
		printf("ERROR file not found %s\n", LandscapeHeightMapFile);
		return false;
	}

	fread(&FullMapWidth, 1, 4, pf);
	fread(&FullMapHeight, 1, 4, pf);

	printf("cMap::InitZones: FullMapWidth:%d FullMapHeight:%d\n", FullMapWidth, FullMapHeight);

	Navigation.MapRows = FullMapHeight / Navigation.ZoneRows;
	Navigation.MapCells = FullMapWidth / Navigation.ZoneCells;

	Navigation.map_nodes_count = Navigation.MapRows * Navigation.MapCells;
	Navigation.map_nodes = new _map_asnode[Navigation.map_nodes_count];

	Navigation.NavZones = new _map_asnode**[Navigation.MapRows];
	ViewZones = new _view_zone_item**[Navigation.MapRows];

	for(int ri = 0; ri < Navigation.MapRows; ++ri)
	{
		Navigation.NavZones[ri] = new _map_asnode*[Navigation.MapCells];
		ViewZones[ri]= new _view_zone_item*[Navigation.MapCells];

		for(int ci = 0; ci < Navigation.MapCells; ++ci)
		{
			Navigation.NavZones[ri][ci] = 0;
			ViewZones[ri][ci] = new _view_zone_item(
				_view_zone_item::_VIEW_ZONE_STAT::NON,
				vec2(ci * (Navigation.ZoneCells - 1), ri * (Navigation.ZoneRows - 1)),
				vec2(ci * (Navigation.ZoneCells - 1), (ri * (Navigation.ZoneRows - 1)) + Navigation.ZoneRows - 1),
				vec2((ci * (Navigation.ZoneCells - 1)) + Navigation.ZoneCells - 1, (ri * (Navigation.ZoneRows - 1)) + Navigation.ZoneRows - 1),
				vec2((ci * (Navigation.ZoneCells - 1)) + Navigation.ZoneCells - 1, ri * (Navigation.ZoneRows - 1))
			);

			ViewZones[ri][ci]->row_id = ri;
			ViewZones[ri][ci]->cell_id = ci;
			//printf("zone : [%d %d] : %p\n", ri, ci, Navigation.NavZones[ri][ci]);
		}
	}

	fclose(pf);

	//plandscape_texture = Textures::TextureCreateFromFilePNG((char*)"data/Bedded_Sandstone_DIF.png");
	//plandscape_nm_texture = Textures::TextureCreateFromFilePNG((char*)"data/Bedded_Sandstone_NML.png");
	//GL_REPEAT, int Filter = GL_LINEAR

	plandscape_texture = Textures::TextureCreateFromFilePNG((char*)"data/maps/map_color_1024.png");// "data/Bedded_Sandstone_DIF.png");// , GL_REPEAT, GL_REPEAT, GL_NEAREST);
	plandscape_nm_texture = Textures::TextureCreateFromFilePNG((char*)"data/Bedded_Sandstone_NML.png");

	return true;
}

void cMap::InitZones(uint map_rows, uint map_cells, uint field_rows, uint field_cells)
{
	FullMapWidth = map_cells * field_cells;
	FullMapHeight = map_rows * field_rows;

	printf("cMap::InitZones: FullMapWidth:%d FullMapHeight:%d\n", FullMapWidth, FullMapHeight);

	Navigation.MapRows = map_rows;
	Navigation.MapCells = map_cells;
	Navigation.ZoneRows = field_rows;
	Navigation.ZoneCells = field_cells;

	Navigation.map_nodes_count = Navigation.MapRows * Navigation.MapCells;
	Navigation.map_nodes = new _map_asnode[Navigation.map_nodes_count];

	Navigation.NavZones = new _map_asnode * *[Navigation.MapRows];
	ViewZones = new _view_zone_item * *[Navigation.MapRows];

	for(int ri = 0; ri < Navigation.MapRows; ++ri)
	{
		Navigation.NavZones[ri] = new _map_asnode * [Navigation.MapCells];
		ViewZones[ri] = new _view_zone_item * [Navigation.MapCells];

		for(int ci = 0; ci < Navigation.MapCells; ++ci)
		{
			Navigation.NavZones[ri][ci] = new _map_asnode();

			Navigation.NavZones[ri][ci]->r = ri;
			Navigation.NavZones[ri][ci]->c = ci;

			ViewZones[ri][ci] = new _view_zone_item(
				_view_zone_item::_VIEW_ZONE_STAT::NON,
				vec2(ci * (Navigation.ZoneCells - 1), ri * (Navigation.ZoneRows - 1)),
				vec2(ci * (Navigation.ZoneCells - 1), (ri * (Navigation.ZoneRows - 1)) + Navigation.ZoneRows - 1),
				vec2((ci * (Navigation.ZoneCells - 1)) + Navigation.ZoneCells - 1, (ri * (Navigation.ZoneRows - 1)) + Navigation.ZoneRows - 1),
				vec2((ci * (Navigation.ZoneCells - 1)) + Navigation.ZoneCells - 1, ri * (Navigation.ZoneRows - 1))
			);

			ViewZones[ri][ci]->row_id = ri;
			ViewZones[ri][ci]->cell_id = ci;
		}
	}

	plandscape_texture = Textures::TextureCreateFromFilePNG((char*)"data/maps/map_color_256.png");// "data/Bedded_Sandstone_DIF.png");// , GL_REPEAT, GL_REPEAT, GL_NEAREST);
	plandscape_nm_texture = Textures::TextureCreateFromFilePNG((char*)"data/Bedded_Sandstone_NML.png");

}

cMap::_zone_file* cMap::ReadZone(
	char* landscape_height_map_file,
	int zone_rows,
	int zone_cells,
	int map_rows,
	int map_cells,
	int map_row,
	int map_cell
)
{
	FILE* pf = fopen(landscape_height_map_file, "rb");

	if(!pf){
		printf("ERROR file not found %s\n", landscape_height_map_file);
		return 0;
	}

	_map_asnode* ret_zone = new _map_asnode();

	ret_zone->r = map_row;
	ret_zone->c = map_cell;

	int start_x = map_cell * zone_cells;
	int start_y = map_row * zone_rows;
	//printf(">> start_x:%d start_y:%d\n", start_x, start_y);

	unsigned int map_width = 0;
	unsigned int map_height = 0;

	fread(&map_width, 1, 4, pf);
	fread(&map_height, 1, 4, pf);

	fseek(pf, ((start_y * map_height) + start_x) * 2, SEEK_CUR);

	int CBlocsCount = zone_cells + 1;
	int RBlocsCount = zone_rows + 1;

	uint polygons_count = ((CBlocsCount - 1) * (RBlocsCount - 1)) * 2;
	uint vertices_count = CBlocsCount * RBlocsCount;

	//printf("polygons_count:%d vertices_count:%d\n", polygons_count, vertices_count);

	//----------------------------------------------------------------------

	btTriangleMesh* pTriangleMesh = new btTriangleMesh();

	_plane_vert* vert_buffer = new _plane_vert[vertices_count];

	float start_tv = 1.0f - (float(map_cell * zone_cells) / float(map_width));
	float start_tu = float(map_row * zone_rows) / float(map_height);

	float end_tv = 1.0f - (float((map_cell < map_cells ? map_cell +1: map_cells) * zone_cells) / float(map_width));
	float end_tu = float((map_row < map_rows ? map_row + 1 : map_rows) * zone_rows) / float(map_height);

	float step_tv = (end_tv - start_tv) / zone_cells;
	float step_tu = (end_tu - start_tu) / zone_rows;

	float tv = start_tv;
	float tu = start_tu;

	/*float start_tv = 0.0f;
	float start_tu = 0.0f;

	float step_tv = 1.0f / field_cells;
	float step_tu = 1.0f / field_rows;

	float tv = start_tv;
	float tu = start_tu;*/

	//printf(">> start_tv:%f start_tu:%f end_tv:%f end_tu:%f step_tv:%f step_tu:%f\n", start_tv, start_tu, end_tu, end_tu, step_tv, step_tu);

	int vi = 0;

	word* height_row = new word[CBlocsCount];

	for(int dy = 0; dy < RBlocsCount; ++dy)
	{
		fread(height_row, 1, CBlocsCount * 2, pf);

		for(int dx = 0; dx < CBlocsCount; ++dx)
		{
			float point_height = height_row[dx];

			vert_buffer[vi].pos = vec3(dx, (point_height / 5000.0f), dy); // /200
			//vert_buffer[vi].pos = vec3(dx, 0.0f, dy);

			vert_buffer[vi].tex = vec2(tv, tu);

			vert_buffer[vi].normal = vec3(0.0f, 1.0f, 0.0f);

			/*if(vert_buffer[vi].pos.x + start_x < ret_zone->min_point.x)ret_zone->min_point.x = vert_buffer[vi].pos.x + start_x;
			if(vert_buffer[vi].pos.x + start_x > ret_zone->max_point.x)ret_zone->max_point.x = vert_buffer[vi].pos.x + start_x;
			if(vert_buffer[vi].pos.z + start_y < ret_zone->min_point.y)ret_zone->min_point.y = vert_buffer[vi].pos.z + start_y;
			if(vert_buffer[vi].pos.z + start_y > ret_zone->max_point.y)ret_zone->max_point.y = vert_buffer[vi].pos.z + start_y;*/
			if(vert_buffer[vi].pos.y < ret_zone->min_height)ret_zone->min_height = vert_buffer[vi].pos.y;
			if(vert_buffer[vi].pos.y > ret_zone->max_height)ret_zone->max_height = vert_buffer[vi].pos.y;

			tv += step_tv;

			++vi;
		}

		tv = start_tv;
		tu += step_tu;

		fseek(pf, (map_width - CBlocsCount) * 2, SEEK_CUR);
	}

	//printf("vertices calculated:%d\n", vi-1);

	//printf("[%f %f]\n", vert_buffer[0].tex.x, vert_buffer[0].tex.y);
	//printf("[%f %f]\n", vert_buffer[vertices_count - 1].tex.x, vert_buffer[vertices_count - 1].tex.y);

	fclose(pf);

	delete[] height_row;

	//------------------------------
	cList<_zone_asnode*> cells_list;
	//------------------------------

	uint indexes_count = polygons_count * 3;
	unsigned short int* indexes = new unsigned short int[indexes_count];

	int ia1 = CBlocsCount + 1;
	int ia2 = 0;
	int ia3 = CBlocsCount;
	int ia4 = 0;
	int ia5 = CBlocsCount + 1;
	int ia6 = 1;

	int block_x = 0;
	int block_y = 0;
	int cell_index = 0;

	for(int i = 0; i < indexes_count; i += 6)
	{
		//printf("\"%d (%d %d %d) (%d %d %d)\"\n", i / 6, ia1, ia2, ia3, ia4, ia5, ia6);

		indexes[i] = ia1;
		indexes[i + 1] = ia2;
		indexes[i + 2] = ia3;

		indexes[i + 3] = ia4;
		indexes[i + 4] = ia5;
		indexes[i + 5] = ia6;

		btVector3 vertex_1(vert_buffer[ia1].pos.x, vert_buffer[ia1].pos.y, vert_buffer[ia1].pos.z);
		btVector3 vertex_2(vert_buffer[ia2].pos.x, vert_buffer[ia2].pos.y, vert_buffer[ia2].pos.z);
		btVector3 vertex_3(vert_buffer[ia3].pos.x, vert_buffer[ia3].pos.y, vert_buffer[ia3].pos.z);
		btVector3 vertex_4(vert_buffer[ia4].pos.x, vert_buffer[ia4].pos.y, vert_buffer[ia4].pos.z);
		btVector3 vertex_5(vert_buffer[ia5].pos.x, vert_buffer[ia5].pos.y, vert_buffer[ia5].pos.z);
		btVector3 vertex_6(vert_buffer[ia6].pos.x, vert_buffer[ia6].pos.y, vert_buffer[ia6].pos.z);

		pTriangleMesh->addTriangle(vertex_1, vertex_2, vertex_3);
		pTriangleMesh->addTriangle(vertex_4, vertex_5, vertex_6);

		//-------------------------------------

		_plane_vert* vA1 = &vert_buffer[ia1];
		_plane_vert* vB1 = &vert_buffer[ia2];
		_plane_vert* vC1 = &vert_buffer[ia3];

		_plane_vert* vA2 = &vert_buffer[ia4];
		_plane_vert* vB2 = &vert_buffer[ia5];
		_plane_vert* vC2 = &vert_buffer[ia6];

		vec3 pA1 = vec3(vA1->pos.x, vA1->pos.y, vA1->pos.z);
		vec3 pB1 = vec3(vB1->pos.x, vB1->pos.y, vB1->pos.z);
		vec3 pC1 = vec3(vC1->pos.x, vC1->pos.y, vC1->pos.z);

		vec3 pA2 = vec3(vA2->pos.x, vA2->pos.y, vA2->pos.z);
		vec3 pB2 = vec3(vB2->pos.x, vB2->pos.y, vB2->pos.z);
		vec3 pC2 = vec3(vC2->pos.x, vC2->pos.y, vC2->pos.z);

		vec3 p1 = pB1 - pA1;
		vec3 q1 = pC1 - pA1;

		vec3 normal1 = normalize(cross(p1, q1));

		vec3 p2 = pB2 - pA2;
		vec3 q2 = pC2 - pA2;

		vec3 normal2 = normalize(cross(p2, q2));

		vA1->normal = normal1;
		vB1->normal = normal1;
		vC1->normal = normal1;
		vA2->normal = normal2;
		vB2->normal = normal2;
		vC2->normal = normal2;

		float plane_angle = acos(dot(normal1, vec3(0.0f, 1.0f, 0.0f)));

		if(plane_angle < 0.52f)
		{
			_zone_asnode* node = new _zone_asnode;
			node->id = cell_index;
			node->r = block_y;
			node->c = block_x;

			vec3 nod_pos = lerp(vA1->pos, vA2->pos, 0.5f);

			//printf("%f %f %f\n", nod_pos.x, nod_pos.y, nod_pos.z);

			node->local_pos = nod_pos;
			node->plane_angle = plane_angle;

			cells_list.push(node);

			++cell_index;
		}
		//printf(">>> field: %dx%d %d:(%dx%d)\n", block_x, block_y, Navigation.NavZones[0].field[block_y][block_x]->id, Navigation.NavZones[0].field[block_y][block_x]->c, Navigation.NavZones[0].field[block_y][block_x]->r);

		//------------------------------------

		ia1 += 1;
		ia2 += 1;
		ia3 += 1;

		ia4 += 1;
		ia5 += 1;
		ia6 += 1;

		//printf("block_x:%d %% (CBlocsCount - 2):%d = %d\n", block_x, (CBlocsCount - 2), block_x % (CBlocsCount - 2));

		if(block_x && block_x % (CBlocsCount - 2) == 0){

			ia1 += 1;
			ia2 += 1;
			ia3 += 1;
			ia4 += 1;
			ia5 += 1;
			ia6 += 1;

			block_x = -1;

			++block_y;
		}
		block_x++;
	}

	ret_zone->AllockField(cells_list.count, RBlocsCount, CBlocsCount);

	int fid = 0;

	cells_list.reset_enumerator();
	while(_zone_asnode* node = cells_list.next())
	{
		ret_zone->nodes[fid].id = fid;
		ret_zone->nodes[fid].r = node->r;
		ret_zone->nodes[fid].c = node->c;
		ret_zone->nodes[fid].local_pos = node->local_pos;
		ret_zone->nodes[fid].plane_angle = node->plane_angle;

		ret_zone->field[node->r][node->c] = &ret_zone->nodes[fid++];
	}

	ret_zone->BuildLinks(cells_list);
	//printf("%d %d\n", Navigation.NavZones[0].nodes_count, cells_list.count);

	cells_list.enumerate([](_zone_asnode* node){delete node; return false; });
	cells_list.free();

	//Navigation.NavZonesList.push(&Navigation.NavZones[0]);

	/*for(int i = 0; i < indexes_count; ++i)
	{
		printf("%d ", indexes[i]);
	}*/

	cLandscapeObject* ret_landscape = new cLandscapeObject();

	ret_landscape->InitGeometrix(vert_buffer, vertices_count, indexes, indexes_count, pTriangleMesh);

	//printf("landscape ok\n");

	ret_landscape->SetPosition(float(start_x), 0.0f, float(start_y));

	_zone_file* zone_file = new _zone_file();

	zone_file->zone = ret_zone;
	zone_file->landscape = ret_landscape;

	return zone_file;
}

bool cMap::LoadZone(int map_row, int map_cell)
{
	_zone_file* zone_file = ReadZone(LandscapeHeightMapFile, Navigation.ZoneRows, Navigation.ZoneCells, Navigation.MapRows, Navigation.MapCells, map_row, map_cell);
	if(zone_file)
	{
		zone_file->landscape->models_list.reset_enumerator();
		while(cModel* model = zone_file->landscape->models_list.next())
		{
			model->mesh_list.reset_enumerator();
			while(cMesh* mesh = model->mesh_list.next())
			{
				cMesh::init_vertexes(mesh, mesh->vert_buff, mesh->vnum);
				cMesh::init_indexes(mesh, mesh->index_buff, mesh->inum);
			}
		}

		int map_node_id = (map_row * Navigation.MapRows) + map_cell;

		_map_asnode* map_node = &Navigation.map_nodes[map_node_id];

		map_node->id = map_node_id;
		map_node->r = map_row;
		map_node->c = map_cell;
		map_node->min_height = zone_file->zone->min_height;
		map_node->max_height = zone_file->zone->max_height;
		map_node->field = zone_file->zone->field;
		map_node->nodes = zone_file->zone->nodes;
		map_node->nodes_count = zone_file->zone->nodes_count;

		Navigation.NavZones[map_row][map_cell] = map_node;
		ViewZones[map_row][map_cell]->LandscapeObject = zone_file->landscape;
		ViewZones[map_row][map_cell]->ZoneStat = _view_zone_item::_VIEW_ZONE_STAT::LOADED;

		zone_file->zone->nodes = 0;
		zone_file->zone->field = 0;
		delete zone_file->zone;

		delete zone_file;

		return true;
	}

	return false;
}

void cMap::WriteMapOld(const char* fname)
{
	printf("cMap::WriteMap()\n");
	FILE* pf = fopen(fname, "wb");

	fwrite(&Navigation.MapRows, 1, 4, pf);
	fwrite(&Navigation.MapCells, 1, 4, pf);
	fwrite(&Navigation.ZoneRows, 1, 4, pf);
	fwrite(&Navigation.ZoneCells, 1, 4, pf);

	for(int mr = 0; mr < Navigation.MapRows; ++mr)
	{
		for(int mc = 0; mc < Navigation.MapCells; ++mc)
		{
			_map_asnode* zone = Navigation.NavZones[mr][mc];

			word nodes_count = 0;
			if(!zone){
				fwrite(&nodes_count, 1, 2, pf);
				continue;
			}

			nodes_count = zone->nodes_count;
			printf("\tmrc:[%d %d]\n", mr, mc);

			fwrite(&nodes_count, 1, 2, pf);

			for(int ni = 0; ni < zone->nodes_count; ++ni)
			{
				_zone_asnode* node = &zone->nodes[ni];

				fwrite(&node->r, 1, 4, pf);
				fwrite(&node->c, 1, 4, pf);

				fwrite(&node->links_count, 1, 4, pf);

				for(int li = 0; li < node->links_count; ++li)
				{
					fwrite(&node->link_nodes[li]->r, 1, 4, pf);
					fwrite(&node->link_nodes[li]->c, 1, 4, pf);
				}
			}

			fwrite(&zone->links_count, 1, 4, pf);
			printf("\tzone->links_count:%d\n", zone->links_count);
			for(int li = 0; li < zone->links_count; ++li)
			{
				fwrite(&zone->link_nodes[li]->r, 1, 4, pf);
				fwrite(&zone->link_nodes[li]->c, 1, 4, pf);
				printf("\tzone->links_count:%d\n", zone->links_count);
			}
		}
	}

	fclose(pf);
}

void cMap::ReadMapOld(const char* fname)
{
	printf("cMap::ReadMap\n");

	FILE* pf = fopen(fname, "rb");

	fread(&Navigation.MapRows, 1, 4, pf);
	fread(&Navigation.MapCells, 1, 4, pf);
	fread(&Navigation.ZoneRows, 1, 4, pf);
	fread(&Navigation.ZoneCells, 1, 4, pf);

	printf("\tMapRows:%d\n", Navigation.MapRows);
	printf("\tMapCells:%d\n", Navigation.MapCells);
	printf("\tZoneRows:%d\n", Navigation.ZoneRows);
	printf("\tZoneCells:%d\n", Navigation.ZoneCells);

	for(int mr = 0; mr < Navigation.MapRows; ++mr)
	{
		for(int mc = 0; mc < Navigation.MapCells; ++mc)
		{
			word nodes_count = 0;
			fread(&nodes_count, 1, 2, pf);

			printf("\trc:[%d %d] nodes_count%d\n", mr, mc, nodes_count);

			if(!nodes_count){				
				continue;
			}

			_map_asnode* zone = Navigation.NavZones[mr][mc];

			zone->nodes_count = nodes_count;

			for(int ni = 0; ni < zone->nodes_count; ++ni)
			{
				//printf("\t\tnode:%d\n", ni);
				_zone_asnode* node = &zone->nodes[ni];

				fread(&node->r, 1, 4, pf);
				fread(&node->c, 1, 4, pf);

				//printf("\t\tnode->rc:[%d %d]\n", node->r, node->c);

				fread(&node->links_count, 1, 4, pf);
				//printf("\t\tnode->links_count:%d\n", node->links_count);

				for(int li = 0; li < node->links_count; ++li)
				{
					fread(&node->link_nodes[li]->r, 1, 4, pf);
					fread(&node->link_nodes[li]->c, 1, 4, pf);
				}
			}

			fread(&zone->links_count, 1, 4, pf);
			printf("\tzone->links_count:%d\n", zone->links_count);
			zone->link_nodes = new _asnode_base*[zone->links_count];
			for(int li = 0; li < zone->links_count; ++li)
			{
				int lr = -1;
				int lc = -1;
				fread(&lr, 1, 4, pf);
				fread(&lc, 1, 4, pf);
				printf("\tzone->link to mrc:%d %d\n", lr, lc);

				zone->link_nodes[li] = (_asnode_base*)Navigation.NavZones[lr][lc];
			}
		}
	}

	fclose(pf);
}

void cMap::WriteMap(const char* fname)
{
	printf("cMap::WriteMap()\n");
	FILE* pf = fopen(fname, "wb");

	word map_rows = Navigation.MapRows;
	word map_cells = Navigation.MapCells;
	word zone_rows = Navigation.ZoneRows;
	word zone_cells = Navigation.ZoneCells;

	fwrite(&map_rows, 1, 2, pf);
	fwrite(&map_cells, 1, 2, pf);
	fwrite(&zone_rows, 1, 2, pf);
	fwrite(&zone_cells, 1, 2, pf);

	for(int mr = 0; mr < Navigation.MapRows; ++mr)
	{
		for(int mc = 0; mc < Navigation.MapCells; ++mc)
		{
			_map_asnode* map_node = Navigation.NavZones[mr][mc];

			word nodes_count = 0;
			if(map_node){
				nodes_count = map_node->nodes_count;
			}

			fwrite(&nodes_count, 1, 2, pf);

			if(!map_node)continue;

			//--------------------
			cMesh* mesh = ViewZones[mr][mc]->LandscapeObject->models_list[0]->mesh_list[0];
			_plane_vert* vertices = (_plane_vert*)mesh->vert_buff;
			word* indexes = mesh->index_buff;
			//word indexes_count = mesh->inum;

			word* height_map = new word[(zone_rows + 1) * (zone_cells + 1)];

			for(int bi = 0; bi < (zone_rows+1) * (zone_cells + 1); ++bi)
			{
				height_map[bi] = vertices[bi].pos.y;
			}

			fwrite(height_map, 1, sizeof(word) * ((zone_rows + 1) * (zone_cells + 1)), pf);

			//fwrite(indexes, 1, sizeof(word) * indexes_count, pf);
			//--------------------

			for(int ni = 0; ni < map_node->nodes_count; ++ni)
			{
				_zone_asnode* field_node = &map_node->nodes[ni];

				word fnr = field_node->r;
				word fnc = field_node->c;
				fwrite(&fnr, 1, 2, pf);
				fwrite(&fnc, 1, 2, pf);

				uchar field_node_links_count = field_node->links_count;
				fwrite(&field_node_links_count, 1, 1, pf);

				for(int li = 0; li < field_node->links_count; ++li)
				{
					word fnlid = field_node->link_nodes[li]->id;

					fwrite(&fnlid, 1, 2, pf);
					//printf("\tfnlid:%d\n", fnlid);
				}
			}

			word map_node_links_count = map_node->links_count;
			fwrite(&map_node_links_count, 1, 2, pf);

			//printf("\tmap_node->links_count:%d\n", map_node->links_count);
			for(int li = 0; li < map_node->links_count; ++li)
			{
				word mnlr = map_node->link_nodes[li]->r;
				word mnlc = map_node->link_nodes[li]->c;
				fwrite(&mnlr, 1, 2, pf);
				fwrite(&mnlc, 1, 2, pf);
			}
		}
	}

	fclose(pf);
}

void cMap::ReadMap(const char* fname)
{
	printf("cMap::ReadMap\n");

	FILE* pf = fopen(fname, "rb");

	word map_rows = -1;
	word map_cells = -1;
	word zone_rows = -1;
	word zone_cells = -1;

	fread(&map_rows, 1, 2, pf);
	fread(&map_cells, 1, 2, pf);
	fread(&zone_rows, 1, 2, pf);
	fread(&zone_cells, 1, 2, pf);

	InitZones(map_rows, map_cells, zone_rows, zone_cells);

	printf("\tMapRows:%d\n", Navigation.MapRows);
	printf("\tMapCells:%d\n", Navigation.MapCells);
	printf("\tZoneRows:%d\n", Navigation.ZoneRows);
	printf("\tZoneCells:%d\n", Navigation.ZoneCells);

	for(int mr = 0; mr < Navigation.MapRows; ++mr)
	{
		for(int mc = 0; mc < Navigation.MapCells; ++mc)
		{
			word nodes_count = 0;
			fread(&nodes_count, 1, 2, pf);

			printf("\trc:[%d %d] nodes_count:%d\n", mr, mc, nodes_count);

			if(!nodes_count){
				continue;
			}

			//--------------------------------------------------------------
			uint vertices_count = (zone_rows + 1) * (zone_cells + 1);
			uint polygons_count = (zone_rows * zone_cells) * 2;
			uint indexes_count = polygons_count * 3;

			_plane_vert* vertices = new _plane_vert[vertices_count];
			word* indexes = new word[indexes_count];
			word* height_map = new word[vertices_count];

			fread(height_map, 1, sizeof(word) * vertices_count, pf);
			//fread(indexes, 1, sizeof(word) * indexes_count, pf);

			float start_tv = 1.0f - (float(mc * zone_cells) / float(FullMapWidth));
			float start_tu = float(mr * zone_rows) / float(FullMapHeight);

			float end_tv = 1.0f - (float((mc < map_cells ? mc + 1 : map_cells) * zone_cells) / float(FullMapWidth));
			float end_tu = float((mr < map_rows ? mr + 1 : map_rows) * zone_rows) / float(FullMapHeight);

			float step_tv = (end_tv - start_tv) / zone_cells;
			float step_tu = (end_tu - start_tu) / zone_rows;

			float tv = start_tv;
			float tu = start_tu;

			for(int bri = 0; bri < zone_rows + 1; ++bri)
			{
				for(int bci = 0; bci < zone_cells + 1; ++bci)
				{
					int ofs = (bri * (zone_rows + 1)) + bci;
					vertices[ofs].pos = vec3(bci, height_map[ofs] / 5000, bri);
					vertices[ofs].tex = vec2(tv, tu);

					//if((mr == 1 && mc == 1))
					{
						//printf("%f %f %f\n", vertices[ofs].pos.x, vertices[ofs].pos.y, vertices[ofs].pos.z);
						//vertices[ofs].pos.y = 0.0f;
					}

					tv += step_tv;
				}

				tv = start_tv;
				tu += step_tu;
			}

			btTriangleMesh* pTriangleMesh = new btTriangleMesh();

			int ia1 = zone_cells + 2;
			int ia2 = 0;
			int ia3 = zone_cells + 1;
			int ia4 = 0;
			int ia5 = zone_cells + 2;
			int ia6 = 1;

			int block_x = 0;
			int block_y = 0;

			for(int i = 0; i < indexes_count; i += 6)
			{
				indexes[i] = ia1;
				indexes[i + 1] = ia2;
				indexes[i + 2] = ia3;

				indexes[i + 3] = ia4;
				indexes[i + 4] = ia5;
				indexes[i + 5] = ia6;

				btVector3 vertex_1(vertices[ia1].pos.x, vertices[ia1].pos.y, vertices[ia1].pos.z);
				btVector3 vertex_2(vertices[ia2].pos.x, vertices[ia2].pos.y, vertices[ia2].pos.z);
				btVector3 vertex_3(vertices[ia3].pos.x, vertices[ia3].pos.y, vertices[ia3].pos.z);
				btVector3 vertex_4(vertices[ia4].pos.x, vertices[ia4].pos.y, vertices[ia4].pos.z);
				btVector3 vertex_5(vertices[ia5].pos.x, vertices[ia5].pos.y, vertices[ia5].pos.z);
				btVector3 vertex_6(vertices[ia6].pos.x, vertices[ia6].pos.y, vertices[ia6].pos.z);

				pTriangleMesh->addTriangle(vertex_1, vertex_2, vertex_3);
				pTriangleMesh->addTriangle(vertex_4, vertex_5, vertex_6);

				//-------------------------------------

				_plane_vert* vA1 = &vertices[ia1];
				_plane_vert* vB1 = &vertices[ia2];
				_plane_vert* vC1 = &vertices[ia3];

				_plane_vert* vA2 = &vertices[ia4];
				_plane_vert* vB2 = &vertices[ia5];
				_plane_vert* vC2 = &vertices[ia6];

				vec3 pA1 = vec3(vA1->pos.x, vA1->pos.y, vA1->pos.z);
				vec3 pB1 = vec3(vB1->pos.x, vB1->pos.y, vB1->pos.z);
				vec3 pC1 = vec3(vC1->pos.x, vC1->pos.y, vC1->pos.z);

				vec3 pA2 = vec3(vA2->pos.x, vA2->pos.y, vA2->pos.z);
				vec3 pB2 = vec3(vB2->pos.x, vB2->pos.y, vB2->pos.z);
				vec3 pC2 = vec3(vC2->pos.x, vC2->pos.y, vC2->pos.z);

				vec3 p1 = pB1 - pA1;
				vec3 q1 = pC1 - pA1;

				vec3 normal1 = normalize(cross(p1, q1));

				vec3 p2 = pB2 - pA2;
				vec3 q2 = pC2 - pA2;

				vec3 normal2 = normalize(cross(p2, q2));

				vA1->normal = normal1;
				vB1->normal = normal1;
				vC1->normal = normal1;
				vA2->normal = normal2;
				vB2->normal = normal2;
				vC2->normal = normal2;

				ia1 += 1;
				ia2 += 1;
				ia3 += 1;

				ia4 += 1;
				ia5 += 1;
				ia6 += 1;

				if(block_x && block_x % (zone_cells - 1) == 0){

					ia1 += 1;
					ia2 += 1;
					ia3 += 1;
					ia4 += 1;
					ia5 += 1;
					ia6 += 1;

					block_x = -1;

					++block_y;
				}
				block_x++;
			}

			cLandscapeObject* landscape_object = new cLandscapeObject();

			landscape_object->InitGeometrix(vertices, vertices_count, indexes, indexes_count, pTriangleMesh, true);

			landscape_object->SetPosition(float(mc), 0.0f, float(mr));

			ViewZones[mr][mc]->LandscapeObject = landscape_object;
			ViewZones[mr][mc]->ZoneStat = _view_zone_item::_VIEW_ZONE_STAT::LOADED;

			ViewZones[mr][mc]->LandscapeObject->is_rendered = false;
			ViewZones[mr][mc]->LandscapeObject->already_collided = false;
			//printf("landscape ok\n");

			//---------------------------------------------------------------

			_map_asnode* map_node = Navigation.NavZones[mr][mc];

			map_node->AllockField(nodes_count, zone_rows, zone_cells);

			for(int ni = 0; ni < map_node->nodes_count; ++ni)
			{
				_zone_asnode* field_node = &map_node->nodes[ni];

				word fnr = -1;
				word fnc = -1;
				fread(&fnr, 1, 2, pf);
				fread(&fnc, 1, 2, pf);

				field_node->r = fnr;
				field_node->c = fnc;
				field_node->id = ni;

				uchar field_node_links_count = -1;
				fread(&field_node_links_count, 1, 1, pf);

				field_node->links_count = field_node_links_count;
				field_node->link_nodes = new _asnode_base*[field_node_links_count];

				for(int li = 0; li < field_node->links_count; ++li)
				{
					word fnlid = -1;
					fread(&fnlid, 1, 2, pf);

					field_node->link_nodes[li] = &map_node->nodes[fnlid];
				}
			}

			word map_node_links_count = -1;
			fread(&map_node_links_count, 1, 2, pf);

			map_node->links_count = map_node_links_count;
			map_node->link_nodes = new _asnode_base * [map_node_links_count];

			if(mr == 2 && mc == 2)
			{
				printf("map_node_links_count:%d\n", map_node_links_count);
			}

			for(int li = 0; li < map_node->links_count; ++li)
			{
				word mnlr = -1;
				word mnlc = -1;
				fread(&mnlr, 1, 2, pf);
				fread(&mnlc, 1, 2, pf);

				if(mr == 2 && mc == 2)
				{
					printf("    link to:%dx%d %p\n", mnlr, mnlc, Navigation.NavZones[mnlr][mnlc]);
				}

				map_node->link_nodes[li] = Navigation.NavZones[mnlr][mnlc];
			}
		}
	}

	fclose(pf);
}

void cMap::Update()
{
	Navigation.Update();
}

void cMap::DBGCreateBox()
{
	btVector3 scale = btVector3(1.0f, 1.0f, 1.0f);
	btVector3 offs = btVector3(0.0f, 0.0f, 0.0f);

	cList<_line*>* plines = new cList<_line*>();

	plines->push(new _line(-50.0f, -10.0f, 50.0f, -10.0f, 1.0f, 0.0f));
	//plines->push(new _line(10.0f, -10.0f, 10.0f, 10.0f));
	//plines->push(new _line(10.0f, 10.0f, -10.0f, 10.0f));
	//plines->push(new _line(-10.0f, 10.0f, -10.0f, -10.0f));

	//PlayerStartPosition *= vec2(scale.x(), scale.z());
	//PlayerStartPosition += vec2(offs.x(), offs.z());

	//create_walls(plines);
	//create_floor();
}