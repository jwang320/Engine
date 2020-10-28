#include "pch.h"
#include "BlockUniverse.h"
#include "GameResources.h"

using namespace SimulationEngine;

BlockUniverse::BlockUniverse(int width, int height, float gridSpacing, float squareWidth, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
	Actor(objectParameters, moveableParameters),
	width(width),
	height(height),
	gridSpacing(gridSpacing),
	squareWidth(squareWidth),
	totalBlockCount(0)
{
	this->objectType = ObjectTypes::BlockUniverse;

	this->blockGrid = new BlockDefinition*[this->width];

	for (int a = 0; a < this->width; ++a)
	{
		this->blockGrid[a] = new BlockDefinition[this->height];
	}

	for (int x = 0; x < this->width; ++x)
	{
		const int thisH = (x == 5 || x == 5) ? 3 : 2;// rand() % 4;
		for (int y = 0; y < this->height; ++y)
		{
			this->blockGrid[x][y].blockType = 0;
			this->blockGrid[x][y].ownerID = -1;
			/*if (y <= thisH || x > 20 && x < 80 && (y == 13 || y == 14 || y == 15))// && (x % 4 == 2 || x % 4 == 3))
			{
				this->blockGrid[x][y].blockType = 1;
				++this->totalBlockCount;
			}*/
		}
	}
	const std::vector<BlockCoordinate>& blockList = GameResources::RoomModelDefinitions[GameResources::RoomModelResourceMap["SmallRoom.m"]].blockList;
	const int halfWidth = GameResources::RoomModelDefinitions[GameResources::RoomModelResourceMap["SmallRoom.m"]].width / 2;
	const int halfHeight = GameResources::RoomModelDefinitions[GameResources::RoomModelResourceMap["SmallRoom.m"]].height / 2;
	for (int a = 0; a < blockList.size(); ++a)
	{
		this->blockGrid[blockList[a].x][blockList[a].y].blockType = 1;
		++this->totalBlockCount;
	}
}

//Left, Right, Down, Up
void BlockUniverse::FindBlockEdges(int x, int y, bool edges[4]) const
{
	edges[0] = x == 0 || this->blockGrid[x - 1][y].blockType != BlockTypes::REGULAR;
	edges[1] = x == this->width - 1 || this->blockGrid[x + 1][y].blockType != BlockTypes::REGULAR;
	edges[2] = y == 0 || this->blockGrid[x][y - 1].blockType != BlockTypes::REGULAR;
	edges[3] = y == this->height - 1 || this->blockGrid[x][y + 1].blockType != BlockTypes::REGULAR;
}

BlockUniverse::~BlockUniverse()
{
	for (int a = 0; a < this->width; ++a)
	{
		delete[] this->blockGrid[a];
	}

	delete[] this->blockGrid;
}