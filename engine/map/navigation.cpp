#include "navigation.h"

#include <engine/debug/primitives.h>

void _map_asnode::AllockField(int NodesCount, int ZoneRows, int ZoneCells)
{
	nodes_count = NodesCount;
	nodes = new _zone_asnode[nodes_count];

	field = new _zone_asnode ** [ZoneRows];

	field_rows = ZoneRows;
	field_cells = ZoneCells;

	for(int ri = 0; ri < field_rows; ++ri)
	{
		field[ri] = new _zone_asnode * [field_cells];

		for(int ci = 0; ci < field_cells; ++ci)
		{
			field[ri][ci] = 0;
		}
	}
}

void _map_asnode::BuildLinks(cList<_zone_asnode*>& cells_list)
{
	//printf(">>> cNavigation::_map_asnode::BuildLinks()\n");
	for(int ci = 0; ci < nodes_count; ++ci)
	{
		_zone_asnode* as_node = &nodes[ci];

		auto cmp_ns = [](_zone_asnode* a, _zone_asnode* b){ if(a->c == b->c && a->r == b->r)return false; if(a->c == b->c && a->r == b->r - 1)return true; return false; };
		auto cmp_ew = [](_zone_asnode* a, _zone_asnode* b){ if(a->c == b->c && a->r == b->r)return false; if(a->c == b->c - 1 && a->r == b->r)return true; return false; };
		auto cmp_sn = [](_zone_asnode* a, _zone_asnode* b){ if(a->c == b->c && a->r == b->r)return false; if(a->c == b->c && a->r == b->r + 1)return true; return false; };
		auto cmp_we = [](_zone_asnode* a, _zone_asnode* b){ if(a->c == b->c && a->r == b->r)return false; if(a->c == b->c + 1 && a->r == b->r)return true; return false; };

		int ns_id = cells_list.compare(as_node, cmp_ns);
		int ew_id = cells_list.compare(as_node, cmp_ew);
		int sn_id = cells_list.compare(as_node, cmp_sn);
		int we_id = cells_list.compare(as_node, cmp_we);

		cList<int> links_list;

		if(ns_id != -1)links_list.push(ns_id);
		if(ew_id != -1)links_list.push(ew_id);
		if(sn_id != -1)links_list.push(sn_id);
		if(we_id != -1)links_list.push(we_id);

		as_node->links_count = links_list.count;
		as_node->link_nodes = (_asnode_base**)new _zone_asnode * [as_node->links_count];

		//printf(">>> %dx%d ns:%d ew:%d sn:%d we:%d\n", as_node->c, as_node->r, ns_id, ew_id, sn_id, we_id);

		for(int lid = 0; lid < links_list.count; ++lid)
		{
			as_node->link_nodes[lid] = &nodes[cells_list[links_list[lid]]->id];
			//printf("     ->%dx%d\n", as_node->link_nodes[lid]->c, as_node->link_nodes[lid]->r);
		}

		links_list.free();
	}
}

void cNavigation::convert_pos_to_loop(int* r, int* c, int rows, int cells)
{
	if(*r < 0)
	{
		*r += rows * -int(floor(float(*r) / float(rows)));
	}
	else if(*r >= rows)
	{
		*r -= rows * int(floor(float(*r) / float(rows)));
	}

	if(*c < 0)
	{
		*c += cells * -int(floor(float(*c) / float(cells)));
	}
	else if(*c >= cells)
	{
		*c -= cells * int(floor(float(*c) / float(cells)));
	}
}

