#include "ui_name_plates.h"
#include "../ui.h"
#include "../../timer.h"

extern cUi Ui;
extern cTextBase TextBase;
extern cTimer Timer;

cUiNamePlates::_name_plate_item::_name_plate_item()
{
	next=0;
	prev=0;
	NamePlate=0;
	
	start_time=0.0f;
	life_time=0.0f;
}

cUiNamePlates::_name_plate_item::~_name_plate_item()
{//DBG_LOG("~%x %x %s",this,NamePlate,NamePlate->name);
	if(NamePlate){
		delete NamePlate;
		NamePlate=0;
	}
	
	if(next){
		delete next;
		next=0;
	}
}

cUiNamePlates::cUiNamePlates()
{
	NamePlatesLast=NamePlatesList=new _name_plate_item();
	PlatesCount=0;
}

cUiNamePlates::~cUiNamePlates()
{
	if(NamePlatesList){
		delete NamePlatesList;
		NamePlatesList=0;
	}
}

void* cUiNamePlates::Add(char* caption,float x,float y,float z,float life_time)
{
	int len=strlen(caption);
	if(!len)return 0;
	
	NamePlatesLast->next=new _name_plate_item();
	NamePlatesLast->next->prev=NamePlatesLast;
	NamePlatesLast=NamePlatesLast->next;
	
	++PlatesCount;
	
	char name[256];
	sprintf(name,"NamePlate%d",PlatesCount);
	
	float cw=0.0f;
	float ch=0.0f;
	TextBase.SizeOfText(1,caption,len,&cw,&ch);
	
	NamePlatesLast->NamePlate=(cUiScrFloatText*)Ui.UiCore.CreateUiWindow(UI_SCR_FLOAT_TEXT,name,win_reg(0.0f,0.0f,cw,ch),Ui.GameMenu->GameInfoLayer);
	NamePlatesLast->NamePlate->SetFont(1);
	NamePlatesLast->NamePlate->SetText(caption,50,255,50);
	
	NamePlatesLast->NamePlate->SetPosWorld(x,y,z);
	
	if(life_time!=0.0f){
		NamePlatesLast->start_time=Timer.ProgrammTime;
		NamePlatesLast->life_time=life_time;
	}
	
	return NamePlatesLast;
}

void cUiNamePlates::Remove(void* item)
{
	_name_plate_item* uisft=(_name_plate_item*)item;
			
	if(uisft->prev)
	{
		if(uisft->next)
		{
			uisft->prev->next=uisft->next;
			uisft->next->prev=uisft->prev;
			
		}else{
			uisft->prev->next=0;
			NamePlatesLast=NamePlatesLast->prev;
		}
	}else{
		NamePlatesList=NamePlatesList->next;
	}
	
	uisft->next=0;
	uisft->prev=0;
	
	//Ui.UiCore.UnlinkWindow(uisft->NamePlate);
	
	delete uisft;
	
	--PlatesCount;
}

