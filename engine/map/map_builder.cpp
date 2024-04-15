#include "map_builder.h"

#include <windows.h>

#include <glm/gtx/compatibility.hpp>//vec3 lerp

bool cMapBuilder::ReadHeightMap(const char* landscape_height_map_file, uint field_rows, uint field_cells)
{
	LandscapeHeightMapFile = landscape_height_map_file;

	Navigation.ZoneRows = field_rows;
	Navigation.ZoneCells = field_cells;

	FILE* pf = fopen(landscape_height_map_file, "rb");

	if(!pf){
		printf("ERROR file not found %s\n", landscape_height_map_file);
		return false;
	}

	fread(&FullMapWidth, 1, 4, pf);
	fread(&FullMapHeight, 1, 4, pf);

	printf("cMap::InitZones: FullMapWidth:%d FullMapHeight:%d\n", FullMapWidth, FullMapHeight);

	Navigation.MapRows = FullMapHeight / Navigation.ZoneRows;
	Navigation.MapCells = FullMapWidth / Navigation.ZoneCells;

	Navigation.map_nodes_count = Navigation.MapRows * Navigation.MapCells;
	Navigation.map_nodes = new _map_asnode[Navigation.map_nodes_count];

	Navigation.NavZones = new _map_asnode **[Navigation.MapRows];

	HeightsTable = new word**[Navigation.MapRows];

	for(int ri = 0; ri < Navigation.MapRows; ++ri)
	{
		HeightsTable[ri] = new word *[Navigation.MapCells];

		Navigation.NavZones[ri] = new _map_asnode *[Navigation.MapCells];

		for(int ci = 0; ci < Navigation.MapCells; ++ci)
		{
			Navigation.NavZones[ri][ci] = 0;
			HeightsTable[ri][ci] = new word[(field_rows + 1) * (field_cells + 1)];
		}
	}

	fclose(pf);

	return true;
}

