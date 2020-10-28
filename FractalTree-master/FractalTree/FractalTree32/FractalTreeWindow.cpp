#include "pch.h"
#include <string>
#include "FractalTreeWindow.h"
#include "FractalTreeApplication.h"
#include "UIMapEditor.h"
#include <sstream>
#include "..\Common Source\LSystem\SystemParameters.h"
#include "..\FractalTree Common\FractalTreeSimulation.h"
#include "..\Common Source\SimpleUI\UIButton.h"
#include "..\Common Source\SimpleUI\UITextOutputBox.h"
#include "..\Common Source\SimpleUI\UIToggleButton.h"
#include "..\Common Source\SimpleUI\UIEmptyElement.h"

using namespace RenderingEngine;
using namespace DirectX;

namespace SimpleUI
{
	void FractalTreeWindow::InitializeMainMenuUI()
	{
		this->mainMenuUIContainer = new SimpleUI::UIControlContainer(this);
		this->mainMenuUIContainer->SetBounds(XMVectorSet(.2f, .9f, 0, 0));
		this->mainMenuUIContainer->SetForegroundTint(XMVectorSet(0, 0, 1, 1));
		this->mainMenuUIContainer->SetPosition(XMVectorSet(.35f, 0, 0, 0));

		SimpleUI::UIButton* testControl = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* resetWorldButton = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* quitButton = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* sandboxButton = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* configureRobotButton = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* connectToNetworkButton = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* accountNameButton = new SimpleUI::UIButton(this);
		SimpleUI::UIButton* yunsikButton = new SimpleUI::UIButton(this);

		SimpleUI::UITextOutputBox* consoleWindow = new SimpleUI::UITextOutputBox(this);

		testControl->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
			thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer);
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
			startLabel.text = "LSystem Parameters";
			testControl->AddTextLabel(startLabel);
		}
		resetWorldButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				thisWindow->simulation->ClearWorld();
		});

		quitButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* window = static_cast<FractalTreeWindow*>(owner);
			/*window->simulation = new SimulationEngine::FractalTreeSimulation();
			window->application->InitializeSimulation(window->simulation);
			window->simulation->EnterWorld();*/
			window->application->InitializeSimulation(false);
		});

		configureRobotButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
			thisWindow->SetForegroundUIElement(thisWindow->mapConfigurationPane);
		});

		sandboxButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* window = static_cast<FractalTreeWindow*>(owner);
			window->application->InitializeSimulation(false, 1);
		});

		yunsikButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* window = static_cast<FractalTreeWindow*>(owner);
			window->application->InitializeSimulation(false, 2);
		});

		accountNameButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* window = static_cast<FractalTreeWindow*>(owner);
			window->application->SetAccountName(callingElement->GetTextLabels()[0].text);
			WCOUT << "Account name set to: " << callingElement->GetTextLabels()[0].text << SimpleUI::endl;
		});

		UILabel startLabel;
		startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startLabel.text = "Enter World";
		quitButton->AddTextLabel(startLabel);

		UILabel accountNameLabel;
		accountNameLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		accountNameLabel.text = this->application->GetAccountName();
		accountNameButton->AddTextLabel(accountNameLabel);
		accountNameButton->SetEditable(true);

		UILabel startShipLabel;
		startShipLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startShipLabel.text = "Configure Robot";
		configureRobotButton->AddTextLabel(startShipLabel);

		UILabel bu;
		bu.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		bu.text = "Enter Blank World";
		sandboxButton->AddTextLabel(bu);

		UILabel bu2;
		bu2.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		bu2.text = "Yunsik Press This";
		yunsikButton->AddTextLabel(bu2);

		UILabel serverLabel;
		serverLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		serverLabel.text = "127.0.0.1";
		connectToNetworkButton->AddTextLabel(serverLabel);
		connectToNetworkButton->SetEditable(true);

		consoleWindow->SetBounds(XMVectorSet(0, 0, 0, 1));
		consoleWindow->SetPosition(XMVectorSet(-.5f, .90f, 0, 0));
		FractalTreeResources::ConsoleOutput = consoleWindow;
		//consoleWindow->Set
		this->mainMenuUIContainer->AddControlLayout(testControl);
		this->mainMenuUIContainer->AddControlLayout(resetWorldButton);
		this->mainMenuUIContainer->AddControlLayout(quitButton);
		this->mainMenuUIContainer->AddControlLayout(configureRobotButton);
		this->mainMenuUIContainer->AddControlLayout(sandboxButton);
		this->mainMenuUIContainer->AddControlLayout(connectToNetworkButton);
		this->mainMenuUIContainer->AddControlLayout(accountNameButton);
		this->mainMenuUIContainer->AddControlLayout(yunsikButton);
		this->mainMenuUIContainer->SetFocusElement(0);
		this->mainMenuUIContainer->AddChild(consoleWindow);
	}

	void FractalTreeWindow::InitializeMapPane()
	{
		const float aspectRatio = (float)this->GetHeight() / this->GetWidth();
		UIContainerLayout layout;
		this->mapConfigurationPane = new UIMapEditor(this, layout);
		//UIMapEditor* imagePane = *///new UIMapEditor(&FractalTreeResources::generatorDefinition, this, this->mainMenuUIContainer);
		mapConfigurationPane->SetBounds(XMVectorSet(.9f * aspectRatio, .9f, 0, 0));
		mapConfigurationPane->SetForegroundTint(XMVectorSet(1, 0, 1, 1));
		mapConfigurationPane->SetPosition(XMVectorSet(.18f, 0, 0, 0));
	}

	void FractalTreeWindow::InitializeLSystemUI()
	{
		UIContainerLayout layout;
		layout.basePosition = XMVectorSet(0, .97f, 0, 0);
		layout.elementSize = XMVectorSet(.8f, .04f, 0, 0);
		layout.offsetMagnitude = .05f;
		this->lSystemParameterContainer = new UIControlContainer(this, layout);
		this->lSystemParameterContainer->SetBounds(XMVectorSet(.4f, 1.8f, 0, 0));
		this->lSystemParameterContainer->SetForegroundTint(XMVectorSet(0, 0, 1, 1));
		this->lSystemParameterContainer->SetPosition(XMVectorSet(.7f, 0, 0, 0));

		UIToggleButton* testControl = new UIToggleButton(this);
		testControl->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			SystemParameters::AllowBranching = !SystemParameters::AllowBranching;
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Toggle Branching";
			testControl->AddTextLabel(startLabel);
		}
		//testControl->SetBounds(XMVectorMultiply(testControl->GetBounds(), XMVectorSet(1, .5, 1, 1)));

		UIToggleButton* bodyVariationButton = new UIToggleButton(this);
		bodyVariationButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			SystemParameters::UseTrunkBodyVariance = !SystemParameters::UseTrunkBodyVariance;
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Use Random Width";
			bodyVariationButton->AddTextLabel(startLabel);
		}

		UIToggleButton* useSmoothNormalButton = new UIToggleButton(this);
		useSmoothNormalButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			SystemParameters::UseFaceNormals = !SystemParameters::UseFaceNormals;
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Use Vertex Normals";
			useSmoothNormalButton->AddTextLabel(startLabel);
		}

		UIToggleButton* burnTreesButton = new UIToggleButton(this);
		burnTreesButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			//SystemParameters::UseFaceNormals = !SystemParameters::UseFaceNormals;
			FractalTreeResources::burnTrees = !FractalTreeResources::burnTrees;
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Burn Trees";
			burnTreesButton->AddTextLabel(startLabel);
		}

		UIToggleButton* isTreeButton = new UIToggleButton(this);
		isTreeButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			//SystemParameters::UseFaceNormals = !SystemParameters::UseFaceNormals;
			SystemParameters::IsTree = !SystemParameters::IsTree;
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Is Tree";
			isTreeButton->AddTextLabel(startLabel);
		}

		UIButton* backButton = new UIButton(this);
		backButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
		{
			FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
			thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
		});
		{
			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Back";
			backButton->AddTextLabel(startLabel);
		}

		UIButton* radiusStartButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Radius Start: " + std::to_string(SystemParameters::TrunkStartRadius);
			radiusStartButton->AddTextLabel(startLabel);
			radiusStartButton->SetEditable(true);
			radiusStartButton->SetEditableStartIndex(strlen("Radius Start: "));

			radiusStartButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float radiusStart;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> radiusStart;

				SystemParameters::TrunkStartRadius = radiusStart;
				
			});
		}

		UIButton* lengthStartButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Length Start: " + std::to_string(SystemParameters::TrunkStartLength);
			lengthStartButton->AddTextLabel(startLabel);
			lengthStartButton->SetEditable(true);
			lengthStartButton->SetEditableStartIndex(strlen("Length Start: "));

			lengthStartButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float radiusStart;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> radiusStart;

				SystemParameters::TrunkStartLength = radiusStart;

			});
		}

		UIButton* minBranchButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Count Min: " + std::to_string(SystemParameters::MinimumBranchCount);
			minBranchButton->AddTextLabel(startLabel);
			minBranchButton->SetEditable(true);
			minBranchButton->SetEditableStartIndex(strlen("Branch Count Min: "));

			minBranchButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				int minBranchCount;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> minBranchCount;

				SystemParameters::MinimumBranchCount = minBranchCount;

			});
		}

		UIButton* maxBranchButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Count Max: " + std::to_string(SystemParameters::MaximumBranchCount);
			maxBranchButton->AddTextLabel(startLabel);
			maxBranchButton->SetEditable(true);
			maxBranchButton->SetEditableStartIndex(strlen("Branch Count Max: "));

			maxBranchButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				int maxBranchCount;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> maxBranchCount;

				SystemParameters::MaximumBranchCount = maxBranchCount;

			});
		}

		UIButton* zBranchMin = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Z Min: " + std::to_string(SystemParameters::BranchZMin);
			zBranchMin->AddTextLabel(startLabel);
			zBranchMin->SetEditable(true);
			zBranchMin->SetEditableStartIndex(strlen("Branch Z Min: "));

			zBranchMin->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::BranchZMin = branchZMin;

			});
		}

		UIButton* zBranchMax = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Z Max: " + std::to_string(SystemParameters::BranchZMax);
			zBranchMax->AddTextLabel(startLabel);
			zBranchMax->SetEditable(true);
			zBranchMax->SetEditableStartIndex(strlen("Branch Z Max: "));

			zBranchMax->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::BranchZMax = branchZMin;

			});
		}

		UIButton* continueTrunkChanceButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Trunk Continue Chance: " + std::to_string(SystemParameters::ContinueTrunkChance);
			continueTrunkChanceButton->AddTextLabel(startLabel);
			continueTrunkChanceButton->SetEditable(true);
			continueTrunkChanceButton->SetEditableStartIndex(strlen("Branch Trunk Continue Chance: "));

			continueTrunkChanceButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				int branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::ContinueTrunkChance = branchZMin;

			});
		}

		UIButton* minimumYSeparationButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Y Separation Min: " + std::to_string(SystemParameters::MinimumYSeparation);
			minimumYSeparationButton->AddTextLabel(startLabel);
			minimumYSeparationButton->SetEditable(true);
			minimumYSeparationButton->SetEditableStartIndex(strlen("Branch Y Separation Min: "));

			minimumYSeparationButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::MinimumYSeparation = branchZMin;

			});
		}

		UIButton* maximumYSeparationButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Y Separation Max: " + std::to_string(SystemParameters::MaximumYSeparation);
			maximumYSeparationButton->AddTextLabel(startLabel);
			maximumYSeparationButton->SetEditable(true);
			maximumYSeparationButton->SetEditableStartIndex(strlen("Branch Y Separation Max: "));

			maximumYSeparationButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::MaximumYSeparation = branchZMin;

			});
		}

		UIButton* radiusFactorButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Radius Factor: " + std::to_string(SystemParameters::radiusDeltaFactorMin);
			radiusFactorButton->AddTextLabel(startLabel);
			radiusFactorButton->SetEditable(true);
			radiusFactorButton->SetEditableStartIndex(strlen("Trunk Radius Factor: "));

			radiusFactorButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::radiusDeltaFactorMin = branchZMin;

			});
		}

		UIButton* radiusFactorRangeButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Radius Factor Range: " + std::to_string(SystemParameters::radiusFactorRange);
			radiusFactorRangeButton->AddTextLabel(startLabel);
			radiusFactorRangeButton->SetEditable(true);
			radiusFactorRangeButton->SetEditableStartIndex(strlen("Trunk Radius Factor Range: "));

			radiusFactorRangeButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::radiusFactorRange = branchZMin;

			});
		}

		UIButton* branchRadiusFactorButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Branch Radius Factor: " + std::to_string(SystemParameters::BranchRadiusFactor);
			branchRadiusFactorButton->AddTextLabel(startLabel);
			branchRadiusFactorButton->SetEditable(true);
			branchRadiusFactorButton->SetEditableStartIndex(strlen("Branch Radius Factor: "));

			branchRadiusFactorButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::BranchRadiusFactor = branchZMin;

			});
		}

		UIButton* lengthFactorButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Length Factor: " + std::to_string(SystemParameters::lengthFactorMin);
			lengthFactorButton->AddTextLabel(startLabel);
			lengthFactorButton->SetEditable(true);
			lengthFactorButton->SetEditableStartIndex(strlen("Trunk Length Factor: "));

			lengthFactorButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::lengthFactorMin = branchZMin;

			});
		}

		UIButton* lengthFactorRangeButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Length Factor Rng: " + std::to_string(SystemParameters::lengthFactorRange);
			lengthFactorRangeButton->AddTextLabel(startLabel);
			lengthFactorRangeButton->SetEditable(true);
			lengthFactorRangeButton->SetEditableStartIndex(strlen("Trunk Length Factor Rng: "));

			lengthFactorRangeButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::lengthFactorRange = branchZMin;

			});
		}

		UIButton* trunkZMin = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Z Rot Min: " + std::to_string(SystemParameters::zRotationMin);
			trunkZMin->AddTextLabel(startLabel);
			trunkZMin->SetEditable(true);
			trunkZMin->SetEditableStartIndex(strlen("Trunk Z Rot Min: "));

			trunkZMin->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::zRotationMin = branchZMin;

			});
		}

		UIButton* trunkZMax = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Z Rot Max: " + std::to_string(SystemParameters::zRotationMax);
			trunkZMax->AddTextLabel(startLabel);
			trunkZMax->SetEditable(true);
			trunkZMax->SetEditableStartIndex(strlen("Trunk Z Rot Max: "));

			trunkZMax->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::zRotationMax = branchZMin;

			});
		}

		UIButton* trunkYMin = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Y Rot Min: " + std::to_string(SystemParameters::yRotationMin);
			trunkYMin->AddTextLabel(startLabel);
			trunkYMin->SetEditable(true);
			trunkYMin->SetEditableStartIndex(strlen("Trunk Y Rot Min: "));

			trunkYMin->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::yRotationMin = branchZMin;

			});
		}

		UIButton* trunkYMax = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Y Rot Max: " + std::to_string(SystemParameters::yRotationMax);
			trunkYMax->AddTextLabel(startLabel);
			trunkYMax->SetEditable(true);
			trunkYMax->SetEditableStartIndex(strlen("Trunk Y Rot Min: "));

			trunkYMax->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				float branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::yRotationMax = branchZMin;

			});
		}

		UIButton* trunkSegmentCountButton = new UIButton(this);
		{

			UILabel startLabel;
			startLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
			startLabel.text = "Trunk Segment Count: " + std::to_string(SystemParameters::trunkSegmentCount);
			trunkSegmentCountButton->AddTextLabel(startLabel);
			trunkSegmentCountButton->SetEditable(true);
			trunkSegmentCountButton->SetEditableStartIndex(strlen("Trunk Segment Count: "));

			trunkSegmentCountButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
			{
				FractalTreeWindow* thisWindow = dynamic_cast<FractalTreeWindow*>(owner);
				UIButton* button = static_cast<UIButton*>(callingElement);
				//thisWindow->SetForegroundUIElement(thisWindow->lSystemParameterContainer->GetParent());
				int branchZMin;
				std::string radiusString = button->GetTextLabels()[0].text.substr(button->GetEditableStartIndex());
				std::stringstream strm(radiusString);
				strm >> branchZMin;

				SystemParameters::trunkSegmentCount = branchZMin;

			});
		}

		//this->lSystemParameterContainer->AddControlLayout(testControl);
		this->lSystemParameterContainer->AddControlLayout(bodyVariationButton);
		this->lSystemParameterContainer->AddControlLayout(useSmoothNormalButton);
		this->lSystemParameterContainer->AddControlLayout(isTreeButton);
		//this->lSystemParameterContainer->AddControlLayout(backButton);
		this->lSystemParameterContainer->AddControlLayout(radiusStartButton);
		this->lSystemParameterContainer->AddControlLayout(lengthStartButton);
		this->lSystemParameterContainer->AddControlLayout(minBranchButton);
		this->lSystemParameterContainer->AddControlLayout(maxBranchButton);
		this->lSystemParameterContainer->AddControlLayout(zBranchMin);
		this->lSystemParameterContainer->AddControlLayout(zBranchMax);
		this->lSystemParameterContainer->AddControlLayout(continueTrunkChanceButton);
		this->lSystemParameterContainer->AddControlLayout(minimumYSeparationButton);
		this->lSystemParameterContainer->AddControlLayout(maximumYSeparationButton);
		this->lSystemParameterContainer->AddControlLayout(branchRadiusFactorButton);
		this->lSystemParameterContainer->AddControlLayout(radiusFactorButton);
		this->lSystemParameterContainer->AddControlLayout(radiusFactorRangeButton);
		this->lSystemParameterContainer->AddControlLayout(lengthFactorButton);
		this->lSystemParameterContainer->AddControlLayout(lengthFactorRangeButton);
		this->lSystemParameterContainer->AddControlLayout(trunkSegmentCountButton);
		this->lSystemParameterContainer->AddControlLayout(trunkZMin);
		this->lSystemParameterContainer->AddControlLayout(trunkZMax);
		this->lSystemParameterContainer->AddControlLayout(trunkYMin);
		this->lSystemParameterContainer->AddControlLayout(trunkYMax);









		testControl->SetToggled(SystemParameters::AllowBranching);
		useSmoothNormalButton->SetToggled(!SystemParameters::UseFaceNormals);
		bodyVariationButton->SetToggled(SystemParameters::UseTrunkBodyVariance);
		isTreeButton->SetToggled(SystemParameters::IsTree);
		this->lSystemParameterContainer->SetFocusElement(0);


	}

	void FractalTreeWindow::InitializeInEngineOverlayUI()
	{
		if (!FractalTreeResources::showCrosshair)
		{
			this->inEngineOverlayContainer = new SimpleUI::UIControlContainer(this);
			this->inEngineOverlayContainer->SetBounds(XMVectorSet(.01f, .01f, 0, 0));
			this->inEngineOverlayContainer->SetForegroundTint(XMVectorSet(0, 0, 1, 1));
			this->inEngineOverlayContainer->SetPosition(XMVectorSet(0, 0, 0, 0));
			this->inEngineOverlayContainer->SetEnabled(true);
			this->inEngineOverlayContainer->SetVisible(true);
		}
		else 
		{
			this->inEngineOverlayContainer = new SimpleUI::UIEmptyElement(this);
		}
	}

	void FractalTreeWindow::Update()
	{
		UIButton* el = static_cast<UIButton*>(this->mainMenuUIContainer->GetChildren()[0]);
		//el->GetTextLabels()[0].appendText = std::to_string((long double)SystemParameters::CurveAmount);
	}
	void FractalTreeWindow::InitializeUI()
	{
		this->InitializeMainMenuUI();
		this->InitializeInEngineOverlayUI();
		this->InitializeLSystemUI();
		this->InitializeMapPane();
		this->SetForegroundUIElement(this->inEngineOverlayContainer);
		this->lSystemParameterContainer->SetParent(this->mainMenuUIContainer);
	}

	FractalTreeWindow::FractalTreeWindow(FractalTreeApplication* application) :
		Window(),
		application(application)
	{

	}

	bool FractalTreeWindow::SetKeyDown(unsigned short whichKey, bool shiftDown)
	{
#ifndef _NO_METRO
		using namespace Windows::System;
#endif
		const bool handled = __super::SetKeyDown(whichKey, shiftDown);

		if(!handled)
		{
			switch (whichKey)
			{
			case 'P':
				{
					if(this->GetForegroundUIContainer() != this->inEngineOverlayContainer)
					{
						this->SetForegroundUIElement(this->inEngineOverlayContainer);
					}
					else
					{
						this->SetForegroundUIElement(this->mainMenuUIContainer);
					}
				}
				break;
#ifndef _NO_METRO
			case VirtualKey::Escape:
				{
					if(this->GetForegroundUIContainer()->GetParent() != NULL)
					{
						this->SetForegroundUIElement(this->GetForegroundUIContainer()->GetParent());
					}
				}
				break;
#endif
			default:
				break;
			}
		}

		return handled;
	}

	FractalTreeWindow::~FractalTreeWindow()
	{
		delete this->inEngineOverlayContainer;
		delete this->mainMenuUIContainer;
	}
}