#include "engine.h"
#include <cstdint>
#include <iostream>

#include <engine/common/timer.h>
#include <engine/common/screen.h>
#include <engine/input/input.h>
#include <engine/files/file.h>
#include <engine/shaders/compiler/shader_compiler.h>
#include <engine/ui/text.h>

cRenderer Engine::Renderer;
shaders_container* Engine::pMainShadersContainer = 0;
//cUiCore Engine::UiCore;
cScene* Engine::pCurrentScene = 0;
cGameBase* Engine::pGame = 0;

bool Engine::DBGShowCollisionsWorldFlag = false;

cTimer Timer;
cInput Input;

static cShaderCompiler ShaderCompiler;

void EngineKeyEvent(_KEY_KODE_ Key, int KeyStat)
{

}

void EngineCursorButtonEvent(int Button, uint Event)
{

}

void EngineCursorMoveEvent(uint x, uint y)
{

}

int Engine::Initialize(cGameBase* game)
{
#if defined(WINDOWS_SPACE)
	int WSCREEN = GetSystemMetrics(SM_CXSCREEN);
	int HSCREEN = GetSystemMetrics(SM_CYSCREEN);
#elif defined(LINUX_DESCTOP_SPACE)
	int WSCREEN = 1024;
	int HSCREEN = 768;
#endif

	Screen::CreateGameWindow(WSCREEN / 4, HSCREEN / 100, 1024, 768);
	Screen::InitializeOGL();
	Screen::SetupOGL();

	files::pFilesObject = new data_files::files();

	ShaderCompiler.LoadSemantics((char*)"./");

	pMainShadersContainer = new shaders_container();

	ShaderCompiler.CompileShader((char*)"data/shaders/main.fx", pMainShadersContainer);
	//ShaderCompiler.CompileShader((char*)"data/shaders/ui.fx", UiCore.pUiShadersContainer);

	TextBase::Init(2);
	TextBase::LoadFont(0, "data/fonts/symonMedium16.bf");
	TextBase::LoadFont(1, "data/fonts/techno.bf");

	TextBase::CreateFontTextuteColorMask(0, 0, 255, 255, 255);

	//UiCore.Init();

	pGame = game;

	pGame->Create();

	return 0;
}

int Engine::SetScene(cScene* scene)
{
	int ret = 0;

	if(pCurrentScene){
		delete pCurrentScene;
		ret = 1;
	}

	pCurrentScene = scene;

	return ret;
}

int Engine::Update()
{
	//Screen::UpdateSystemEvents();
	Input.Update();

	Screen::Clear();

	if(pCurrentScene){

		pGame->UpdateBeforeCalculations();

		pCurrentScene->Update();

		pCurrentScene->pMainCamera->Updating = true;
		pCurrentScene->pMainCamera->Update();

		Renderer.CollectViewObjects(pCurrentScene);

		Renderer.RenderShadowsDepth(pCurrentScene);

		pCurrentScene->SkyBox.WorldMx[3][0] = pCurrentScene->pMainCamera->eye.x;
		pCurrentScene->SkyBox.WorldMx[3][1] = pCurrentScene->pMainCamera->eye.y;
		pCurrentScene->SkyBox.WorldMx[3][2] = pCurrentScene->pMainCamera->eye.z;
		pCurrentScene->SkyBox.Draw(pCurrentScene->pMainCamera);

		glViewport(0, 0, pCurrentScene->pMainCamera->width, pCurrentScene->pMainCamera->height);
		glScissor(0, 0, pCurrentScene->pMainCamera->width, pCurrentScene->pMainCamera->height);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((float*)&pCurrentScene->pMainCamera->ProjMx);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((float*)&pCurrentScene->pMainCamera->ViewMx);

		if(DBGShowCollisionsWorldFlag){
			pCurrentScene->CollisionsDynamicsWorld->debugDrawWorld();
		}

		pGame->UpdateAfterCalculations();

		Renderer.Render(pCurrentScene);

		pGame->UpdateAfterRender();
	}

	Input.ResetStats();

	Screen::Swap();

	return 0;
}