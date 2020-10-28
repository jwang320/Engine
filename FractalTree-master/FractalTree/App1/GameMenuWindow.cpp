#include "pch.h"
#include "GameMenuWindow.h"

#include "..\SimpleUI\UIContainer.h"
#include "..\SimpleUI\UIEmptyElement.h"
#include "..\SimpleUI\UIButton.h"
#include "UIImage.h"
#include "GameApplication.h"
#include "PlatformerCharacter.h"
#include "GameResources.h"

using namespace SimpleUI;
using namespace DirectX;
using namespace std;

GameMenuWindow::GameMenuWindow(GameApplication* application) :
	application(application),
	playerCharacterID(-1),
	currentCharacterFrame(0),
	selectedBlock(NULL),
	selectedBlockInt(0)
{
	this->inEngineOverlayContainer = new UIEmptyElement(this);

	this->mainMenuUIContainer = new SimpleUI::UIControlContainer(this);
	this->mainMenuUIContainer->SetBounds(XMVectorSet(.4f, 1.8f, 0, 0));
	this->mainMenuUIContainer->SetForegroundTint(XMVectorSet(0, 0, 1, 1));
	this->mainMenuUIContainer->SetPosition(XMVectorSet(.7f, 0, 0, 0));

	UIButton* enterWorldButton = new UIButton(this, this->mainMenuUIContainer);
	UIButton* enterNewGameButton = new UIButton(this, this->mainMenuUIContainer);
	UIButton* testControl = new UIButton(this, this->mainMenuUIContainer);

	enterWorldButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		GameMenuWindow* thisWindow = dynamic_cast<GameMenuWindow*>(owner);
		thisWindow->application->InitializeSimulation("TestWorld.gd");
	});
	{
		UILabel startLabel;
		startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startLabel.text = "Enter Game World";
		enterWorldButton->AddTextLabel(startLabel);
	}

	enterNewGameButton->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		GameMenuWindow* thisWindow = dynamic_cast<GameMenuWindow*>(owner);
		thisWindow->application->InitializeSimulation("Level0.gd");
	});
	{
		UILabel startLabel;
		startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startLabel.text = "Enter New Game";
		enterNewGameButton->AddTextLabel(startLabel);
	}

	testControl->SetOnUseCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		GameMenuWindow* thisWindow = dynamic_cast<GameMenuWindow*>(owner);
		exit(0);
	});
	{
		UILabel startLabel;
		startLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
		startLabel.text = "Quit";
		testControl->AddTextLabel(startLabel);
	}

	this->mainMenuUIContainer->AddControlLayout(enterWorldButton);
	this->mainMenuUIContainer->AddControlLayout(enterNewGameButton);
	this->mainMenuUIContainer->AddControlLayout(testControl);
	this->mainMenuUIContainer->SetFocusElement(0);

	
	this->SetForegroundUIElement(this->inEngineOverlayContainer);

	this->initializeInventoryUI();
}

