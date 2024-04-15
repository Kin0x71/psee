#include "ui_uett.h"
#include "../ui.h"

extern cScreen* pScreen;
extern cUi Ui;
extern uint OPT_SCREEN_W;
extern uint OPT_SCREEN_H;

cUiUett::cUiUett()
{
	pUiWinUETT=0;
	Texture00=0;
	Texture01=0;
	Texture02=0;
	Texture03=0;
	Texture040=0;
	Texture042=0;
	Texture043=0;
	Texture044=0;
	Texture045=0;
	Texture05=0;
	Texture06=0;
	Texture07=0;
	Texture08=0;
	Texture09=0;
	Texture10=0;
}

cUiUett::~cUiUett()
{//DBG_LOG("~cUiUett()");
	if(pUiWinUETT)
	{
		Ui.UiCore.UnlinkWindow(pUiWinUETT);
		
		delete pUiWinUETT;
		pUiWinUETT=0;
		delete Texture00;
		delete Texture01;
		delete Texture02;
		delete Texture03;
		delete Texture040;
		delete Texture042;
		delete Texture043;
		delete Texture044;
		delete Texture045;
		delete Texture05;
		delete Texture06;
		delete Texture07;
		delete Texture08;
		delete Texture09;
		delete Texture10;
	}
}

