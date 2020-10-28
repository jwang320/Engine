#pragma once

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, int);
void Run();
void HandleInput();
int CreateMainWindow(bool fullScren = true, int width = 1600, int height = 900);
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeApplication();
void InitDInput();
void CleanApplication();