void GameMenuWindow::initializeInventoryUI()
{
	this->currentCharacterFrame = 0;
	this->inventoryUIContainer = new UIControlContainer(this);
	this->inventoryUIContainer->SetBounds(XMVectorSet(1, 1, 0, 0));
	this->inventoryUIContainer->SetAutoPassClick(true);
	//this->inventoryUIContainer->SetPosition(XMVectorSet(.45f, .45f, 0, 0));
	this->cursorImage = new UIImage(this, this->inventoryUIContainer);
	this->cursorImage->SetPosition(XMVectorSet(0, 0, 0, 0));
	this->cursorImage->SetBounds(XMVectorSet(.03f, .03f * 16.f / 10.f, 0, 0));
	this->cursorImage->SetForegroundTint(XMVectorSet(1, 1, 1, .6f));
	this->cursorImage->SetUseRelativeScale(false);
	this->cursorImage->SetVisible(true);
	this->inventoryUIContainer->AddChild(this->cursorImage);

	UIControlContainer* uiBackPane = new UIControlContainer(this, UIContainerLayout(), this->inventoryUIContainer);
	//uiBackPane->SetPosition(XMVectorSet(.45f, .45f, 0, 0));
	//uiBackPane->SetBounds(XMVectorSet(.9f, .9f, 0, 0));
	uiBackPane->SetPosition(XMVectorSet(.5f, .5f, 0, 0));
	uiBackPane->SetBounds(XMVectorSet(1, 1, 0, 0));
	uiBackPane->SetForegroundTint(XMVectorSet(1, 1, 1, .6f));
	uiBackPane->SetUseRelativeScale(false);
	uiBackPane->SetUseRelativePosition(false);
	uiBackPane->SetVisible(true);
	uiBackPane->SetAutoPassClick(true);
	uiBackPane->SetOnFocusChangeCallback([](UIElement* callingElement, int activationMethod, Window* owner)
	{
		GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);
		const UIControlContainer* callingContainer = static_cast<UIControlContainer*>(callingElement);
		callingWindow->UpdateSelectedBlockType(callingContainer->GetFocusElementIndex());
	});

	this->inventoryUIContainer->AddChild(uiBackPane, "uiBackPane");
	this->inventoryUIContainer->SetPassThroughInputElement(uiBackPane);

	UIImage* blockCountButton = new UIImage(this, uiBackPane);
	blockCountButton->SetUseRelativeScale(false);
	blockCountButton->SetUseRelativePosition(true);
	blockCountButton->SetPosition(XMVectorSet(-.8f, -.1f, 0, 0));
	blockCountButton->SetBounds(XMVectorSet(.3f, .15f, 0, 0));
	blockCountButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
	blockCountButton->SetVisible(true);
	UILabel blockCountLabel;
	blockCountLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
	blockCountLabel.text = "Total Block Count: ";
	blockCountButton->AddTextLabel(blockCountLabel);
	uiBackPane->AddChild(blockCountButton, "blockCountButton");
	uiBackPane->SetFocusElement(0);

	UIImage* currentFrameButton = new UIImage(this, uiBackPane);
	currentFrameButton->SetUseRelativeScale(false);
	currentFrameButton->SetUseRelativePosition(false);
	currentFrameButton->SetFocusable(false);
	currentFrameButton->SetUseCompoundTransformation(false);
	currentFrameButton->SetPosition(XMVectorSet(.8f, .1f, 0, 0));
	currentFrameButton->SetBounds(XMVectorSet(.3f, .075f, 0, 0));
	currentFrameButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
	currentFrameButton->SetVisible(true);

	UILabel frameCountLabel;
	frameCountLabel.offset = XMVectorSet(-.1f, -.025f, 0, 0);
	frameCountLabel.text = "Current Frame: 0";
	currentFrameButton->AddTextLabel(frameCountLabel);
	uiBackPane->AddChild(currentFrameButton, "frameCount");
	uiBackPane->SetFocusElement(0);
	this->currentFrameImage = currentFrameButton;

	UIImage* nextFrameButton = new UIImage(this, uiBackPane);
	nextFrameButton->SetUseRelativeScale(false);
	nextFrameButton->SetUseRelativePosition(false);
	nextFrameButton->SetClickable(true);
	nextFrameButton->SetFocusable(false);
	nextFrameButton->SetUseCompoundTransformation(false);
	nextFrameButton->SetPosition(XMVectorSet(.5f, .1f, 0, 0));
	nextFrameButton->SetBounds(XMVectorSet(.15f, .075f, 0, 0));
	nextFrameButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
	nextFrameButton->SetVisible(true);
	nextFrameButton->SetOnClickCallback([](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
	{
		GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);		
		callingWindow->IncrementCurrentCharacterFrame();
		callingWindow->GetCurrentFrameButton()->GetTextLabels()[0].text = "Current Frame: " + to_string(callingWindow->GetCurrentCharacterFrame());
		callingWindow->calculateAnimationFrame();
		
	});

	UILabel nextFrameLabel;
	nextFrameLabel.offset = XMVectorSet(-.06f, -.025f, 0, 0);
	nextFrameLabel.text = "Next Frame";
	nextFrameButton->AddTextLabel(nextFrameLabel);
	uiBackPane->AddChild(nextFrameButton, "nextFrame");

	UIImage* next5FrameButton = new UIImage(this, uiBackPane);
	next5FrameButton->SetUseRelativeScale(false);
	next5FrameButton->SetUseRelativePosition(false);
	next5FrameButton->SetClickable(true);
	next5FrameButton->SetFocusable(false);
	next5FrameButton->SetUseCompoundTransformation(false);
	next5FrameButton->SetPosition(XMVectorSet(.5f, .024f, 0, 0));
	next5FrameButton->SetBounds(XMVectorSet(.15f, .075f, 0, 0));
	next5FrameButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
	next5FrameButton->SetVisible(true);
	next5FrameButton->SetOnClickCallback([](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
	{
		GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);
		callingWindow->SetCurrentCharacterFrame(callingWindow->GetCurrentCharacterFrame() + 5);
		callingWindow->GetCurrentFrameButton()->GetTextLabels()[0].text = "Current Frame: " + to_string(callingWindow->GetCurrentCharacterFrame());
		callingWindow->calculateAnimationFrame();

	});

	UILabel next5FrameLabel;
	next5FrameLabel.offset = XMVectorSet(-.06f, -.025f, 0, 0);
	next5FrameLabel.text = "Next 5 Frames";
	next5FrameButton->AddTextLabel(next5FrameLabel);
	uiBackPane->AddChild(next5FrameButton, "next5Frame");

	UIImage* previous5FrameButton = new UIImage(this, uiBackPane);
	previous5FrameButton->SetUseRelativeScale(false);
	previous5FrameButton->SetUseRelativePosition(false);
	previous5FrameButton->SetClickable(true);
	previous5FrameButton->SetFocusable(false);
	previous5FrameButton->SetUseCompoundTransformation(false);
	previous5FrameButton->SetPosition(XMVectorSet(.34f, -.01f, 0, 0));
	previous5FrameButton->SetBounds(XMVectorSet(.15f, .075f, 0, 0));
	previous5FrameButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
	previous5FrameButton->SetVisible(true);
	previous5FrameButton->SetOnClickCallback([](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
	{
		GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);
		if (callingWindow->GetCurrentCharacterFrame() >= 5)
		{
			callingWindow->SetCurrentCharacterFrame(callingWindow->GetCurrentCharacterFrame() - 5);
			callingWindow->GetCurrentFrameButton()->GetTextLabels()[0].text = "Current Frame: " + to_string(callingWindow->GetCurrentCharacterFrame());
		}
		else if (callingWindow->GetCurrentCharacterFrame() > 0)
		{
			callingWindow->SetCurrentCharacterFrame(0);
			callingWindow->GetCurrentFrameButton()->GetTextLabels()[0].text = "Current Frame: " + to_string(callingWindow->GetCurrentCharacterFrame());
		}

		callingWindow->calculateAnimationFrame();
	});

	UILabel previous5FrameLabel;
	previous5FrameLabel.offset = XMVectorSet(-.06f, -.025f, 0, 0);
	previous5FrameLabel.text = "Prev 5 Frames";
	previous5FrameButton->AddTextLabel(previous5FrameLabel);
	uiBackPane->AddChild(previous5FrameButton, "next5Frame");

	uiBackPane->AddChild(nextFrameButton, "nextFrame");
	UIImage* previousFrameButton = new UIImage(this, uiBackPane);
	previousFrameButton->SetUseRelativeScale(false);
	previousFrameButton->SetUseRelativePosition(false);
	previousFrameButton->SetClickable(true);
	previousFrameButton->SetFocusable(false);
	previousFrameButton->SetUseCompoundTransformation(false);
	previousFrameButton->SetPosition(XMVectorSet(.34f, .1f, 0, 0));
	previousFrameButton->SetBounds(XMVectorSet(.15f, .075f, 0, 0));
	previousFrameButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
	previousFrameButton->SetVisible(true);
	previousFrameButton->SetOnClickCallback([](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
	{
		GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);
		if (callingWindow->GetCurrentCharacterFrame() > 0)
		{
			callingWindow->DecrementCurrentCharacterFrame();
			callingWindow->GetCurrentFrameButton()->GetTextLabels()[0].text = "Current Frame: " + to_string(callingWindow->GetCurrentCharacterFrame());
		}

		callingWindow->calculateAnimationFrame();
	});

	UILabel previousFrameLabel;
	previousFrameLabel.offset = XMVectorSet(-.06f, -.025f, 0, 0);
	previousFrameLabel.text = "Prev Frame";
	previousFrameButton->AddTextLabel(previousFrameLabel);
	uiBackPane->AddChild(previousFrameButton, "previousFrame");
	uiBackPane->SetFocusElement(0);
	

	UIControlContainer* characterBlockPane = new UIControlContainer(this, UIContainerLayout(), uiBackPane);
	characterBlockPane->SetPosition(XMVectorSet(.7f, .5f, 0, 0));
	characterBlockPane->SetBounds(XMVectorSet(.75f * 10.f / 16.f, .75f, 0, 0));
	characterBlockPane->SetForegroundTint(XMVectorSet(1, .6f, .6f, .6f));
	characterBlockPane->SetUseRelativeScale(false);
	characterBlockPane->SetUseRelativePosition(false);
	characterBlockPane->SetUseCompoundTransformation(false);
	characterBlockPane->SetVisible(true);
	characterBlockPane->SetAutoPassClick(false);
	characterBlockPane->SetClickable(true);
	characterBlockPane->SetOnClickCallback([](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
	{
		GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);
		

		callingWindow->addAnimation(x, y);
	});

	uiBackPane->AddChild(characterBlockPane, "characterBlockPane");
	//characterBlockPane->SetFocusElement(0);
}

