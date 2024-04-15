#ifndef _UI_UETT_H_
#define _UI_UETT_H_

class cTexture;
class cUiBase;

class cUiUett
{
public:
	cUiBase* pUiWinUETT;
	cTexture** Texture00;
	cTexture** Texture01;
	cTexture** Texture02;
	cTexture** Texture03;
	cTexture** Texture040;
	cTexture** Texture042;
	cTexture** Texture043;
	cTexture** Texture044;
	cTexture** Texture045;
	cTexture** Texture05;
	cTexture** Texture06;
	cTexture** Texture07;
	cTexture** Texture08;
	cTexture** Texture09;
	cTexture** Texture10;
	
	cUiUett();
	~cUiUett();
	
	cUiBase* Create();
};

#endif