cUiBase* cUiUett::Create()
{
	cTexture* Texture00=TextureCreateFromFilePNG("data/ui/uett/00.png");
	cTexture* Texture01=TextureCreateFromFilePNG("data/ui/uett/01.png");
	cTexture* Texture02=TextureCreateFromFilePNG("data/ui/uett/02.png");
	cTexture* Texture03=TextureCreateFromFilePNG("data/ui/uett/03.png");
	cTexture* Texture040=TextureCreateFromFilePNG("data/ui/uett/040.png");
	cTexture* Texture042=TextureCreateFromFilePNG("data/ui/uett/042.png");
	cTexture* Texture043=TextureCreateFromFilePNG("data/ui/uett/043.png");
	cTexture* Texture044=TextureCreateFromFilePNG("data/ui/uett/044.png");
	cTexture* Texture045=TextureCreateFromFilePNG("data/ui/uett/045.png");
	cTexture* Texture05=TextureCreateFromFilePNG("data/ui/uett/05.png");
	cTexture* Texture06=TextureCreateFromFilePNG("data/ui/uett/06.png");
	cTexture* Texture07=TextureCreateFromFilePNG("data/ui/uett/07.png");
	cTexture* Texture08=TextureCreateFromFilePNG("data/ui/uett/08.png");
	cTexture* Texture09=TextureCreateFromFilePNG("data/ui/uett/09.png");
	cTexture* Texture10=TextureCreateFromFilePNG("data/ui/uett/10.png");

	float posx=0.0f;
	float posy=0.0f;

	cUiBase* pUiWinUETT=Ui.UiCore.CreateUiWindow(UI_BASE,"uett",win_reg(posx,posy,float(OPT_SCREEN_W),float(OPT_SCREEN_H)));

	pUiWinUETT->tex.x=-(float(OPT_SCREEN_W)/float((Texture00)->surf.w))/2.0f;
	pUiWinUETT->tex.y=-(float(OPT_SCREEN_H)/float((Texture00)->surf.h))/2.0f;
	pUiWinUETT->tex.w=(float(OPT_SCREEN_W)/float((Texture00)->surf.w))/2.0f;
	pUiWinUETT->tex.h=(float(OPT_SCREEN_H)/float((Texture00)->surf.h))/2.0f;

	pUiWinUETT->SetBackground(Texture00);
	
	posx=(float(OPT_SCREEN_W)/2.0f)-(float((Texture01)->surf.w)/2.0f);
	posy=(float(OPT_SCREEN_H)/2.0f)-(float((Texture01)->surf.h)/2.0f);

	cUiBase* pUiWin01=Ui.UiCore.CreateUiWindow(UI_BASE,"01",
		win_reg(
			posx,
			posy,
			float((Texture01)->surf.w),
			float((Texture01)->surf.h)
		),
		pUiWinUETT
	);

	pUiWin01->SetBackground(Texture01,0.0f,0.0f,1.0f,1.0f);
	
	posx=0.0f;
	posy+=91.0f;

	cUiBase* pUiWin02=Ui.UiCore.CreateUiWindow(UI_BASE,"02",
		win_reg(
			posx,
			posy,
			float(OPT_SCREEN_W),
			float((Texture02)->surf.h)
		),
		pUiWinUETT
	);
	
	pUiWin02->SetBackground(Texture02,0.0f,0.0f,1.0f,1.0f);

	posy+=float((Texture02)->surf.h);

	cUiBase* pUiWin03=Ui.UiCore.CreateUiWindow(UI_BASE,"03",
		win_reg(
			posx,
			posy,
			float(OPT_SCREEN_W),
			float((Texture03)->surf.h)
		),
		pUiWinUETT
	);
	
	pUiWin03->SetBackground(Texture03,0.0f,0.0f,1.0f,1.0f);
	
	float nw=float((Texture042)->surf.w);
	float nh=float((Texture042)->surf.h);
	float lw=(float(OPT_SCREEN_W)/7.0f)-(nw+7.0f);
	float lh=float((Texture042)->surf.h);

	posx=25.0f;
	posy+=149.0f;
	//N
	cUiBase* pUiWin042=Ui.UiCore.CreateUiWindow(UI_BASE,"042",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin042->SetBackground(Texture042,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0400=Ui.UiCore.CreateUiWindow(UI_BASE,"0400",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0400->tex.w=0.4f;
	pUiWin0400->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	//N
	posx+=lw;
	cUiBase* pUiWin043=Ui.UiCore.CreateUiWindow(UI_BASE,"043",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin043->SetBackground(Texture043,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0401=Ui.UiCore.CreateUiWindow(UI_BASE,"0401",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0401->tex.w=0.6f;
	pUiWin0401->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	//N
	posx+=lw;
	cUiBase* pUiWin044=Ui.UiCore.CreateUiWindow(UI_BASE,"044",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin044->SetBackground(Texture044,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0402=Ui.UiCore.CreateUiWindow(UI_BASE,"0402",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0402->tex.w=0.8f;
	pUiWin0402->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	//N 5
	posx+=lw;
	cUiBase* pUiWin045=Ui.UiCore.CreateUiWindow(UI_BASE,"045",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin045->SetBackground(Texture045,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0403=Ui.UiCore.CreateUiWindow(UI_BASE,"0403",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0403->tex.w=1.0f;
	pUiWin0403->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	//N
	posx+=lw;
	cUiBase* pUiWin046=Ui.UiCore.CreateUiWindow(UI_BASE,"046",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin046->SetBackground(Texture044,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0404=Ui.UiCore.CreateUiWindow(UI_BASE,"0404",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0404->tex.w=0.8f;
	pUiWin0404->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	//N
	posx+=lw;
	cUiBase* pUiWin047=Ui.UiCore.CreateUiWindow(UI_BASE,"047",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin047->SetBackground(Texture043,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0405=Ui.UiCore.CreateUiWindow(UI_BASE,"0405",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0405->tex.w=0.6f;
	pUiWin0405->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	//N
	posx+=lw;
	cUiBase* pUiWin048=Ui.UiCore.CreateUiWindow(UI_BASE,"048",win_reg(posx,posy,nw,nh),pUiWinUETT);
	pUiWin048->SetBackground(Texture042,0.0f,0.0f,1.0f,1.0f);
	//L
	posx+=nw;
	cUiBase* pUiWin0406=Ui.UiCore.CreateUiWindow(UI_BASE,"0406",win_reg(posx,posy,lw,lh),pUiWinUETT);
	pUiWin0406->tex.w=0.4f;
	pUiWin0406->SetBackground(Texture040,0.0f,0.0f,1.0f,1.0f);
	
	posx=0.0f;
	posy+=lh;
	cUiBase* pUiWin05=Ui.UiCore.CreateUiWindow(UI_BASE,"05",win_reg(posx,posy,float(OPT_SCREEN_W),float((Texture05)->surf.h)),pUiWinUETT);
	pUiWin05->SetBackground(Texture05,0.0f,0.0f,1.0f,1.0f);

	posy+=(Texture05)->surf.h;
	cUiBase* pUiWin06=Ui.UiCore.CreateUiWindow(UI_BASE,"06",win_reg(posx,posy,float(OPT_SCREEN_W),float((Texture06)->surf.h)),pUiWinUETT);
	pUiWin06->tex.w=float(OPT_SCREEN_W)/((Texture06)->surf.h*2.0f);
	pUiWin06->SetBackground(Texture06);

	posy=float((Texture09)->surf.h);
	posx=float((Texture10)->surf.w);

	cUiBase* pUiWin070=Ui.UiCore.CreateUiWindow(UI_BASE,"070",win_reg(posx,posy,float((Texture07)->surf.w),float((Texture07)->surf.h)),pUiWinUETT);
	pUiWin070->SetBackground(Texture07,0.0f,0.0f,1.0f,1.0f);

	posx=float(OPT_SCREEN_W)-(Texture07)->surf.w-(Texture10)->surf.w;
	cUiBase* pUiWin071=Ui.UiCore.CreateUiWindow(UI_BASE,"071",win_reg(posx,posy,float((Texture07)->surf.w),float((Texture07)->surf.h)),pUiWinUETT);
	pUiWin071->SetBackground(Texture07,0.0f,0.0f,1.0f,1.0f);

	posy=float(OPT_SCREEN_H)-(Texture09)->surf.h-(Texture08)->surf.h;
	posx=float((Texture10)->surf.w);

	cUiBase* pUiWin080=Ui.UiCore.CreateUiWindow(UI_BASE,"080",win_reg(posx,posy,float((Texture08)->surf.w),float((Texture08)->surf.h)),pUiWinUETT);
	pUiWin080->SetBackground(Texture08,0.0f,0.0f,1.0f,1.0f);

	posx=float(OPT_SCREEN_W)-(Texture08)->surf.w-(Texture10)->surf.w;
	cUiBase* pUiWin081=Ui.UiCore.CreateUiWindow(UI_BASE,"081",win_reg(posx,posy,float((Texture08)->surf.w),float((Texture08)->surf.h)),pUiWinUETT);
	pUiWin081->SetBackground(Texture08,0.0f,0.0f,1.0f,1.0f);
	//
	posx=0.0f;
	posy=0.0f;
	cUiBase* pUiWin100=Ui.UiCore.CreateUiWindow(UI_BASE,"100",win_reg(posx,posy,float((Texture10)->surf.w),float(OPT_SCREEN_H)),pUiWinUETT);
	pUiWin100->SetBackground(Texture10,0.0f,0.0f,1.0f,float(OPT_SCREEN_H)/(Texture10)->surf.h);

	posx=float(OPT_SCREEN_W)-(Texture10)->surf.w;

	cUiBase* pUiWin101=Ui.UiCore.CreateUiWindow(UI_BASE,"100",win_reg(posx,posy,float((Texture10)->surf.w),float(OPT_SCREEN_H)),pUiWinUETT);
	pUiWin101->SetBackground(Texture10,0.0f,0.0f,1.0f,float(OPT_SCREEN_H)/(Texture10)->surf.h);
	//
	posx=0.0f;
	posy=0.0f;
	cUiBase* pUiWin090=Ui.UiCore.CreateUiWindow(UI_BASE,"090",win_reg(posx,posy,float(OPT_SCREEN_W),float((Texture09)->surf.h)),pUiWinUETT);
	pUiWin090->SetBackground(Texture09,0.0f,0.0f,float(OPT_SCREEN_W)/(Texture09)->surf.w,1.0f);

	posy=float(OPT_SCREEN_H)-(Texture09)->surf.h;
	cUiBase* pUiWin091=Ui.UiCore.CreateUiWindow(UI_BASE,"091",win_reg(posx,posy,float(OPT_SCREEN_W),float((Texture09)->surf.h)),pUiWinUETT);
	pUiWin091->SetBackground(Texture09,0.0f,0.0f,float(OPT_SCREEN_W)/(Texture09)->surf.w,1.0f);

	return pUiWinUETT;
}