_map_asnode* cMapBuilder::ReadZone(
	int zone_rows,
	int zone_cells,
	int map_rows,
	int map_cells,
	int map_row,
	int map_cell
)
{
	FILE* pf = fopen(LandscapeHeightMapFile, "rb");

	if(!pf){
		printf("ERROR file not found %s\n", LandscapeHeightMapFile);
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

	vec3* vert_buffer = new vec3[vertices_count];

	int vi = 0;

	word* height_row = new word[CBlocsCount];

	for(int dy = 0; dy < RBlocsCount; ++dy)
	{
		fread(height_row, 1, CBlocsCount * 2, pf);

		for(int dx = 0; dx < CBlocsCount; ++dx)
		{
			float point_height = height_row[dx];

			HeightsTable[map_row][map_cell][vi] = height_row[dx];

			vert_buffer[vi] = vec3(dx, (point_height / 5000.0f), dy);

			if(vert_buffer[vi].y < ret_zone->min_height)ret_zone->min_height = vert_buffer[vi].y;
			if(vert_buffer[vi].y > ret_zone->max_height)ret_zone->max_height = vert_buffer[vi].y;

			++vi;
		}

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

		//-------------------------------------

		vec3* vA1 = &vert_buffer[ia1];
		vec3* vB1 = &vert_buffer[ia2];
		vec3* vC1 = &vert_buffer[ia3];

		vec3* vA2 = &vert_buffer[ia4];
		vec3* vB2 = &vert_buffer[ia5];
		vec3* vC2 = &vert_buffer[ia6];

		vec3 pA1 = vec3(vA1->x, vA1->y, vA1->z);
		vec3 pB1 = vec3(vB1->x, vB1->y, vB1->z);
		vec3 pC1 = vec3(vC1->x, vC1->y, vC1->z);

		vec3 pA2 = vec3(vA2->x, vA2->y, vA2->z);
		vec3 pB2 = vec3(vB2->x, vB2->y, vB2->z);
		vec3 pC2 = vec3(vC2->x, vC2->y, vC2->z);

		vec3 p1 = pB1 - pA1;
		vec3 q1 = pC1 - pA1;

		vec3 normal1 = normalize(cross(p1, q1));

		vec3 p2 = pB2 - pA2;
		vec3 q2 = pC2 - pA2;

		vec3 normal2 = normalize(cross(p2, q2));

		float plane_angle = acos(dot(normal1, vec3(0.0f, 1.0f, 0.0f)));

		if(plane_angle < 0.52f)
		{
			_zone_asnode* node = new _zone_asnode;
			node->id = cell_index;
			node->r = block_y;
			node->c = block_x;

			vec3 nod_pos = lerp(*vA1, *vA2, 0.5f);

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

	delete[] vert_buffer;
	delete[] indexes;

	return ret_zone;
}

bool cMapBuilder::LoadZone(int map_row, int map_cell)
{
	_map_asnode* asmap = ReadZone(Navigation.ZoneRows, Navigation.ZoneCells, Navigation.MapRows, Navigation.MapCells, map_row, map_cell);
	
	if(asmap)
	{
		int map_node_id = (map_row * Navigation.MapRows) + map_cell;

		_map_asnode* map_node = &Navigation.map_nodes[map_node_id];

		map_node->id = map_node_id;
		map_node->r = map_row;
		map_node->c = map_cell;
		map_node->min_height = asmap->min_height;
		map_node->max_height = asmap->max_height;
		map_node->field = asmap->field;
		map_node->nodes = asmap->nodes;
		map_node->nodes_count = asmap->nodes_count;

		Navigation.NavZones[map_row][map_cell] = map_node;

		return true;
	}

	return false;
}

class _CopyNodes{
public:
	int rows = 0;
	int cells = 0;
	_asnode_base*** source_fild_centre = 0;
	_asnode_base*** source_fild_left = 0;
	_asnode_base*** source_fild_right = 0;
	_asnode_base*** source_fild_top = 0;
	_asnode_base*** source_fild_bottom = 0;
	_asnode_base*** field_centre = 0;
	_asnode_base*** field_left = 0;
	_asnode_base*** field_right = 0;
	_asnode_base*** field_top = 0;
	_asnode_base*** field_bottom = 0;
	cAStar* pAStar;

	_CopyNodes(int rn, int cn)
	{
		rows = rn;
		cells = cn;

		field_centre = new _asnode_base * *[rows];
		field_left = new _asnode_base * *[rows];
		field_right = new _asnode_base * *[rows];
		field_top = new _asnode_base * *[rows];
		field_bottom = new _asnode_base * *[rows];

		for(int ri = 0; ri < rows; ++ri)
		{
			field_centre[ri] = new _asnode_base * [cells];
			field_left[ri] = new _asnode_base * [cells];
			field_right[ri] = new _asnode_base * [cells];
			field_top[ri] = new _asnode_base * [cells];
			field_bottom[ri] = new _asnode_base * [cells];

			for(int ci = 0; ci < cells; ++ci)
			{
				field_centre[ri][ci] = 0;
				field_left[ri][ci] = 0;
				field_right[ri][ci] = 0;
				field_top[ri][ci] = 0;
				field_bottom[ri][ci] = 0;
			}
		}
	}

	~_CopyNodes()
	{
		for(int ri = 0; ri < rows; ++ri)
		{
			delete[] field_centre[ri];
			delete[] field_left[ri];
			delete[] field_right[ri];
			delete[] field_top[ri];
			delete[] field_bottom[ri];
		}

		delete[] field_centre;
		delete[] field_left;
		delete[] field_right;
		delete[] field_top;
		delete[] field_bottom;
	}

	void init_fields()
	{
		for(int ri = 0; ri < rows; ++ri)
		{
			for(int ci = 0; ci < cells; ++ci)
			{
				if(source_fild_centre[ri][ci]){
					field_centre[ri][ci] = new _asnode_base();
					field_centre[ri][ci]->r = ri;
					field_centre[ri][ci]->c = ci;
				}

				if(source_fild_left[ri][ci]){
					field_left[ri][ci] = new _asnode_base();
					field_left[ri][ci]->r = ri;
					field_left[ri][ci]->c = ci;
				}

				if(source_fild_right[ri][ci]){
					field_right[ri][ci] = new _asnode_base();
					field_right[ri][ci]->r = ri;
					field_right[ri][ci]->c = ci;
				}

				if(source_fild_top[ri][ci]){
					field_top[ri][ci] = new _asnode_base();
					field_top[ri][ci]->r = ri;
					field_top[ri][ci]->c = ci;
				}

				if(source_fild_bottom[ri][ci]){
					field_bottom[ri][ci] = new _asnode_base();
					field_bottom[ri][ci]->r = ri;
					field_bottom[ri][ci]->c = ci;
				}
			}
		}
	}

	void free_fields()
	{
		//printf("free_fields()\n");
		for(int ri = 0; ri < rows; ++ri)
		{
			for(int ci = 0; ci < cells; ++ci)
			{
				if(field_centre[ri][ci]){
					delete field_centre[ri][ci];
					field_centre[ri][ci] = 0;
				}

				if(field_left[ri][ci]){
					delete field_left[ri][ci];
					field_left[ri][ci] = 0;
				}

				if(field_right[ri][ci]){
					delete field_right[ri][ci];
					field_right[ri][ci] = 0;
				}

				if(field_top[ri][ci]){
					delete field_top[ri][ci];
					field_top[ri][ci] = 0;
				}

				if(field_bottom[ri][ci]){
					delete field_bottom[ri][ci];
					field_bottom[ri][ci] = 0;
				}
			}
		}

		//printf("free_fields() ok\n");
	}

	void set_sources(_zone_asnode*** c, _zone_asnode*** l, _zone_asnode*** r, _zone_asnode*** t, _zone_asnode*** b)
	{
		source_fild_centre = (_asnode_base***)c;
		source_fild_left = (_asnode_base***)l;
		source_fild_right = (_asnode_base***)r;
		source_fild_top = (_asnode_base***)t;
		source_fild_bottom = (_asnode_base***)b;
	}

	void copy_links(_asnode_base*** sourc, _asnode_base*** dest, int sr, int sc, int rn, int cn)
	{
		for(int ri = sr; ri < rn; ++ri)
		{
			for(int ci = sc; ci < cn; ++ci)
			{
				if(!sourc[ri][ci])continue;

				int links_count = sourc[ri][ci]->links_count;

				dest[ri][ci]->links_count = links_count;
				dest[ri][ci]->link_nodes = new _asnode_base * [links_count];

				for(int li = 0; li < links_count; ++li)
				{
					int lr = sourc[ri][ci]->link_nodes[li]->r;
					int lc = sourc[ri][ci]->link_nodes[li]->c;
					dest[ri][ci]->link_nodes[li] = dest[lr][lc];
				}
			}
		}
	}

	void copy()
	{
		copy_links(source_fild_centre, field_centre, 1, 1, rows - 1, cells - 1);
		copy_links(source_fild_left, field_left, 0, 1, rows, cells);
		copy_links(source_fild_right, field_right, 0, 0, rows, cells - 1);
		copy_links(source_fild_top, field_top, 0, 0, rows - 1, cells);
		copy_links(source_fild_bottom, field_bottom, 1, 0, rows, cells);

		//left
		for(int ri = 0; ri < rows; ++ri)
		{
			if(source_fild_centre[ri][cells - 1] && !field_centre[ri][cells - 1]){
				field_centre[ri][cells - 1] = new _asnode_base();
			}

			if(source_fild_left[ri][0] && !source_fild_left[ri][0]){
				field_left[ri][0] = new _asnode_base();
			}

			bool add_link = (source_fild_centre[ri][cells - 1] && source_fild_left[ri][0]);

			int centre_links_count = 0;
			int left_links_count = 0;

			if(source_fild_centre[ri][cells - 1])centre_links_count = source_fild_centre[ri][cells - 1]->links_count;
			if(source_fild_left[ri][0])left_links_count = source_fild_left[ri][0]->links_count;

			if(add_link){
				++centre_links_count;
				++left_links_count;
			}

			if(source_fild_centre[ri][cells - 1])
			{
				field_centre[ri][cells - 1]->links_count = centre_links_count;
				field_centre[ri][cells - 1]->link_nodes = new _asnode_base * [centre_links_count];

				for(int li = 0; li < source_fild_centre[ri][cells - 1]->links_count; ++li)
				{
					int r = source_fild_centre[ri][cells - 1]->link_nodes[li]->r;
					int c = source_fild_centre[ri][cells - 1]->link_nodes[li]->c;
					field_centre[ri][cells - 1]->link_nodes[li] = field_centre[r][c];
				}
			}

			if(field_left[ri][0])
			{
				field_left[ri][0]->links_count = left_links_count;
				field_left[ri][0]->link_nodes = new _asnode_base * [left_links_count];

				for(int li = 0; li < source_fild_left[ri][0]->links_count; ++li)
				{
					int r = source_fild_left[ri][0]->link_nodes[li]->r;
					int c = source_fild_left[ri][0]->link_nodes[li]->c;
					field_left[ri][0]->link_nodes[li] = field_left[r][c];
				}
			}

			if(add_link){
				field_centre[ri][cells - 1]->link_nodes[centre_links_count - 1] = field_left[ri][0];
				field_left[ri][0]->link_nodes[left_links_count - 1] = field_centre[ri][cells - 1];
			}
		}

		//right
		for(int ri = 0; ri < rows; ++ri)
		{
			if(source_fild_centre[ri][0] && !source_fild_centre[ri][0]){
				field_centre[ri][0] = new _asnode_base();
			}

			if(source_fild_right[ri][cells - 1] && !field_right[ri][cells - 1]){
				field_right[ri][cells - 1] = new _asnode_base();
			}

			bool add_link = (source_fild_centre[ri][0] && source_fild_right[ri][cells - 1]);

			int centre_links_count = 0;
			int right_links_count = 0;

			if(source_fild_centre[ri][0])centre_links_count = source_fild_centre[ri][0]->links_count;
			if(source_fild_right[ri][cells - 1])right_links_count = source_fild_right[ri][cells - 1]->links_count;

			if(add_link){
				++centre_links_count;
				++right_links_count;
			}

			if(source_fild_centre[ri][0])
			{
				field_centre[ri][0]->links_count = centre_links_count;
				field_centre[ri][0]->link_nodes = new _asnode_base * [centre_links_count];

				for(int li = 0; li < source_fild_centre[ri][0]->links_count; ++li)
				{
					int r = source_fild_centre[ri][0]->link_nodes[li]->r;
					int c = source_fild_centre[ri][0]->link_nodes[li]->c;
					field_centre[ri][0]->link_nodes[li] = field_centre[r][c];
				}
			}

			if(field_right[ri][cells - 1])
			{
				field_right[ri][cells - 1]->links_count = right_links_count;
				field_right[ri][cells - 1]->link_nodes = new _asnode_base * [right_links_count];

				for(int li = 0; li < source_fild_right[ri][cells - 1]->links_count; ++li)
				{
					int r = source_fild_right[ri][cells - 1]->link_nodes[li]->r;
					int c = source_fild_right[ri][cells - 1]->link_nodes[li]->c;
					field_right[ri][cells - 1]->link_nodes[li] = field_right[r][c];
				}
			}

			if(add_link){
				field_centre[ri][0]->link_nodes[centre_links_count - 1] = field_right[ri][cells - 1];
				field_right[ri][cells - 1]->link_nodes[right_links_count - 1] = field_centre[ri][0];
			}
		}

		//top
		for(int ci = 0; ci < cells; ++ci)
		{
			if(source_fild_centre[rows - 1][ci] && !field_centre[rows - 1][ci]){
				field_centre[rows - 1][ci] = new _asnode_base();
			}

			if(source_fild_top[0][ci] && !field_top[0][ci]){
				field_top[0][ci] = new _asnode_base();
			}

			bool add_link = (source_fild_centre[rows - 1][ci] && source_fild_top[0][ci]);

			int centre_links_count = 0;
			int top_links_count = 0;

			if(source_fild_centre[rows - 1][ci])centre_links_count = source_fild_centre[rows - 1][ci]->links_count;
			if(source_fild_top[0][ci])top_links_count = source_fild_top[0][ci]->links_count;

			if(add_link){
				++centre_links_count;
				++top_links_count;
			}

			if(source_fild_centre[rows - 1][ci])
			{
				field_centre[rows - 1][ci]->links_count = centre_links_count;
				field_centre[rows - 1][ci]->link_nodes = new _asnode_base * [centre_links_count];

				for(int li = 0; li < source_fild_centre[rows - 1][ci]->links_count; ++li)
				{
					int r = source_fild_centre[rows - 1][ci]->link_nodes[li]->r;
					int c = source_fild_centre[rows - 1][ci]->link_nodes[li]->c;
					field_centre[rows - 1][ci]->link_nodes[li] = field_centre[r][c];
				}
			}

			if(field_top[0][ci])
			{
				field_top[0][ci]->links_count = top_links_count;
				field_top[0][ci]->link_nodes = new _asnode_base * [top_links_count];

				for(int li = 0; li < source_fild_top[0][ci]->links_count; ++li)
				{
					int r = source_fild_top[0][ci]->link_nodes[li]->r;
					int c = source_fild_top[0][ci]->link_nodes[li]->c;
					field_top[0][ci]->link_nodes[li] = field_top[r][c];
				}
			}

			if(add_link){
				field_centre[rows - 1][ci]->link_nodes[centre_links_count - 1] = field_top[0][ci];
				field_top[0][ci]->link_nodes[top_links_count - 1] = field_centre[rows - 1][ci];
			}
		}

		//bottom
		for(int ci = 0; ci < cells; ++ci)
		{
			if(source_fild_centre[0][ci] && !field_centre[0][ci]){
				field_centre[0][ci] = new _asnode_base();
			}

			if(source_fild_bottom[rows - 1][ci] && !field_bottom[rows - 1][ci]){
				field_bottom[rows - 1][ci] = new _asnode_base();
			}

			bool add_link = (source_fild_centre[0][ci] && source_fild_bottom[rows - 1][ci]);

			int centre_links_count = 0;
			int bottom_links_count = 0;

			if(source_fild_centre[0][ci])centre_links_count = source_fild_centre[0][ci]->links_count;
			if(source_fild_bottom[rows - 1][ci])bottom_links_count = source_fild_bottom[rows - 1][ci]->links_count;

			if(add_link){
				++centre_links_count;
				++bottom_links_count;
			}

			if(source_fild_centre[0][ci])
			{
				field_centre[0][ci]->links_count = centre_links_count;
				field_centre[0][ci]->link_nodes = new _asnode_base * [centre_links_count];

				for(int li = 0; li < source_fild_centre[0][ci]->links_count; ++li)
				{
					int r = source_fild_centre[0][ci]->link_nodes[li]->r;
					int c = source_fild_centre[0][ci]->link_nodes[li]->c;
					field_centre[0][ci]->link_nodes[li] = field_centre[r][c];
				}
			}

			if(field_bottom[rows - 1][ci])
			{
				field_bottom[rows - 1][ci]->links_count = bottom_links_count;
				field_bottom[rows - 1][ci]->link_nodes = new _asnode_base * [bottom_links_count];

				for(int li = 0; li < source_fild_bottom[rows - 1][ci]->links_count; ++li)
				{
					int r = source_fild_bottom[rows - 1][ci]->link_nodes[li]->r;
					int c = source_fild_bottom[rows - 1][ci]->link_nodes[li]->c;
					field_bottom[rows - 1][ci]->link_nodes[li] = field_bottom[r][c];
				}
			}

			if(add_link){
				field_centre[0][ci]->link_nodes[centre_links_count - 1] = field_bottom[rows - 1][ci];
				field_bottom[rows - 1][ci]->link_nodes[bottom_links_count - 1] = field_centre[0][ci];
			}
		}
	}

	int test(
		_asnode_base*** field_side,
		int start_centre_row,
		int start_centre_cell,
		int end_centre_row,
		int end_centre_cell,
		int start_side_row,
		int start_side_cell,
		int end_side_row,
		int end_side_cell,
		int steps_centre,
		int steps_side
	)
	{
		for(int centre_ri = start_centre_row; centre_ri < end_centre_row; centre_ri += steps_centre)
		{
			for(int centre_ci = start_centre_cell; centre_ci < end_centre_cell; centre_ci += steps_centre)
			{
				if(!field_centre[centre_ri][centre_ci])continue;

				for(int side_ri = start_side_row; side_ri < end_side_row; side_ri += steps_side)
				{
					for(int side_ci = start_side_cell; side_ci < end_side_cell; side_ci += steps_side)
					{
						if(!field_side[side_ri][side_ci])continue;

						//printf("                                  \r");
						//printf("test %d %d -> %d %d\r", centre_ri, centre_ci, side_ri, side_ci);
						_PATH_TRACK* path = pAStar->FindPath(field_centre[centre_ri][centre_ci], field_side[side_ri][side_ci]);
						if(path){
							delete path;
							//printf("path found! start centre:%d %d end side:%d %d\n", centre_ri, centre_ci, side_ri, side_ci);
							return 1;
						}
					}
				}
			}
		}

		return 0;
	}
};

struct _zfrltp_param{
	int map_row = -1;
	cNavigation* navigation = 0;
};

DWORD WINAPI ZonesFindRowLinksThreadProc(void* pParam)
{
	_zfrltp_param* thread_param = (_zfrltp_param*)pParam;
	cNavigation* navigation = thread_param->navigation;

	int zone_rows = navigation->ZoneRows;
	int zone_cells = navigation->ZoneCells;
	int map_cells = navigation->MapCells;

	_map_asnode*** nav_zones = navigation->NavZones;

	//printf("ZonesFindRowLinksThreadProc row:%d\n", thread_param->map_row);

	_CopyNodes* copy_nodes = new _CopyNodes(zone_rows, zone_cells);
	copy_nodes->pAStar = new cAStar();

	for(int ic = 0; ic < map_cells; ++ic)
	{
		int mr = thread_param->map_row;
		int mc = ic;

		_map_asnode* zone = nav_zones[mr][mc];

		if(!zone){
			printf("ZonesFindRowLinksThreadProc::centre zone not found\n");
			return -1;
		}

		navigation->convert_pos_to_loop(&mr, &mc, navigation->MapRows, navigation->MapCells);

		_i2p l = _i2p(mr, mc + 1);
		_i2p r = _i2p(mr, mc - 1);
		_i2p t = _i2p(mr + 1, mc);
		_i2p b = _i2p(mr - 1, mc);

		navigation->convert_pos_to_loop(&l.row, &l.cell, navigation->MapRows, navigation->MapCells);
		navigation->convert_pos_to_loop(&r.row, &r.cell, navigation->MapRows, navigation->MapCells);
		navigation->convert_pos_to_loop(&t.row, &t.cell, navigation->MapRows, navigation->MapCells);
		navigation->convert_pos_to_loop(&b.row, &b.cell, navigation->MapRows, navigation->MapCells);

		_map_asnode* l_zone = navigation->NavZones[l.row][l.cell];
		_map_asnode* r_zone = navigation->NavZones[r.row][r.cell];
		_map_asnode* t_zone = navigation->NavZones[t.row][t.cell];
		_map_asnode* b_zone = navigation->NavZones[b.row][b.cell];

		if(!l_zone || !r_zone || !t_zone || !b_zone){
			printf("side zone not found.\n");
			return -2;
		}

		copy_nodes->set_sources(
			zone->field,
			navigation->NavZones[l_zone->r][l_zone->c]->field,
			navigation->NavZones[r_zone->r][r_zone->c]->field,
			navigation->NavZones[t_zone->r][t_zone->c]->field,
			navigation->NavZones[b_zone->r][b_zone->c]->field
		);

		copy_nodes->init_fields();

		copy_nodes->copy();

		//printf("test left\n");
		int add_link_left = copy_nodes->test(copy_nodes->field_left, 1, 1, zone_rows - 1, zone_cells - 1, 1, 1, zone_rows - 1, zone_cells - 1, 4, 4);
		//printf("test right\n");
		int add_link_right = copy_nodes->test(copy_nodes->field_right, 1, 1, zone_rows - 1, zone_cells - 1, 1, 1, zone_rows - 1, zone_cells - 1, 4, 4);
		//printf("test top\n");
		int add_link_top = copy_nodes->test(copy_nodes->field_top, 1, 1, zone_rows - 1, zone_cells - 1, 1, 1, zone_rows - 1, zone_cells - 1, 4, 4);
		//printf("test bottom\n");
		int add_link_bottom = copy_nodes->test(copy_nodes->field_bottom, 1, 1, zone_rows - 1, zone_cells - 1, 1, 1, zone_rows - 1, zone_cells - 1, 4, 4);

		//printf("zone:[%d %d] add ll:%d rl:%d tl:%d bl:%d\n", thread_param->map_row, ic, add_link_left, add_link_right, add_link_top, add_link_bottom);

		copy_nodes->free_fields();

		zone->links_count = add_link_left + add_link_right + add_link_top + add_link_bottom;
		zone->link_nodes = new _asnode_base * [zone->links_count];

		int cur_link = 0;

		if(add_link_left)zone->link_nodes[cur_link++] = nav_zones[l.row][l.cell];
		if(add_link_right)zone->link_nodes[cur_link++] = nav_zones[r.row][r.cell];
		if(add_link_top)zone->link_nodes[cur_link++] = nav_zones[t.row][t.cell];
		if(add_link_bottom)zone->link_nodes[cur_link++] = nav_zones[b.row][b.cell];
	}

	delete copy_nodes->pAStar;
	delete copy_nodes;

	printf("ZonesFindRowLinksThreadProc finished row:%d\n", thread_param->map_row);

	delete thread_param;

	return 0;
}

int cMapBuilder::BuildLinks()
{
	for(int mr = 0; mr < Navigation.MapRows; mr += 2)
	{
		_zfrltp_param* thparam = new _zfrltp_param();
		thparam->map_row = mr;
		thparam->navigation = &Navigation;
		
		printf("start thread --- row:%d\n", mr);
		HANDLE hThreads = CreateThread(NULL, 0, &ZonesFindRowLinksThreadProc, thparam, 0, NULL);
	}

	for(int mr = 1; mr < Navigation.MapRows; mr += 2)
	{
		_zfrltp_param* thparam = new _zfrltp_param();
		thparam->map_row = mr;
		thparam->navigation = &Navigation;

		printf("start thread --- row:%d\n", mr);
		HANDLE hThreads = CreateThread(NULL, 0, &ZonesFindRowLinksThreadProc, thparam, 0, NULL);
	}

	return 0;
}

void cMapBuilder::WriteMap(const char* fname)
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
			fwrite(HeightsTable[mr][mc], 1, sizeof(word) * ((zone_rows + 1) * (zone_cells + 1)), pf);

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