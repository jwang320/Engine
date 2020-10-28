#pragma once
#include "GameSimulation.h"

class GameRenderer;
class GameMenuWindow;

namespace Win8GameRenderingEngine { class GameBindingGraph; }

class GameApplication
{
protected:
	//Effort underway to humanize the code.
	GameSimulation* gameSimulation;
	GameRenderer* renderer;
	GameMenuWindow* window;
	Win8GameRenderingEngine::GameBindingGraph* bindingGraph;
	int playerID;
	DirectX::XMVECTOR cameraVelocity;
	float cameraSpeed;
	DirectX::XMFLOAT4 cursorLocation;
	bool cursorActivated;
	DirectX::XMVECTOR cursorWorldLocation;
	bool hasReset;

public:
	GameApplication();
	void Initialize();
	void InitializeSimulation(const std::string& levelName, bool network = false, int worldType = 0, int clientID = 0);
	void Update();
#ifndef _WIN32_APPLICATION
	void InitializeRenderer(Windows::UI::Core::CoreWindow^ window);
#endif
#ifdef _WIN32_APPLICATION
	void InitializeRenderer(HWND hwnd, bool fullScreen, int width, int height);
#endif
	void KeyPressed(short key);
	void KeyReleased(short key);
	void MouseMoved(float dx, float dy);
	void MouseDown(short button, float dx, float dy);
	void MouseUp(short button);

	~GameApplication();

	static bool running;
	static volatile bool finishedRendering;

private:
	void updateRenderer();
	void updateCursorWorldLocation();

	static void RenderThread(GameApplication* thisApplication);
};