#pragma once

#include "..\SimpleUI\Window.h"
#include <map>

class GameApplication;
class GameSimulation;

namespace SimpleUI
{
	class UIControlContainer;
	class UIElement;
	class UIImage;
}

class GameMenuWindow : public SimpleUI::Window
{
private:
	SimpleUI::UIControlContainer* mainMenuUIContainer;
	SimpleUI::UIElement* inEngineOverlayContainer;
	SimpleUI::UIControlContainer* inventoryUIContainer;
	SimpleUI::UIImage* cursorImage;
	GameApplication* application;
	GameSimulation* simulation;
	bool showCursor;
	int playerCharacterID;
	unsigned int currentCharacterFrame;
	SimpleUI::UIElement* currentFrameImage;
	SimpleUI::UIImage* selectedBlock;
	static const int gridWidth = 64;
	SimpleUI::UIImage* blockGrid[gridWidth][gridWidth];
	int selectedBlockInt;
	std::map<int, int> blockTypeConverter;

public:
	GameMenuWindow(GameApplication* application);
	virtual bool SetKeyDown(unsigned short whichKey, bool shiftDown = false);


	void SetSimulation(GameSimulation* simulation) { this->simulation = simulation; }
	void UpdateCursorLocation(float x, float y);
	void SetCursorVisible(bool showCursor, bool firstCall);
	std::string GetSelectedBlockType() const;
	void RefreshUIData(bool firstCall);
	void calculateAnimationFrame(bool forceChange = false, int forceChangeIndex = 0);
	bool addAnimation(float x, float y);
	void IncrementCurrentCharacterFrame() { ++this->currentCharacterFrame; }
	void SetCurrentCharacterFrame(int frame) { this->currentCharacterFrame = frame; }
	void DecrementCurrentCharacterFrame() { --this->currentCharacterFrame; }
	unsigned int GetCurrentCharacterFrame() const { return this->currentCharacterFrame; }
	void UpdateSelectedBlockType(int focusIndex);
	SimpleUI::UIImage* GetSelectedBlock() { return this->selectedBlock; }
	void SetSelectedBlock(SimpleUI::UIImage* selectedBlock) { this->selectedBlock = selectedBlock; }
	SimpleUI::UIElement* GetCurrentFrameButton() { return this->currentFrameImage; }
	~GameMenuWindow();

private:
	void initializeInventoryUI();
	void updateInventoryUI(bool firstCall);
};