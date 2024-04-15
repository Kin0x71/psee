#include "screen.h"
#include "gl_head.h"
#include <stdio.h>

Screen::_WINDOW Screen::MainWindow;
Screen::_WINDOW Screen::GameWindow;
int Screen::width = 0;
int Screen::height = 0;
int Screen::posx = 0;
int Screen::posy = 0;

#if defined(WINDOWS_SPACE)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch(msg)
	{
		case WM_SYSKEYDOWN:
			return 0;
			break;
		case WM_COMMAND:
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool Screen::CreateGameWindow(int PosX, int PosY, int Width, int Height)
{
	wchar_t* szMainWindowClass = (wchar_t*)L"PARANOIASOFTOGLEXPERIMENTALENGINEMAINWINDOWCLASS";
	wchar_t* szGameWindowClass = (wchar_t*)L"PARANOIASOFTOGLEXPERIMENTALENGINEGAMEWINDOWCLASS";

	GameWindow.hInstance = MainWindow.hInstance = GetModuleHandle(0);

	WNDCLASSEXW wcex_main;
	wcex_main.cbSize = sizeof(WNDCLASSEXW);
	wcex_main.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex_main.lpfnWndProc = WndProc;
	wcex_main.cbClsExtra = 0;
	wcex_main.cbWndExtra = 0;
	wcex_main.hInstance = MainWindow.hInstance;
	wcex_main.hIcon = LoadIcon(wcex_main.hInstance, MAKEINTRESOURCE(2));
	wcex_main.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex_main.hbrBackground = (HBRUSH)(4);
	wcex_main.lpszMenuName = 0;
	wcex_main.lpszClassName = szMainWindowClass;
	wcex_main.hIconSm = LoadIcon(wcex_main.hInstance, MAKEINTRESOURCE(108));

	RegisterClassExW(&wcex_main);

	WNDCLASSEXW wcex_game;
	wcex_game.cbSize = sizeof(WNDCLASSEXW);
	wcex_game.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex_game.lpfnWndProc = WndProc;
	wcex_game.cbClsExtra = 0;
	wcex_game.cbWndExtra = 0;
	wcex_game.hInstance = GameWindow.hInstance;
	wcex_game.hIcon = 0;
	wcex_game.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex_game.hbrBackground = (HBRUSH)(4);
	wcex_game.lpszMenuName = 0;
	wcex_game.lpszClassName = szGameWindowClass;
	wcex_game.hIconSm = LoadIcon(wcex_game.hInstance, MAKEINTRESOURCE(108));

	RegisterClassExW(&wcex_game);

	HWND hWndMain = CreateWindowExW(
		0, szMainWindowClass, L"",
		WS_SYSMENU | WS_MINIMIZEBOX | CS_HREDRAW | CS_VREDRAW | CS_OWNDC | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		PosX, PosY, Width, Height,
		NULL,
		NULL, MainWindow.hInstance, NULL
	);

	ShowWindow(hWndMain, SW_SHOW);
	UpdateWindow(hWndMain);

	HWND hWndGame = CreateWindowExW(
		0, szGameWindowClass, L"",
		WS_CHILD | CS_HREDRAW | CS_VREDRAW | CS_OWNDC | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, Width, Height,
		hWndMain,
		NULL, GameWindow.hInstance, NULL
	);

	ShowWindow(hWndGame, SW_SHOW);
	UpdateWindow(hWndGame);

	RECT wrec;
	RECT crec;

	GetWindowRect(hWndMain, &wrec);
	GetClientRect(hWndMain, &crec);

	int offsx = (wrec.right - wrec.left - crec.right) / 2;
	int offsy = wrec.bottom - wrec.top - crec.bottom;

	//SetWindowPos(hWndMain, HWND_TOP, 0, 0, Width + offsx+20, Height + offsy+20, SWP_NOMOVE);

	MainWindow.hWnd = hWndMain;
	MainWindow.hDc = GetDC(hWndMain);

	GameWindow.hWnd = hWndGame;
	GameWindow.hDc = GetDC(hWndGame);

	width = Width;
	height = Height;
	posx = PosX;
	posy = PosY;

	return true;
}

int Screen::InitializeOGL()
{
	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(GameWindow.hDc, &pfd);

	SetPixelFormat(GameWindow.hDc, pixelFormat, &pfd);
	wglMakeCurrent(GameWindow.hDc, wglCreateContext(GameWindow.hDc));
	wglGetCurrentContext();

	typedef BOOL(__stdcall* wglSwapIntervalEXT_Func)(int);
	wglSwapIntervalEXT_Func wglSwapIntervalEXT = wglSwapIntervalEXT_Func(wglGetProcAddress("wglSwapIntervalEXT"));
	if(wglSwapIntervalEXT) wglSwapIntervalEXT(0);

	GLenum glew_status = glewInit();
	if(GLEW_OK != glew_status)
	{
		printf("Error glewInit: %s\n", glewGetErrorString(glew_status));
		//system("pause");
		exit(0);
		return -1;
	}

	int majorv = 0;
	int minorv = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &majorv);
	glGetIntegerv(GL_MINOR_VERSION, &minorv);
	printf("VER:%d,%d\n", majorv, minorv);
	printf("RENDERER:%s\n", glGetString(GL_RENDERER));

	return 0;
}

