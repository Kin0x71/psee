#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "render/renderer.h"
#include "scene/scene.h"
//#include "ui/ui_core.h"

class cGameBase{
public:
	virtual void Create(){};
	virtual void UpdateBeforeCalculations(){};
	virtual void UpdateAfterCalculations(){};
	virtual void UpdateAfterRender(){};
};

namespace Engine {

	extern cRenderer Renderer;
	extern shaders_container* pMainShadersContainer;
	//extern cUiCore UiCore;
	extern cScene* pCurrentScene;
	extern cGameBase* pGame;

	extern double last_frame_time;
	extern bool DBGShowCollisionsWorldFlag;

	int Initialize(cGameBase* game);
	int SetScene(cScene* scene);
	int Update();
};

#endif