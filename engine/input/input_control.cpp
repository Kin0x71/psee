#include "input_control.h"
#include "timer.h"
#include "input.h"
#include "event.h"
#include "ui/ui.h"
#include "ui/windows/ui_name_plates.h"
#include "ui/windows/ui_console.h"

#include "ui/windows/ui_editor.h"
#include "debug.h"

extern cTimer Timer;
extern cScreen Screen;

extern cCamera Camera;
extern cInput Input;
extern cUi Ui;

#define _CLIENT_COMAND_ 100

void ICCursorButtonEvent(int Button,uint Event);
void ICKeyCallback(_KEY_KODE_ key,int stat);

namespace input_control
{
	float LastWorldCursorX=0.0f;
	float LastWorldCursorY=0.0f;
};

void input_control::Init()
{
	cEvent::EventKey.Subscribe(ICKeyCallback);
	cEvent::EventCursorButton.Subscribe(ICCursorButtonEvent);
}
/*
void ClientCommandReply(_SocketEvent* pse)
{
	if(pse->ErrorFlag){
		DBG_LOG("ERROR ClientCommandReply");
		return;
	}

	_str_tag_node* pTagsTree=cPrtool::CollectTags(pse->body,pse->size);

	NetControl.ParseServerMessage(pTagsTree);

	switch(pse->event)
	{
		case _CLIENT_COMAND_:
			
		break;
	}
}
*/
bool ExecuteLocaleConsoleCommand(char* data)
{
	//DBG_LOG("%s",data);
	if(!strcmp(data,"editor"))
	{
		Ui.UiCore.CreateUiWindow(UI_BASE,"Editor",win_reg(5.0f,5.0f,20.0f,37.0f));
		
		return true;
	}
	
	return false;
}

void input_control_UiConsole_OnEnter(cUiBase* conole,char* data)
{
	//DBG_LOG("input_control_UiConsole_OnEnter:%s",data);
	
	if(!data)return;
	
	if(data[0]=='/')
	{
		if(!ExecuteLocaleConsoleCommand(&data[1]))
		{
			char send_buff[1024];
			strcpy(send_buff,"client_command?");
			int bofs=strlen(send_buff);
			int dofs=1;
			int i=1;
			int len=0;
			for(;;++i)
			{
				if(data[i]==' ' || data[i]==0)
				{
					len=i-dofs;
					memcpy(&send_buff[bofs],"command[]=",9);
					bofs+=9;
					//DBG_LOG("[%s] %d",&data[dofs],len);
					memcpy(&send_buff[bofs],&data[dofs],len);
					//send_buff[bofs+len]=0;
					//DBG_LOG("(%s)",send_buff);
					dofs+=len+1;
					bofs+=len;

					if(data[i]){
						send_buff[bofs++]='&';
					}else{
						send_buff[bofs++]=0;
						//DBG_LOG("END");
						break;
					}
				}
			}

			//DBG_LOG("%s",send_buff);
			//Get(send_buff,(_SocCollbackPtr)ClientCommandReply,_CLIENT_COMAND_);
		}
	}
}