#elif defined(LINUX_DESCTOP_SPACE)

bool Screen::CreateGameWindow(int PosX, int PosY, int Width, int Height)
{
	Display* display = XOpenDisplay(NULL);
	if(display == NULL) {
		printf("Не удалось установить соединение с графическим терминалом.\n");
		return 1;
	}

	/* Получим предварительные сведения */
	int screen = DefaultScreen(display);			// Экран по-умолчанию
	int root_win = RootWindow(display, screen); // Корневое окно
	int bgcolor = WhitePixel(display, screen);	// Белый цвет экрана

	/* Открываем окно */
	int window = XCreateSimpleWindow(display, root_win, 100, 100, 320, 200, 0, 0, bgcolor);

	/* Делаем окно видимым */
	XMapWindow(display, window);

	/* Все сформированные команды принудительно сбрасываем на сервер */
	XFlush(display);
	return true;
}

int Screen::InitializeOGL()
{
	return 0;
}

#endif

int Screen::SetupOGL()
{
	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	glClearColor(0.0f, 0.5f, 0.8f, 1.0f);

	glClearDepth(1.0f);

	glFrontFace(GL_CCW);

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	//glDepthFunc(GL_ALWAYS);

	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//Alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR_MATERIAL);
	glAlphaFunc(GL_GREATER, 0.01f);

	//glEnable(GL_ALPHA_TEST); 
	//glAlphaFunc(GL_GREATER,0.1f);
	/*
	glPointSize(10.0);
	//glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, @DistanceAttenuation);
	glPointParameterf( GL_POINT_FADE_THRESHOLD_SIZE,10.0);
	glPointParameterf( GL_POINT_SIZE_MIN,10.00);
	glPointParameterf( GL_POINT_SIZE_MAX,10.0);*/
	//glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	//glEnable(GL_POINT_SPRITE);
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//glViewport(0, 0, width, height);
	//glScissor(0, 0, width, height);
	
	/*float fog_color[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glEnable(GL_FOG);                       // �������� ����� (GL_FOG)
	glFogi(GL_FOG_MODE, GL_EXP2);// �������� ��� ������
	glFogfv(GL_FOG_COLOR, fog_color);        // ������������� ���� ������
	glFogf(GL_FOG_DENSITY, 0.35f);          // ��������� ������ ����� �����
	glHint(GL_FOG_HINT, GL_DONT_CARE);      // ��������������� ��������� ������
	glFogf(GL_FOG_START, 1.0f);             // �������, � ������� ���������� �����
	glFogf(GL_FOG_END, 5.0f);

	glEnable(GL_MULTISAMPLE);*/

	return 0;
}

void Screen::UpdateSystemEvents()
{
	MSG msg;

	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Screen::Clear()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Screen::Swap()
{
	return SwapBuffers(GameWindow.hDc);
}