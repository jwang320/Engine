#pragma once 
#include "../Simulation Engine Physical/Actor.h"
#include "BlockUniverse.h"
#include "PlatformerBrain.h"
#include <map>

class PlatformerWorld;

struct BlockCoordinate
{
	int x;
	int y;
	int blockType;
	int blockLevel;
	int deathTimer;
	unsigned int createVersion;
	bool isAlive;
	int originalX;
	int originalY;
	int animationStageIndex;
	int animationStageCount;
	unsigned int currentAnimationFrame;
	struct AnimationDefinition { unsigned int when; int xTranslation; int yTranslation; int newX; int newY; };
	std::vector<AnimationDefinition> animationStageList;
};

enum BlockTypes
{
	REGULAR,
	ARMOR,
	GUN,
	HEART,
	THRUSTER
};

struct InventoryStackDescription
{
	int count;
	int blockType;
};

class PlatformerCharacter : public SimulationEngine::Actor
{
	friend class PlatformerBrain;

protected:
	static const int gridWidth = 64;

	//The 2D list of coordinates relative to the character's center,
	//dictating where the character's blocks exist.
	std::vector<BlockCoordinate> blockList;
	std::vector<int> blockTypeList[5];
	int blockGrid[gridWidth][gridWidth];
	BlockUniverse* parentBlockUniverse;
	bool removeOnRest;
	bool illuminateVelocity;
	int blockType;
	int deathTimer;
	//when the block configuration changes, this number should change so the renderer
	//knows to search for new changes
	unsigned int modificationVersion;
	bool isHeartVulnerable;
	PlatformerBrain* myBrain;
	bool movementFriction;
	int activeBlockCount;
	std::map<std::string, InventoryStackDescription> inventoryMap;
	int owningCharacter;
	int weaponCooldown;
	int weaponFrameCount;
	bool canWalkStairs;
	DirectX::XMVECTOR previousLocation;
	int frameUpdate;
	bool useDiscreteAnimation;
	unsigned int currentAnimationCount;
	int selectedBlockType;
	int maxBlockLevel;

public:
	PlatformerCharacter(BlockUniverse* parentBlockUniverse, bool removeOnRest = false, int owningCharacter = -1, int owningPlayer = 0, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

	virtual void Act() override;
	virtual void OnBlockCollision(const BlockDefinition& blockDefinition, const DirectX::XMVECTOR& collisionLocation);
	virtual void die();
	void DamageBlock(int blockIndex, float damageAmount);
	void KillBlock(int blockIndex);
	void KillBlocks(const std::vector<int>& blocKillList);
	bool AddBlock(const DirectX::XMVECTOR& pos, int blockType);
	bool AddBlock(const DirectX::XMVECTOR& pos, const std::string& blockType);
	bool IsValidBlockAddLocation(int x, int y) const;
	bool IsValidBlockAddLocation(const DirectX::XMVECTOR& pos) const;
	bool IsValidBlockReplaceLocation(const DirectX::XMVECTOR & pos) const;
	void InitializeAIControl(const PlatformerBrain::AIBrainParameters& parameters = PlatformerBrain::AIBrainParameters());
	void FireGuns(const DirectX::XMVECTOR& targetLocation);
	void FireGuns(float angle); // in radians
	void DeactivateBlocks();
	void RefreshWorldBlockLocations();
	void ResetAnimationState();
	//returns false if animation is invalid -- translations should be between -1 and 1
	bool AddBlockAnimation(int blockIndex, unsigned int when, int xTranslation, int yTranslation);
	
	const std::vector<BlockCoordinate>& GetBlockList() const { return this->blockList; }
	const std::vector<int>& GetBlockListByType(int blockType) const { return this->blockTypeList[blockType]; }
	int GetBlockCount() const { return static_cast<int>(this->blockList.size()); }
	int GetActiveBlockCount() const { return this->activeBlockCount; }
	BlockUniverse* GetParentBlockUniverse() const { return this->parentBlockUniverse; }
	bool IlluminateVelocity() const { return this->illuminateVelocity; }
	bool UseMovementFriction() const { return this->movementFriction; }
	unsigned int GetModificationVersion() const { return this->modificationVersion; }
	int GetOwningCharacter() const { return this->owningCharacter; }
	int GetOwningPlayer() const { return this->owningPlayer; }
	const std::map<std::string, InventoryStackDescription>& GetInventoryMap() const { return this->inventoryMap; }
	bool CanWalkStairs() const { return this->canWalkStairs; }
	int GetBlockIndex(int x, int y) const { return this->blockGrid[x + this->gridWidth / 2][y + this->gridWidth / 2]; }
	DirectX::XMVECTOR GetPreviousLocation() const { return this->previousLocation; }
	bool UseDiscreteAnimation() const { return this->useDiscreteAnimation; }
	bool IsBlockFoot(int blockIndex) const;
	int GetBlockType() const { return this->blockType; }
	unsigned int GetCurrentAnimationCount() const { return this->currentAnimationCount; }

	void SetIlluminateVelocity(bool illuminateVelocity) { this->illuminateVelocity = illuminateVelocity; }
	void SetDeathTimer(int deathTimer) { this->deathTimer = deathTimer; }
	void SetCanWalkStairs(bool canWalkStairs) { this->canWalkStairs = canWalkStairs; }
	void SetSelectedBlockType(int selectedBlockType) { this->selectedBlockType = selectedBlockType; }

protected:
	void loadBlockModel(const std::string& modelName);
	void rebuildBlockGrid();
	void killDisconnectedBlocks();
	void deactivateBlock(int blockIndex);
	void refreshBlockDeathTimers();

private:
	bool hasPathToBlockOfType(int blockIndex, int targetBlockType, std::vector<int>& visitedList) const;
	void calculateNextAnimationFrame();
};