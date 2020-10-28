#pragma once
#include "..\Common Source\SimpleUI\Window.h"

namespace SimpleUI { class UIControlContainer; }
namespace SimpleUI { class UIElement; }
namespace SimpleUI { class UIButton; }
namespace SpaceGameCore {
	class SpaceGameSimulation;
}

namespace SpaceGameClient
{
	class SpaceGameApplication;

	class SpaceGameWindow : public SimpleUI::Window
	{
	private:
		SpaceGameClient::SpaceGameApplication* const application;
		SpaceGameCore::SpaceGameSimulation* simulation;
		SimpleUI::UIControlContainer* mainMenuUIContainer;
		SimpleUI::UIControlContainer* networkConnectUIContainer;
		SimpleUI::UIControlContainer* gameUI;
		SimpleUI::UIElement* inEngineOverlayContainer;
		std::string accountName = "vexal";
		std::string password = "wisconsinsucks";
		std::string ipAddress = "127.0.0.1";
		SimpleUI::UIButton* passwordButton;
		SimpleUI::UIButton* accountNameButton;

	public:
		SpaceGameWindow(SpaceGameClient::SpaceGameApplication* application);

		virtual bool SetKeyDown(unsigned short whichKey, bool shiftDown = false) override;
		void SetSimulation(SpaceGameCore::SpaceGameSimulation* simulation) { this->simulation = simulation; }
		~SpaceGameWindow();

	private:
		void initializeUI();
		void initializeNetworkConnectMenu();
		void initializeMainMenuUI();
		void initializeInEngineOverlayUI();

		SimpleUI::UIButton* createPasswordButton();
		SimpleUI::UIButton* createAccountNameButton();
		SimpleUI::UIButton* createEnterNetworkGameButton();
	};
}