int cNavigation::BuildZonesLinks()
{
	printf("------------------------------ cNavigation::BuildZonesLinks() ------------------------------\n");

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

		_CopyNodes(int rn, int cn, cAStar* astar_ptr)
		{
			pAStar = astar_ptr;

			rows = rn;
			cells = cn;

			field_centre = new _asnode_base ** [rows];
			field_left = new _asnode_base ** [rows];
			field_right = new _asnode_base ** [rows];
			field_top = new _asnode_base ** [rows];
			field_bottom = new _asnode_base ** [rows];

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
			copy_links(source_fild_top, field_top, 0, 0, rows-1, cells);
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
							printf("test %d %d -> %d %d\r", centre_ri, centre_ci, side_ri, side_ci);
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

	_CopyNodes CopyNodes(ZoneRows, ZoneCells, &AStar);

	for(int ir = 0; ir < MapRows; ++ir)
	{
		for(int ic = 0; ic < MapCells; ++ic)
		{
			int mr = ir;
			int mc = ic;

			convert_pos_to_loop(&mr, &mc, MapRows, MapCells);

			_map_asnode* zone = NavZones[mr][mc];
			if(!zone){
				printf("centre zone not found\n");
				return -1;
			}

			_i2p l = _i2p(mr, mc + 1);
			_i2p r = _i2p(mr, mc - 1);
			_i2p t = _i2p(mr + 1, mc);
			_i2p b = _i2p(mr - 1, mc);

			convert_pos_to_loop(&l.row, &l.cell, MapRows, MapCells);
			convert_pos_to_loop(&r.row, &r.cell, MapRows, MapCells);
			convert_pos_to_loop(&t.row, &t.cell, MapRows, MapCells);
			convert_pos_to_loop(&b.row, &b.cell, MapRows, MapCells);

			_map_asnode* l_zone = NavZones[l.row][l.cell];
			_map_asnode* r_zone = NavZones[r.row][r.cell];
			_map_asnode* t_zone = NavZones[t.row][t.cell];
			_map_asnode* b_zone = NavZones[b.row][b.cell];

			if(!l_zone || !r_zone || !t_zone || !b_zone){
				printf("side zone not found.\n");
				return -2;
			}

			CopyNodes.set_sources(
				zone->field,
				NavZones[l_zone->r][l_zone->c]->field,
				NavZones[r_zone->r][r_zone->c]->field,
				NavZones[t_zone->r][t_zone->c]->field,
				NavZones[b_zone->r][b_zone->c]->field
			);

			CopyNodes.init_fields();

			CopyNodes.copy();

			printf("test left\n");
			int add_link_left = CopyNodes.test(CopyNodes.field_left, 1, 1, ZoneRows - 1, ZoneCells - 1, 1, 1, ZoneRows - 1, ZoneCells - 1, 3, 3);
			printf("test right\n");
			int add_link_right = CopyNodes.test(CopyNodes.field_right, 1, 1, ZoneRows - 1, ZoneCells - 1, 1, 1, ZoneRows - 1, ZoneCells - 1, 3, 3);
			printf("test top\n");
			int add_link_top = CopyNodes.test(CopyNodes.field_top, 1, 1, ZoneRows - 1, ZoneCells - 1, 1, 1, ZoneRows - 1, ZoneCells - 1, 3, 3);
			printf("test bottom\n");
			int add_link_bottom = CopyNodes.test(CopyNodes.field_bottom, 1, 1, ZoneRows - 1, ZoneCells - 1, 1, 1, ZoneRows - 1, ZoneCells - 1, 3, 3);

			//printf("zone:[%d %d] add ll:%d rl:%d tl:%d bl:%d\n", mr, mc, add_link_left, add_link_right, add_link_top, add_link_bottom);

			CopyNodes.free_fields();

			zone->links_count = add_link_left + add_link_right + add_link_top + add_link_bottom;
			zone->link_nodes = new _asnode_base * [zone->links_count];

			int cur_link = 0;

			if(add_link_left)zone->link_nodes[cur_link++] = NavZones[l_zone->r][l_zone->c];
			if(add_link_right)zone->link_nodes[cur_link++] = NavZones[r_zone->r][r_zone->c];
			if(add_link_top)zone->link_nodes[cur_link++] = NavZones[t_zone->r][t_zone->c];
			if(add_link_bottom)zone->link_nodes[cur_link++] = NavZones[b_zone->r][b_zone->c];

			//printf("\r               ");
			printf("%dx%d %d%%\n", ir, ic, (((ir* MapRows) + ic) * 100) / (MapRows * MapCells));
		}
	}

	printf("\n");

	return 0;
}

void cNavigation::AddTrackingObject(cObjectBase* object)
{
	printf("cNavigation::AddTrackingObject(%s)\n", object->name);

	int field_asnode_id = -1;

	//printf("m  %d %d %p\n", object->NavInfo.map_pos.row, object->NavInfo.map_pos.cell, NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell]);
	if(NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell])
	{
		//printf("z  field:%p\n", NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell]->field);
		//printf("f  %d %d %p\n", object->NavInfo.zone_pos.row, object->NavInfo.zone_pos.cell, NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell]->field[object->NavInfo.zone_pos.row][object->NavInfo.zone_pos.cell]);

		if(NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell]->field && NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell]->field[object->NavInfo.zone_pos.row][object->NavInfo.zone_pos.cell])
		{
			field_asnode_id = NavZones[object->NavInfo.map_pos.row][object->NavInfo.map_pos.cell]->field[object->NavInfo.zone_pos.row][object->NavInfo.zone_pos.cell]->id;
		}
	}

	tracking_objects.push(
		new _tracking_item(object, object->NavInfo.map_pos.row, object->NavInfo.map_pos.cell, object->NavInfo.zone_pos.row, object->NavInfo.zone_pos.cell, field_asnode_id)
	);
}

