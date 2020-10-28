#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
typedef HWND SystemWindowType;

namespace SpaceGameCore {
	class SpaceGameSimulation;
	class SpaceGameRenderer;
	class SpaceGameBindingGraph;
	class SpaceGameUDPClient;
}
namespace SpaceGameClient
{
	class SpaceGameWindow;

	class SpaceGameApplication
	{
	private:
		SpaceGameCore::SpaceGameSimulation* simulation = nullptr;
		SpaceGameCore::SpaceGameRenderer* renderer = nullptr;
		SpaceGameCore::SpaceGameBindingGraph* bindingGraph = nullptr;
		SpaceGameClient::SpaceGameWindow* window = nullptr;
		SpaceGameCore::SpaceGameUDPClient* networkClient = nullptr;

		int playerID;
		bool useNetwork = false;
		bool active = false;
	public:
		SpaceGameApplication();
		void Initialize();
		void InitializeRenderer(HWND hwnd, bool fullScreen, int width, int height);
		void InitializeSimulation();
		void InitializeSimulationNetwork(const int clientId);
		void updateRenderer();
		int ConnectToServer(const std::string& accountName, const std::string& password, const std::string& ipAddress, short port);
		void disconnectFromServer();
		void Update();

		void KeyPressed(short key, bool shiftDown = false);
		void sendCommand(const SimulationEngine::Command& command, const bool alwaysLocal = false);
		void KeyReleased(short key);
		void MouseMoved(float dx, float dy);
		DirectX::XMVECTOR GetFloorProjection(float screenX, float screenY);
		void MouseDown(short button, float dx, float dy);
		void MouseUp(short button);

		static void RenderThread(SpaceGameApplication* thisApplication);
	private:
		void initializeDebugStrings();
		void handleNetwork();
		int getFocusObject();
		void quitSimulation();
	};
}