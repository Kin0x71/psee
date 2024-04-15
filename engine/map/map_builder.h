#ifndef _MAP_BUILDER_H_
#define _MAP_BUILDER_H_

#include "navigation.h"

class cMapBuilder{
public:
	const char* LandscapeHeightMapFile = 0;
	int FullMapWidth = 0;
	int FullMapHeight = 0;

	cNavigation Navigation;

	word*** HeightsTable = 0;

	~cMapBuilder()
	{
		if(HeightsTable){
			for(int ri = 0; ri < Navigation.MapRows; ++ri)
			{
				for(int ci = 0; ci < Navigation.MapRows; ++ci)
				{
					delete[] HeightsTable[ri][ci];
				}

				delete[] HeightsTable[ri];
			}

			delete[] HeightsTable;
		}
	}

	bool ReadHeightMap(const char* landscape_height_map_file, uint field_rows, uint field_cells);
	_map_asnode* ReadZone(
		int zone_rows,
		int zone_cells,
		int map_rows,
		int map_cells,
		int map_row,
		int map_cell
	);
	bool LoadZone(int map_row, int map_cell);
	int BuildLinks();
	void WriteMap(const char* fname);
};

#endif