void input_control::Update()
{
	int viewport[4]={0,Screen.height+Screen.ofsy,Screen.width+Screen.ofsx,-Screen.height};
	
	float objectCoordinate[3];
	
	UnProject(Input.Cursor.Pos.x,Input.Cursor.Pos.y,0.1f,(float*)&Camera.ViewMx,(float*)&Camera.ProjMx,viewport,objectCoordinate);

	vec3 vViewPos(objectCoordinate[0],objectCoordinate[1],objectCoordinate[2]);

	vec3 vPlanePos=vec3(0.0f,0.0f,0.0f);
	vec3 vPlaneNormal=vec3(0.0f,1.0f,0.0f);
	
	vec3 vDir=vViewPos-Camera.Eye;

	vec3 vCentr=IntersectionPoint(Camera.Eye,vDir,vPlanePos,vPlaneNormal);
	
	//DebugObjects.DrawLine(Camera.Eye.x,Camera.Eye.y-0.01f,Camera.Eye.z,vCentr.x,vCentr.y,vCentr.z,0.01f);
	
	float xoffset=LastWorldCursorX-(vCentr.x-Camera.Eye.x);
	float yoffset=LastWorldCursorY-(vCentr.z-Camera.Eye.z);
			
	LastWorldCursorX=vCentr.x-Camera.Eye.x;
	LastWorldCursorY=vCentr.z-Camera.Eye.z;
	
	if(!Ui.UiCore.CursorAboveUi && !Ui.UiCore.FocusedWindow)
	{
		if(Input.Cursor.Pos.changed)
		{
			//DBG_LOG("%f-%f=%f",LastWorldCursorX,vCentr.x,LastWorldCursorX-vCentr.x);
			
#if defined(ANDROID_SPACE)
			if(Input.Cursor.android_last_event==ANDROID_INPUT_EVENT_TOUCH_MOVE)
#elif defined(LINUX_SPACE) || defined(WINDOWS_SPACE)
			if(Input.Cursor.CursorButton1)
#endif
			{
				//DBG_LOG("x:%f y:%f",xoffset,yoffset);
				//Camera.MoveLeftRight(xoffset);
				//Camera.Forward(-yoffset);
				//return;
			}
		}

	}
	/*
	if(!pScene)return;
	
	pScene->WorldCursor=vCentr;

	if(pLight1DBG){
		*pLight1DBG->pPosCone=vec4(vCentr.x,3.0f,vCentr.z,-1.0f);
	}

	if(BarrelObj){
		//DBG_LOG("%f %f\n",vCentr.x,vCentr.z);
		//BarrelObj->Transform.Position(vCentr);
		//BarrelObj->Transform.Update();
	}
	*/
	if(Input.Cursor.Pos.changed)
	{
		Ui.UiCore.UiBaseCursorMoveEvent(Input.Cursor.Pos.x,Input.Cursor.Pos.y);
	}

	bool uiclick=false;

	if(Input.Cursor.CursorButton1Down)
	{
		uiclick=Ui.UiCore.UiBaseCursorDownEvent(Input.Cursor.Pos.x,Input.Cursor.Pos.y);
	}
	
	if(Input.Cursor.CursorButton1Up)
	{
		Ui.UiCore.UiBaseCursorUpEvent(Input.Cursor.Pos.x,Input.Cursor.Pos.y);
	}
}

void ICCursorButtonEvent(int Button,uint Event)
{
	//DBG_LOG("ICCursorButtonEvent %d %d %d",Button,Event,Ui.UiCore.CursorAboveUi);
	if(Ui.UiCore.CursorAboveUi)return;
	/*
	if(pScene->pLandscape)
	{
//#if defined(ANDROID_SPACE)
		
//#elif defined(LINUX_SPACE) || defined(WINDOWS_SPACE)
		if(Event==CURSOR_DOWN)
		{
			if(Button==1)
			{
				//DBG_LOG("%d %d",pScene->pLandscape->LandscapeCursor.y,pScene->pLandscape->LandscapeCursor.x);

				int Retount=0;
				cObject* pRetObj[256];
				if(pScene->GetUnitFromCell(pScene->pLandscape->LandscapeCursor.y,pScene->pLandscape->LandscapeCursor.x,pRetObj,&Retount))
				{
					SelectPlayer.Select((cObjectUnit*)pRetObj[0]);
					((cUiNamePlates::_name_plate_item*)((cObjectUnit*)pRetObj[0])->link_scene->name_plate_item)->NamePlate->SetTextColor(200,255,0);
					DBG_LOG("Select %s %d",pRetObj[0]->Name,((cObjectUnit*)pRetObj[0])->UnitControl.track_id);
				}

			}else if(Button==2)
			{
				SelectPlayer.Enum();

				while(cObjectUnit* selunit=SelectPlayer.Next())
				{
					selunit->UnitControl.PlayerMoveOnCell(pScene->pLandscape->LandscapeCursor.y,pScene->pLandscape->LandscapeCursor.x);
				}
			}
		}
//#endif
	}*/
}

void ICKeyCallback(_KEY_KODE_ key,int stat)
{
	if(!stat)return;

	Ui.UiCore.UiBaseKeyDownEvent(key);
}