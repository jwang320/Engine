#pragma once

class BlockUniverse;
class PlatformerCharacter;

//The GameSimulation class will contain the logic for how the game is run.
class GameSimulation : public SimulationEngine::PhysicalWorldSimulation
{
protected:
	BlockUniverse* blockUniverse;
	PlatformerCharacter* playerChar;
	bool isBlockAddMode;
	DirectX::XMVECTOR cursorWorldLocation;
	const std::string levelName;

public:
	GameSimulation(const std::string& levelName = "TestWorld.gd");
	void InitializeWorld();
	void EnterWorld();
	void InitializeLocalPlayer(int playerID = -1);
	bool TestCreateBlock(const DirectX::XMVECTOR& pos, const std::string& blockType);
	virtual void ResetWorld() override;
	virtual void Update() override;

	void SetBlockAddMode(bool isBlockAddMode) { this->isBlockAddMode = isBlockAddMode; }
	void SetCursorWorldLocation(const DirectX::XMVECTOR& cursorWorldLocation) { this->cursorWorldLocation = cursorWorldLocation; }
	bool IsBlockAddMode() const { return this->isBlockAddMode; }
	DirectX::XMVECTOR GetCursorWorldLocation() const { return this->cursorWorldLocation; }
	int GetPlayerCharacterID() const;
};