void cNavigation::RemoveTrackingObject(cObjectBase* object)
{
	int rm_index = tracking_objects.enumerate(
		[object](_tracking_item* item){
			if(item->object == object)return true;
			return false;
		}
	);

	if(rm_index != -1)
	{
		delete tracking_objects.pop_index(rm_index);
	}
}

void cNavigation::GetMapPosition(float x, float z, _map_pos* ret)
{
	int mr = int(floor(z / float(ZoneRows)));
	int mc = int(floor(x / float(ZoneCells)));

	float zone_pos_x = float(mc * int(ZoneCells));
	float zone_pos_z = float(mr * int(ZoneRows));

	convert_pos_to_loop(&mr, &mc, MapRows, MapCells);

	ret->set(mr, mc, z - zone_pos_z, x - zone_pos_x);
}

void cNavigation::Update()
{
	tracking_objects.reset_enumerator();
	while(_tracking_item* item = tracking_objects.next())
	{
		//DrawCross(vec3(item->object->position.x, 0.01f, item->object->position.z), 0.2f, "\xff\x00\xff");
		//DrawCross(vec3(item->object->position.x, 0.01f, item->object->position.z - 2.0f), 0.2f, "\x00\xff\x00");

		//float item_cx = Navigation.NavZonesList[item->zone_id]->nodes[item->cell_id].x + 1.0f;
		//float item_cy = Navigation.NavZonesList[item->zone_id]->nodes[item->cell_id].y + 1.0f;

		//DrawCross(vec3(item_cx, 0.01f, item_cy), 0.3f, "\x00\xff\x00");

		_map_pos* pmpi = &item->object->NavInfo;

		if(item->object->position.x != item->last_pos.x || item->object->position.z != item->last_pos.z)
		{
			//printf("%s change position %f %f\n", item->object->name, item->object->position.x, item->object->position.z);

			GetMapPosition(item->object->position.x, item->object->position.z, pmpi);

			int field_nod_id = -1;
			if(NavZones[pmpi->map_pos.row][pmpi->map_pos.cell])
			{
				if(NavZones[pmpi->map_pos.row][pmpi->map_pos.cell]->field && NavZones[pmpi->map_pos.row][pmpi->map_pos.cell]->field[pmpi->zone_pos.row][pmpi->zone_pos.cell])
				{
					field_nod_id = NavZones[pmpi->map_pos.row][pmpi->map_pos.cell]->field[pmpi->zone_pos.row][pmpi->zone_pos.cell]->id;
				}
			}

			item->last_pos.x = item->object->position.x;
			item->last_pos.z = item->object->position.z;
		}

		for(int ri = 0; ri < ZoneRows; ++ri)
		{
			/*DrawLine(
				vec3(pmpi->map_pos.cell * ZoneCells, 0.05f, pmpi->map_pos.row * ZoneRows + ri),
				vec3(pmpi->map_pos.cell * ZoneCells + ZoneCells, 0.05f, pmpi->map_pos.row * ZoneRows + ri),
				"\x00\xff\x00"
			);*/

			for(int ci = 0; ci < ZoneCells; ++ci)
			{
				/*DrawLine(
					vec3(pmpi->map_pos.cell * ZoneCells + ci, 0.05f, pmpi->map_pos.row * ZoneRows),
					vec3(pmpi->map_pos.cell * ZoneCells + ci, 0.05f, pmpi->map_pos.row * ZoneRows + ZoneRows),
					"\x00\xff\x00"
				);*/

				_map_asnode* nav_zone = NavZones[pmpi->map_pos.row][pmpi->map_pos.cell];

				if(nav_zone)
				{
					if(nav_zone->field && nav_zone->field[ri][ci])
					{
						_zone_asnode*** field = nav_zone->field;

						vec3 world_pos = nav_zone->field[ri][ci]->local_pos + vec3(pmpi->map_pos.cell * ZoneCells, 0.05f, pmpi->map_pos.row * ZoneRows);
						DrawCross(world_pos, 0.3f, "\x00\xff\x00");

						_zone_asnode* node = field[ri][ci];
						for(int li = 0; li < node->links_count; ++li)
						{
							vec3 link_world_pos = ((_zone_asnode*)node->link_nodes[li])->local_pos + vec3(pmpi->map_pos.cell * ZoneCells, 0.05f, pmpi->map_pos.row * ZoneRows);

							DrawLine(
								world_pos,
								link_world_pos,
								"\xff\x00\xff"
							);
						}
					}
				}
			}
		}

		DrawCross(vec3((pmpi->map_pos.cell * ZoneCells + pmpi->zone_pos.cell) + 0.5f, 0.1f, (pmpi->map_pos.row * ZoneRows + pmpi->zone_pos.row) + 0.5f), 0.5f, "\xff\x00\x00");
	}
}