bool GameMenuWindow::addAnimation(float x, float y)
{
	if (this->selectedBlock == NULL)
	{
		return false;
	}

	if (this->simulation == NULL)
		return false;

	this->playerCharacterID = this->simulation->GetPlayerCharacterID();

	if (!this->simulation->IsObjectValid(this->playerCharacterID))
	{
		return false;
	}

	XMFLOAT4 selectedBlockPosition;
	XMStoreFloat4(&selectedBlockPosition, this->selectedBlock->GetPosition());

	const float blockWidth = .03f;
	const float margin = .005f;
	const float aspectRatio = this->GetWidth() / this->GetHeight();
	const float blockOffsetX = .70f;
	const float blockOffsetY = .3f;

	int squareWidth = static_cast<int>(blockWidth * this->GetWidth());
	int squareHeight = static_cast<int>(blockWidth * this->GetHeight() * aspectRatio);
	//XMVectorSet(x / blockWidth + blockOffsetX, x * blockWidth * aspectRatio + blockOffsetY, 0, 0)
	//float a = (x / (this->GetWidth() / / blockWidth - blockOffsetX;
	//float b = y / blockWidth - blockOffsetY;
	const float a = (x / (this->GetWidth())) * 2 - 1 - blockOffsetX;
	const float b = (y / (this->GetHeight())) * 2 - 1 - blockOffsetY;
	const int xLoc = (a / blockWidth + (a > 0 ? .5f : -.5f));;//x / (squareWidth) -blockOffsetX;
	const int yLoc = (b / (blockWidth * aspectRatio) + (b > 0 ? .5f : -.5f));;// /y / (squareWidth) +blockOffsetY;

	/*if (this->blockGrid[this->gridWidth / 2 + xLoc][this->gridWidth / 2 + yLoc] != NULL)
	{
		return false;
	}*/

	const float aP = selectedBlockPosition.x - blockOffsetX;//(x / (this->GetWidth())) * 2 - 1 - blockOffsetX;
	const float bP = selectedBlockPosition.y - blockOffsetY;//(y / (this->GetHeight())) * 2 - 1 - blockOffsetY;
	const int xLocP = aP / blockWidth + (aP > 0 ? .5f : -.5f);//x / (squareWidth) -blockOffsetX;
	const int yLocP = bP / (blockWidth * aspectRatio) + (bP > 0 ? .5f : -.5f);// /y / (squareWidth) +blockOffsetY;

	const int xChange = abs(xLocP - xLoc);
	const int yChange = abs(yLocP - yLoc);
	if (xChange > 1 || yChange > 1 || (xChange == 0 && yChange == 0))
	{
		return false;
	}

	PlatformerCharacter* playerCharacter = static_cast<PlatformerCharacter*>(this->simulation->GetWorldObjectByGlobalID(this->playerCharacterID));
	//playerCharacter->ResetAnimationState();
	const int blockIndex = stoi(this->selectedBlock->GetName());
	playerCharacter->AddBlockAnimation(blockIndex, this->GetCurrentCharacterFrame(), xLoc - xLocP, yLoc - yLocP);
	this->calculateAnimationFrame(true, blockIndex);
	playerCharacter->ResetAnimationState();
	return true;
}

