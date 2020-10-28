#include "pch.h"
#include "App32.h"

#include <dinput.h>
#include "GameApplication.h"
#include <Windowsx.h>
#include <fstream>

const BYTE keyvalues[256] = { 0, DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z, DIK_SPACE, DIK_ESCAPE, DIK_RETURN, DIK_DOWN, DIK_UP, DIK_LEFT, DIK_RIGHT, DIK_TAB, DIK_BACKSPACE, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9, DIK_0, DIK_PERIOD };
const BYTE asciivalues[256] = { 0, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 32, 27, 13, 0, 0, 0, 0, 9, 8, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 46 };

bool keydown[256];
bool mousedown[4] = { false, false, false, false };
static GameApplication* application;

HINSTANCE hInstance;
HWND hwndMain;

LPDIRECTINPUT8 din;
LPDIRECTINPUTDEVICE8 dinkeyboard;
LPDIRECTINPUTDEVICE8 dinmouse;
DIMOUSESTATE mousestate;
BYTE keystate[256];
int windowWidth;
int windowHeight;
bool useFullScreen;
bool quitProgram = false;

const long long frameInterval = 12;
extern double performanceFrequency;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, int)
{
	hInstance = hInst;
#ifndef _DEBUG
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	const int winWidth = desktop.right;
	const int winHeight = desktop.bottom;
	windowWidth = winWidth;
	windowHeight = winHeight;
	CreateMainWindow(true, winWidth, winHeight);
#else
	CreateMainWindow(false, 1600, 900);
	windowWidth = 1600;
	windowHeight = 900;
#endif
	InitializeApplication();
	Run();
	return 0;
}

void InitializeApplication()
{
	_LARGE_INTEGER perfFreq;
	QueryPerformanceFrequency(&perfFreq);
	performanceFrequency = perfFreq.QuadPart / 1000.0;
	InitDInput();

	application = new GameApplication();
	application->Initialize();
	application->InitializeRenderer(hwndMain, useFullScreen, windowWidth, windowHeight);
}

void Run()
{
	MSG msg = { 0 };
	_LARGE_INTEGER frameStart;
	frameStart.QuadPart = 0LL;
	bool newFrame = true;

	while (WM_QUIT != msg.message)
	{
		if (newFrame)
		{
			newFrame = false;
			QueryPerformanceCounter(&frameStart);
		}

		if (PeekMessageA(&msg, hwndMain, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (quitProgram)
		{
			return;
		}

		HandleInput();

		_LARGE_INTEGER currentTick;
		QueryPerformanceCounter(&currentTick);
		const double elapsedTime = (currentTick.QuadPart - frameStart.QuadPart) / performanceFrequency;
		if (elapsedTime >= frameInterval)
		{
			newFrame = true;
			application->Update();
		}
		else
		{
			Sleep(1);
		}
	}
}

void HandleInput()
{
	static bool menuOpen = false;
	dinkeyboard->Acquire();
	dinkeyboard->GetDeviceState(256, (LPVOID) keystate);
	if (!menuOpen)
	{
		dinmouse->Acquire();
		dinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID) &mousestate);

		//Mouse move
		if (mousestate.lX != 0 || mousestate.lY != 0)
		{
			application->MouseMoved(mousestate.lX * -.01f, mousestate.lY * .01f);
		}

		//Mouse button
		if (!mousedown[0] && mousestate.rgbButtons[0] & 0x80)
		{
			mousedown[0] = true;
			application->MouseDown(1, windowWidth * .5f, windowHeight * .5f);
		}
		else if (mousedown[0] && !(mousestate.rgbButtons[0] & 0x80))
		{
			application->MouseUp(1);
			mousedown[0] = false;
		}

		if (!mousedown[1] && mousestate.rgbButtons[1] & 0x80)
		{
			mousedown[1] = true;
			application->MouseDown(2, windowWidth * .5f, windowHeight * .5f);
		}
		else if (mousedown[1] && !(mousestate.rgbButtons[1] & 0x80))
		{
			application->MouseUp(2);
			mousedown[1] = false;
		}
	}

	//Special keys
	if (keystate[DIK_F9] & 0x80)
	{
		CleanApplication();
	}
	if (!keydown[142] && keystate[DIK_F10] & 0x80)
	{
		menuOpen = !menuOpen;
		if (menuOpen)
		{
			dinmouse->Unacquire();
		}
		else
		{
			HRESULT hr;
			hr = dinmouse->Acquire();
		}

		keydown[142] = true;
		application->KeyPressed('P');
	}
	else if (keydown[142] && !(keystate[DIK_F10] & 0x80))
	{
		keydown[142] = false;
		application->KeyReleased('P');
	}
	if (!keydown[KEY_DOWN] && keystate[DIK_DOWN] & 0x80)
	{
		keydown[KEY_DOWN] = true;
		application->KeyPressed(KEY_DOWNARROW);
	}
	else if (keydown[KEY_DOWN] && !(keystate[DIK_DOWN] & 0x80))
	{
		keydown[KEY_DOWN] = false;
		application->KeyReleased(KEY_DOWNARROW);
	}
	if (!keydown[KEY_UP] && keystate[DIK_UP] & 0x80)
	{
		keydown[KEY_UP] = true;
		application->KeyPressed(KEY_UPARROW);
	}
	else if (keydown[KEY_UP] && !(keystate[DIK_UP] & 0x80))
	{
		keydown[KEY_UP] = false;
		application->KeyReleased(KEY_UPARROW);
	}
	if (!keydown[KEY_RETURN] && keystate[DIK_RETURN] & 0x80)
	{
		keydown[KEY_RETURN] = true;
		application->KeyPressed(KEY_ENTER);
	}
	else if (keydown[KEY_RETURN] && !(keystate[DIK_RETURN] & 0x80))
	{
		keydown[KEY_RETURN] = false;
		application->KeyReleased(KEY_ENTER);
	}
	if (!keydown[KEY_SPACE] && keystate[DIK_SPACE] & 0x80)
	{
		keydown[KEY_SPACE] = true;
		application->KeyPressed(KEY_SPACE);
	}
	else if (keydown[KEY_SPACE] && !(keystate[DIK_SPACE] & 0x80))
	{
		keydown[KEY_SPACE] = false;
		application->KeyReleased(KEY_SPACE);
	}
	else
	{
		//Alphabet keys
		for (int a = 1; a < 60; ++a)
		{
			if (!keydown[a] && keystate[keyvalues[a]] & 0x80)
			{
				keydown[a] = true;
				application->KeyPressed(asciivalues[a]);
			}
			else if (keydown[a] && !(keystate[keyvalues[a]] & 0x80))
			{
				keydown[a] = false;
				application->KeyReleased(asciivalues[a]);
			}
		}
	}
}

