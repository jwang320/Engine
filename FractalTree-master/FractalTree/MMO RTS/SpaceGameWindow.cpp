#include "pch.h"
#include "SpaceGameWindow.h"
#include "SpaceGameApplication.h"
#include "..\Common Source\SimpleUI\UIContainer.h"
#include "..\Common Source\SimpleUI\UIButton.h"
#include "..\Common Source\SimpleUI\UIEmptyElement.h"

using namespace DirectX;
using namespace SimpleUI;
using namespace SpaceGameClient;
using namespace SpaceGameCore;

SpaceGameWindow::SpaceGameWindow(SpaceGameApplication* application) :
	application(application)
{
	this->initializeUI();
}

bool SpaceGameWindow::SetKeyDown(unsigned short whichKey, bool shiftDown)
{
	bool handled = __super::SetKeyDown(whichKey, shiftDown);

	if (!handled)
	{
		switch (whichKey)
		{
		case 'P':
		{
			if (this->GetForegroundUIContainer() != this->inEngineOverlayContainer)
			{
				this->SetForegroundUIElement(this->inEngineOverlayContainer);
			}
			else
			{
				this->SetForegroundUIElement(this->mainMenuUIContainer);
			}
		}
		break;
		default:
			break;
		}
	}

	return handled;
}

void SpaceGameWindow::initializeUI()
{
	this->initializeMainMenuUI();
	this->initializeInEngineOverlayUI();
	this->initializeNetworkConnectMenu();
	this->SetForegroundUIElement(this->inEngineOverlayContainer);
}

void SpaceGameWindow::initializeNetworkConnectMenu()
{

}
void SpaceGameWindow::initializeMainMenuUI()
{
	this->mainMenuUIContainer = new UIControlContainer(this);
	this->mainMenuUIContainer->SetBounds(XMVectorSet(.2f, .9f, 0, 0));
	this->mainMenuUIContainer->SetForegroundTint(XMVectorSet(0, 0, 1, 1));
	this->mainMenuUIContainer->SetPosition(XMVectorSet(.35f, 0, 0, 0));

	UIButton* const testControl = new UIButton(this);

	testControl->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		SpaceGameWindow* const thisWindow = dynamic_cast<SpaceGameWindow*>(owner);
		thisWindow->application->InitializeSimulation();
	});
	{
		UILabel startLabel;
		startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startLabel.text = "Enter Local Game";
		testControl->AddTextLabel(startLabel);
	}

	UIButton* const testControl2 = new UIButton(this);

	testControl2->SetOnUseCallback([](UIElement * callingElement, int activationMethod, Window * owner)
								  {
									  SpaceGameWindow* const thisWindow = dynamic_cast<SpaceGameWindow*>(owner);
									  thisWindow->application->disconnectFromServer();
								  });
	{
		UILabel startLabel;
		startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startLabel.text = "Disconnect";
		testControl2->AddTextLabel(startLabel);
	}

	UIButton* const networkButton = this->createEnterNetworkGameButton();
	this->accountNameButton = this->createAccountNameButton();
	this->passwordButton = this->createPasswordButton();
	this->mainMenuUIContainer->AddControlLayout(testControl);
	this->mainMenuUIContainer->AddControlLayout(this->accountNameButton);
	this->mainMenuUIContainer->AddControlLayout(this->passwordButton);
	this->mainMenuUIContainer->AddControlLayout(networkButton);
	this->mainMenuUIContainer->AddControlLayout(testControl2);
	this->mainMenuUIContainer->SetFocusElement(0);
}

using namespace std;
UIButton* SpaceGameWindow::createPasswordButton()
{
	UIButton* const passwordButton = new UIButton(this);
	passwordButton->SetEditable(true);
	passwordButton->AddTextLabel({ "Password: ", XMVectorSet(-.1f, -.01f, 0, 0) });
	passwordButton->SetEditableStartIndex(strlen("Password: "));
	passwordButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		SpaceGameWindow* const thisWindow = dynamic_cast<SpaceGameWindow*>(owner);
		const string text = callingElement->GetTextLabels()[0].text;
		const int pInd = callingElement->GetEditableStartIndex();
		thisWindow->password = text.substr(pInd);

	});

	return passwordButton;
}

UIButton* SpaceGameWindow::createAccountNameButton()
{
	UIButton* const accountNameButton = new UIButton(this);
	accountNameButton->SetEditable(true);
	accountNameButton->AddTextLabel({ "Account: ", XMVectorSet(-.1f, -.01f, 0, 0) });
	accountNameButton->SetEditableStartIndex(strlen("Account: "));
	accountNameButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		SpaceGameWindow* const thisWindow = dynamic_cast<SpaceGameWindow*>(owner);
		const string text = callingElement->GetTextLabels()[0].text;
		const int pInd = callingElement->GetEditableStartIndex();
		thisWindow->accountName = text.substr(pInd);

	});

	return accountNameButton;
}

UIButton* SpaceGameWindow::createEnterNetworkGameButton()
{
	UIButton* const testControl = new UIButton(this);

	testControl->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		SpaceGameWindow* const thisWindow = dynamic_cast<SpaceGameWindow*>(owner);

		const string accountName = thisWindow->accountNameButton->GetTextLabels()[0].text;
		const int accountInd = thisWindow->accountNameButton->GetEditableStartIndex();
		if (!accountName.substr(accountInd).empty())
		{
			thisWindow->accountName = accountName.substr(accountInd);
		}
			
		const string password = thisWindow->passwordButton->GetTextLabels()[0].text;
		const int passwordInd = thisWindow->passwordButton->GetEditableStartIndex();
		if (!password.substr(passwordInd).empty())
		{
			thisWindow->password = password.substr(passwordInd);
		}

		const int myId = thisWindow->application->ConnectToServer(thisWindow->accountName, thisWindow->password, thisWindow->ipAddress, 6112);

		if (myId > 0)
		{
			thisWindow->application->InitializeSimulationNetwork(myId);
		}
	});

	testControl->AddTextLabel({  "Enter Network Game", XMVectorSet(-.1f, -.01f, 0, 0) });
	return testControl;
}

void SpaceGameWindow::initializeInEngineOverlayUI()
{
	this->inEngineOverlayContainer = new UIEmptyElement(this);
}

SpaceGameWindow::~SpaceGameWindow()
{
	delete this->inEngineOverlayContainer;
	delete this->mainMenuUIContainer;
}