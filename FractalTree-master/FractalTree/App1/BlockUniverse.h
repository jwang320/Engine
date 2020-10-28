#pragma once 
#include "../Simulation Engine Physical/Actor.h"

struct BlockDefinition
{
	int blockType;
	int ownerID;
	int blockOwnerIndex;
};

class BlockUniverse : public SimulationEngine::Actor
{
protected:
	const int width;
	const int height;
	BlockDefinition** blockGrid; //2D Grid array, 0 is empty
	const float gridSpacing;
	const float squareWidth;
	int totalBlockCount;

public:
	BlockUniverse(int width, int height, float gridSpacing, float squareWidth, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

	BlockDefinition** GetBlockGrid() const { return this->blockGrid; }
	int GetBlockType(int x, int y) const { return this->IsValidGridCoordinate(x, y) ? this->blockGrid[x][y].blockType : 0; }
	int GetBlockOwner(int x, int y) const { return this->IsValidGridCoordinate(x, y) ? this->blockGrid[x][y].ownerID : -1; }
	int GetBlockOwnerIndex(int x, int y) const { return this->IsValidGridCoordinate(x, y) ? this->blockGrid[x][y].blockOwnerIndex : -1; }
	BlockDefinition GetBlockDefinition(int x, int y) const { return this->IsValidGridCoordinate(x, y) ? this->blockGrid[x][y] : BlockDefinition(); }
	int GetWidth() const { return this->width; }
	int GetHeight() const { return this->height; }
	int GetTotalBlockCount() const { return this->totalBlockCount; }
	//Left, Right, Down, Up
	void FindBlockEdges(int x, int y, bool edges[4]) const;
	float GetGridSpacing() const { return this->gridSpacing; }
	float GetSquareWidth() const { return this->squareWidth; }
	void SetBlockType(int x, int y, int blockType, int ownerID, int blockIndex) { if (this->IsValidGridCoordinate(x, y)) { this->blockGrid[x][y].blockType = blockType; this->blockGrid[x][y].ownerID = ownerID; this->blockGrid[x][y].blockOwnerIndex = blockIndex; } }
	bool IsValidGridCoordinate(int x, int y) const { return x >= 0 && y >= 0 && x < this->width && y < this->height; }
	~BlockUniverse();

	enum BlockTypes
	{
		EMPTY,
		REGULAR,
		CHARACTER
	};
};