int CreateMainWindow(bool fullScreen, int width, int height)
{
	useFullScreen = fullScreen;
	windowWidth = width;
	windowHeight = height;
	DWORD   dwExStyle;
	DWORD   dwStyle;
	int bits = 32;
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (&MainWndProc);
	wc.hInstance = hInstance;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = L"MainWClass";

	RegisterClassEx(&wc);

	if (fullScreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = bits;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN != DISP_CHANGE_SUCCESSFUL))
		{
			fullScreen = false;
		}
	}
	if (fullScreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(TRUE);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	hwndMain = CreateWindowEx(dwExStyle, L"MainWClass", L"Main Window", dwStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, width, height, (HWND) NULL, (HMENU) NULL, hInstance, NULL);
	ShowWindow(hwndMain, SW_SHOWDEFAULT);
	UpdateWindow(hwndMain);
	SetForegroundWindow(hwndMain);
	SetFocus(hwndMain);
	return 0;
}

void InitDInput()
{
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &din, NULL);
	din->CreateDevice(GUID_SysKeyboard, &dinkeyboard, NULL);
	din->CreateDevice(GUID_SysMouse, &dinmouse, NULL);
	dinkeyboard->SetDataFormat(&c_dfDIKeyboard);
	dinmouse->SetDataFormat(&c_dfDIMouse);
	dinkeyboard->SetCooperativeLevel(hwndMain, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dinmouse->SetCooperativeLevel(hwndMain, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	for (int a = 0; a < 256; ++a)
	{
		keydown[a] = false;
	}
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		return 0;
	case WM_SIZE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		CleanApplication();
		PostQuitMessage(0);
		return 0;
		break;
	case WM_ENTERMENULOOP:
		dinmouse->Unacquire();
		return 0;
		break;
	case WM_LBUTTONDOWN:
	{
		int xLoc = GET_X_LPARAM(lParam);
		int yLoc = GET_Y_LPARAM(lParam);
		application->MouseDown(0, static_cast<float>(xLoc), static_cast<float>(yLoc));
		break;
	}

	case WM_RBUTTONDOWN:
	{
		int xLoc = GET_X_LPARAM(lParam);
		int yLoc = GET_Y_LPARAM(lParam);
		application->MouseDown(1, static_cast<float>(xLoc), static_cast<float>(yLoc));
		break;
	}
	case WM_LBUTTONUP:
		application->MouseUp(0);
		break;
	case WM_KEYDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

void CleanApplication()
{
	//delete application;
	dinmouse->Unacquire();
	dinmouse->Release();
	dinkeyboard->Unacquire();
	dinkeyboard->Release();
	din->Release();
	quitProgram = true;
	exit(0);
}