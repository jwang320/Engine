#pragma once
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, int);
void HandleInput();
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CreateMainWindow(bool fullScren = true, int width = 1600, int height = 900);
void InitializeApplication();
void InitDInput();
void InitializeServer();
void Run();
void ReceiveNetworkInput();
float GetNewZCurve(float xAcceleration, float yAcceleration, float zAcceleration = 0);
void CleanApplication();