bool cNavigation::SetNode(int zone_id, int cell_id)
{
	return false;
}
/*
bool cNavigation::FindPath(vec3& start, vec3& end, _path_container* ret_path_container)
{
	int start_zone_row = -1;
	int start_zone_cell = -1;
	int start_field_cell_id = -1;

	if(FindIdsByCoord(start.x, start.z, &start_zone_row, &start_zone_cell, &start_field_cell_id))
	{
		//printf("start_zone_id:%d start_cell_id:%d x:%d Y:%d\n", start_zone_id, start_cell_id, NavZones[start_zone_id].nodes[start_cell_id].c, NavZones[start_zone_id].nodes[start_cell_id].r);

		int end_zone_row = -1;
		int end_zone_cell = -1;
		int end_field_cell_id = -1;

		if(FindIdsByCoord(end.x, end.z, &end_zone_row, &end_zone_cell, &end_field_cell_id))
		{
			//printf("\tend_zone_id:%d end_cell_id:%d x:%d Y:%d\n", end_zone_id, end_cell_id, NavZones[end_zone_id].nodes[end_cell_id].c, NavZones[end_zone_id].nodes[end_cell_id].r);
			_map_asnode* zone = NavZones[start_zone_row][start_zone_cell];

			uint start_x = zone->nodes[start_field_cell_id].c;
			uint start_y = zone->nodes[start_field_cell_id].r;
			uint end_x = zone->nodes[end_field_cell_id].c;
			uint end_y = zone->nodes[end_field_cell_id].r;
			//printf("%d %d->%d %d\n", start_x, start_y, end_x, end_y);
			_PATH_TRACK* path = AStar.FindPath(start_x, start_y, end_x, end_y, (_asnode_base***)zone->field);

			if(path)
			{
				ret_path_container->row = start_zone_row;
				ret_path_container->cell = start_zone_cell;

				for(int i = 0; i < path->num; ++i)
				{
					ret_path_container->path_list.push(zone->nodes[path->points[i].as_id].world_pos);
				}

				delete path;

				return true;
			}
		}
	}
	
	return false;
}
*/
bool cNavigation::MapFindPath(const _i2p& start, const _i2p& end)
{
	return false;
}

cList<vec3>* cNavigation::ZoneFindPath(_map_asnode* zone, vec2& local_start, vec2& local_end)
{
	_PATH_TRACK* path = AStar.FindPath(
		uint(local_start.x / ZoneCells),
		uint(local_start.y / ZoneRows),
		uint(local_end.x / ZoneCells),
		uint(local_end.y / ZoneRows),
		(_asnode_base***)zone->field
	);

	if(path)
	{
		cList<vec3>* path_list = new cList<vec3>();

		for(int i = 0; i < path->num; ++i)
		{
			path_list->push(zone->nodes[path->points[i].as_id].local_pos);
		}

		delete path;

		return path_list;
	}

	return 0;
}