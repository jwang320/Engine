#pragma once
#include "..\Common Source\SimpleUI\Window.h"

class FractalTreeApplication;
namespace SimulationEngine {class FractalTreeSimulation;}

namespace SimpleUI
{
	class UIMapEditor;
	class UIControlContainer;

	class FractalTreeWindow : public Window
	{
	protected:
		FractalTreeApplication* application;

	private:
		UIControlContainer* mainMenuUIContainer;
		UIControlContainer* lSystemParameterContainer;
		UIControlContainer* treeParameterContainer;
		UIMapEditor* mapConfigurationPane;
		UIElement* inEngineOverlayContainer;

	public:
		SimulationEngine::FractalTreeSimulation* simulation;
		FractalTreeWindow(FractalTreeApplication* application);
		virtual bool SetKeyDown(unsigned short whichKey, bool shiftDown = false) override;
		void SetSimulation(SimulationEngine::FractalTreeSimulation* simulation) {this->simulation = simulation;}
		void Update();
		void InitializeUI();
		~FractalTreeWindow();

	private:
		void InitializeMainMenuUI();
		void InitializeInEngineOverlayUI();
		void InitializeLSystemUI();
		void InitializeMapPane();

	};
}