void GameMenuWindow::calculateAnimationFrame(bool forceChange, int forceChangeIndex)
{

	if (this->simulation == NULL)
		return;

	this->playerCharacterID = this->simulation->GetPlayerCharacterID();

	if (this->simulation->IsObjectValid(this->playerCharacterID))
	{
		const PlatformerCharacter* playerCharacter = static_cast<PlatformerCharacter*>(this->simulation->GetWorldObjectByGlobalID(this->playerCharacterID));
		UIControlContainer* uiBackPane = static_cast<UIControlContainer*>(this->inventoryUIContainer->GetChild("uiBackPane"));
		UIControlContainer* characterBlockPane = static_cast<UIControlContainer*>(uiBackPane->GetChild("characterBlockPane"));

		//vector<const BlockCoordinate::AnimationDefinition&> animationList;
		const float blockWidth = .03f;
		const float margin = .005f;
		const float aspectRatio = this->GetWidth() / this->GetHeight();
		const float blockOffsetX = .70f;
		const float blockOffsetY = .3f;

		for (int blockIndex = 0; blockIndex < playerCharacter->GetBlockList().size(); ++blockIndex)
		{
			if (playerCharacter->GetBlockList()[blockIndex].isAlive)
			{
				const BlockCoordinate& currentBlock = playerCharacter->GetBlockList()[blockIndex];
				int xTranslation = 0;
				int yTranslation = 0;
				bool hasChanged = false;
				for (int animationIndex = 0; animationIndex < currentBlock.animationStageCount; ++animationIndex)
				{
					if (this->GetCurrentCharacterFrame() >= currentBlock.animationStageList[animationIndex].when)
					{
						hasChanged = true;
						xTranslation += currentBlock.animationStageList[animationIndex].xTranslation;
						yTranslation += currentBlock.animationStageList[animationIndex].yTranslation;					
					}
				}

				if (hasChanged || this->GetCurrentCharacterFrame() == 0)
				{
					string blockName = to_string(blockIndex);
					UIImage* blockImage = static_cast<UIImage*>(characterBlockPane->GetChild(blockName));
					blockImage->SetPosition(XMVectorSet(((currentBlock.originalX + xTranslation) * blockWidth) + blockOffsetX,
						(currentBlock.originalY + yTranslation) * blockWidth  * aspectRatio + blockOffsetY, 0, 0));
				}
			}
		}
	}
}

