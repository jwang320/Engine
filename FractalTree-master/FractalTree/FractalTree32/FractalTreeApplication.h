#pragma once
namespace SimpleUI {class FractalTreeWindow;}
namespace SimulationEngine {class FractalTreeSimulation;}
namespace RenderingEngine {class FractalTreeRenderer; class ActorRendererBindingGraph;}

//This class should be instantiated by the main() method.
class FractalTreeApplication
{
protected:
	SimpleUI::FractalTreeWindow* window;
	SimulationEngine::FractalTreeSimulation* simulation = nullptr;
	RenderingEngine::FractalTreeRenderer* renderer2 = nullptr;
	RenderingEngine::ActorRendererBindingGraph* bindingGraph = nullptr;
	int playerID = -1;

private:
	int perlinAmpInd;
	int perlinFreqInd;
	int treeUpdateInd;
	int yRotInd;
	int itCountInd;
	std::string accountName = "default";

public:
	FractalTreeApplication();
	void Initialize();
	void InitializeRenderer(HWND hwnd, bool fullScreen, int width, int height);
	void Update();
	void InitializeSimulation(bool network = false, int worldType = 0, int clientID = -1);
	DirectX::XMVECTOR GetFloorProjection(float screenX, float screenY);
	void SetAccountName(const std::string& accountName) { this->accountName = accountName; }
	std::string GetAccountName() const { return this->accountName; }

	void KeyPressed(short key);
	void KeyReleased(short key);
	void MouseMoved(float dx, float dy);
	void MouseDown(short button, float dx, float dy);
	void MouseUp(short button);
	virtual ~FractalTreeApplication();

private:
	void updateRenderer();

	void requestPlayerSpawn();
	void sendCommand(const SimulationEngine::Command& command, bool alwaysLocal = false);
	static void RenderThread(FractalTreeApplication* thisApplication);

	void initializeDebugStrings();
};