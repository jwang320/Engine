#ifdef _WIN32_APPLICATION
#pragma once


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, int);
void HandleInput();
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int CreateMainWindow(bool fullScren = true, int width = 1600, int height = 900);
void InitializeApplication();
void InitDInput();
void Run();
void CleanApplication();
#endif
