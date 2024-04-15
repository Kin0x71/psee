#ifndef _UI_NAME_PLATES_H_
#define _UI_NAME_PLATES_H_

class cUiScrFloatText;
class cTexture;

class cUiNamePlates
{
public:
	struct _name_plate_item
	{
		_name_plate_item* next;
		_name_plate_item* prev;
		
		cUiScrFloatText* NamePlate;
		
		float start_time;
		float life_time;
		
		_name_plate_item();
		~_name_plate_item();
	};
private:
	_name_plate_item* NamePlatesList;
	_name_plate_item* NamePlatesLast;
	int PlatesCount;
	
public:
	
	cUiNamePlates();
	~cUiNamePlates();
	
	void* Add(char* caption,float x,float y,float z,float life_time=0.0f);
	void Remove(void* item);
};

#endif