string GameMenuWindow::GetSelectedBlockType() const
{
	UIControlContainer* backPane = static_cast<UIControlContainer*>(this->inventoryUIContainer->GetChild("uiBackPane"));
	const string& focusName = backPane->GetFocusElement()->GetName();
	return focusName;
}

void GameMenuWindow::UpdateCursorLocation(float x, float y)
{
	this->cursorImage->SetPosition(XMVectorSet(x, y, 0, 0));
}

void GameMenuWindow::SetCursorVisible(bool showCursor, bool firstCall)
{
	if (showCursor)
	{
		this->updateInventoryUI(firstCall);
		this->SetForegroundUIElement(this->inventoryUIContainer);
	}
	else
	{
		this->SetForegroundUIElement(this->inEngineOverlayContainer);
	}
}

void GameMenuWindow::updateInventoryUI(bool firstCall)
{
	if (this->simulation == NULL)
		return;

	this->playerCharacterID = this->simulation->GetPlayerCharacterID();

	if (this->simulation->IsObjectValid(this->playerCharacterID))
	{
		PlatformerCharacter* playerCharacter = static_cast<PlatformerCharacter*>(this->simulation->GetWorldObjectByGlobalID(this->playerCharacterID));
		UIControlContainer* uiBackPane = static_cast<UIControlContainer*>(this->inventoryUIContainer->GetChild("uiBackPane"));
		
		UIElement* blockCountButton = uiBackPane->GetChild("blockCountButton");

		blockCountButton->GetTextLabels()[0].text = "Total Block Count " + std::to_string(playerCharacter->GetActiveBlockCount());

		//create a UI element for each block type in the character's inventory.
		int index = 1;
		const float sep = .16f;
		const int offsetCount = uiBackPane->GetChildren().size();
		auto inventoryMap = playerCharacter->GetInventoryMap();
		
		for (auto it = inventoryMap.begin(); it != inventoryMap.end(); ++it)
		{
			if (it->first != "characterBlockPane")
			{
				if (uiBackPane->GetChild(it->first) == NULL)
				{
					UIImage* newButton = new UIImage(this, uiBackPane);
					newButton->SetUseRelativeScale(false);
					newButton->SetUseRelativePosition(true);
					newButton->SetPosition(XMVectorSet(-.8f, -.1f - index * sep, 0, 0));
					newButton->SetBounds(XMVectorSet(.3f, .15f, 0, 0));
					newButton->SetForegroundTint(XMVectorSet(1, .2f, .2f, .6f));
					newButton->SetVisible(true);
					newButton->SetFocusable(true);
					UILabel newLabel;
					newLabel.offset = XMVectorSet(-.1f, -.01f, 0, 0);
					newLabel.text = it->first + "s: " + to_string(it->second.count);
					newButton->AddTextLabel(newLabel);
					uiBackPane->AddChild(newButton, it->first);
					this->blockTypeConverter[offsetCount + index - 1] = GameResources::blockTypes[it->first];
					//uiBackPane->SetFocusElement(index);
					++index;
				}
				else
				{
					uiBackPane->GetChild(it->first)->GetTextLabels()[0].text = it->first + "s: " + to_string(it->second.count);
				}
			}
		}

		//The UIElements used to display the blocks are reused forever.
		UIControlContainer* characterBlockPane = static_cast<UIControlContainer*>(uiBackPane->GetChild("characterBlockPane"));

		if (firstCall)
		{
			for (auto it = characterBlockPane->GetChildrenMap().begin(); it != characterBlockPane->GetChildrenMap().end(); ++it)
			{
				it->second->SetVisible(false);
			}

			for (int a = 0; a < this->gridWidth; ++a)
			{
				for (int b = 0; b < this->gridWidth; ++b)
				{
					this->blockGrid[a][b] = NULL;
				}
			}
		}

		const float blockWidth = .03f;
		const float margin = .005f;
		const float aspectRatio = this->GetWidth() / this->GetHeight();
		const float blockOffsetX = .70f;
		const float blockOffsetY = .3f;
		for (int blockIndex = 0; blockIndex < playerCharacter->GetBlockList().size(); ++blockIndex)
		{
			const BlockCoordinate& currentBlock = playerCharacter->GetBlockList()[blockIndex];
			string blockName = to_string(blockIndex);
			UIImage* blockImage = static_cast<UIImage*>(characterBlockPane->GetChild(blockName));

			if (currentBlock.isAlive)
			{
				if (blockImage == NULL)
				{
					UIImage* newButton = new UIImage(this, uiBackPane);
					newButton->SetUseRelativeScale(false);
					//newButton->SetUseRelativePosition(true);
					newButton->SetUseCompoundTransformation(false);
					newButton->SetPosition(XMVectorSet(currentBlock.originalX * blockWidth + blockOffsetX, currentBlock.originalY * blockWidth * aspectRatio + blockOffsetY, 0, 0));
					//newButton->SetPosition(XMVectorSet(blockOffset, blockOffset, 0, 0));
					newButton->SetBounds(XMVectorSet(blockWidth - margin, blockWidth * aspectRatio - margin * aspectRatio, 0, 0));
					switch (currentBlock.blockType)
					{
					case REGULAR:
						newButton->SetForegroundTint(XMVectorSet(.7f, .7f, .7f, .6f));
						newButton->SetFocusTint(XMVectorSet(.7f, .7f, .7f, .6f));
						break;
					case ARMOR:
						break;
					case GUN:
						newButton->SetForegroundTint(XMVectorSet(.1f, .8f, .2f, .6f));
						newButton->SetFocusTint(XMVectorSet(.1f, .8f, .2f, .6f));
						break;
					case HEART:
						newButton->SetForegroundTint(XMVectorSet(.1f, .1f, .8f, .6f));
						newButton->SetFocusTint(XMVectorSet(.1f, .1f, .8f, .6f));
						break;
					}
					newButton->SetVisible(true);
					newButton->SetFocusable(false);
					newButton->SetClickable(true);
					newButton->SetOnClickCallback([](UIElement* callingElement, Window* owner, short whichButton, float x, float y)
					{
						GameMenuWindow* callingWindow = static_cast<GameMenuWindow*>(owner);
						UIImage* clickedBlock = static_cast<UIImage*>(callingElement);
						if (clickedBlock != callingWindow->GetSelectedBlock())
						{
							clickedBlock->SetFocusTint(clickedBlock->GetForegroundTint());
							clickedBlock->SetForegroundTint(XMVectorSet(1, 1, 1, 1));
							if (callingWindow->GetSelectedBlock() != NULL)
							{
								callingWindow->GetSelectedBlock()->SetForegroundTint(callingWindow->GetSelectedBlock()->GetFocusTint());
							}
							callingWindow->SetSelectedBlock(clickedBlock);
						}
						else
						{
							//clickedBlock->SetForegroundTint(XMVectorSet(.1, .8f, .2f, .6f));
							clickedBlock->SetForegroundTint(clickedBlock->GetForegroundTint());
							callingWindow->SetSelectedBlock(NULL);
						}
					});

					characterBlockPane->AddChild(newButton, blockName);
					this->blockGrid[this->gridWidth / 2 + currentBlock.originalX][this->gridWidth / 2 + currentBlock.originalY] = newButton;
				}
				else if (!blockImage->IsVisible() || firstCall)
				{
					blockImage->SetVisible(true);
					blockImage->SetPosition(XMVectorSet(currentBlock.originalX * blockWidth + blockOffsetX, currentBlock.originalY * blockWidth * aspectRatio + blockOffsetY, 0, 0));
					switch (currentBlock.blockType)
					{
					case REGULAR:
						blockImage->SetForegroundTint(XMVectorSet(.7f, .7f, .7f, .6f));
						break;
					case ARMOR:
						break;
					case GUN:
						blockImage->SetForegroundTint(XMVectorSet(.1f, .8f, .2f, .6f));
						break;
					case HEART:
						blockImage->SetForegroundTint(XMVectorSet(.1f, .1f, .8f, .6f));
						break;
					}
					this->blockGrid[this->gridWidth / 2 + currentBlock.originalX][this->gridWidth / 2 + currentBlock.originalY] = blockImage;
					//blockImage->SetPosition(XMVectorSet(blockOffset, blockOffset, 0, 0));
				}
			}
			else if (blockImage != NULL && blockImage->IsVisible())
			{
				blockImage->SetVisible(false);
				this->blockGrid[this->gridWidth / 2 + currentBlock.originalX][this->gridWidth / 2 + currentBlock.originalY] = NULL;
			}
		}

		if (uiBackPane->GetFocusElementIndex() == 0)
		{
			uiBackPane->SetFocusElement(9);
		}

		playerCharacter->SetSelectedBlockType(this->blockTypeConverter[uiBackPane->GetFocusElementIndex()]);
	}
}

void GameMenuWindow::UpdateSelectedBlockType(int focusIndex)
{
	if (this->simulation == NULL)
		return;

	this->playerCharacterID = this->simulation->GetPlayerCharacterID();

	if (this->simulation->IsObjectValid(this->playerCharacterID))
	{
		PlatformerCharacter* playerCharacter = static_cast<PlatformerCharacter*>(this->simulation->GetWorldObjectByGlobalID(this->playerCharacterID));
		playerCharacter->SetSelectedBlockType(this->blockTypeConverter[focusIndex]);
	}
}

void GameMenuWindow::RefreshUIData(bool firstCall)
{
	this->updateInventoryUI(firstCall);
}

bool GameMenuWindow::SetKeyDown(unsigned short whichKey, bool shiftDown)
{
	bool handled = __super::SetKeyDown(whichKey);

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

GameMenuWindow::~GameMenuWindow()
{

}