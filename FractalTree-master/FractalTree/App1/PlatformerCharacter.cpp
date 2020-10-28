#include "pch.h"
#include "PlatformerCharacter.h"
#include "GameResources.h"
#include "PlatformerMissile.h"
#include "PlatformerWorld.h"

using namespace SimulationEngine;
using namespace DirectX;
using std::vector;
using std::string;

PlatformerCharacter::PlatformerCharacter(BlockUniverse* parentBlockUniverse, bool removeOnRest, int owningCharacter, int owningPlayer, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
	Actor(objectParameters, moveableParameters, ActorParameters(), owningPlayer),
	parentBlockUniverse(parentBlockUniverse),
	removeOnRest(removeOnRest),
	illuminateVelocity(false),
	blockType(2),
	deathTimer(-1),
	modificationVersion(0),
	isHeartVulnerable(true),
	movementFriction(true),
	activeBlockCount(0),
	owningCharacter(owningCharacter),
	myBrain(NULL),
	weaponCooldown(20),
	weaponFrameCount(0),
	canWalkStairs(false),
	frameUpdate(0),
	useDiscreteAnimation(GameResources::UseDiscreteAnimation),
	currentAnimationCount(0),
	selectedBlockType(0),
	maxBlockLevel(6)
{
	this->objectType = ObjectTypes::PlatformerCharacter;
	this->scale = XMVectorSet(this->parentBlockUniverse->GetSquareWidth(), this->parentBlockUniverse->GetSquareWidth(), this->parentBlockUniverse->GetSquareWidth(), 0);
	this->componentFlags |= PhysicalWorld::PHYSICS;
	this->componentFlags |= PhysicalWorld::PATH;

	this->blockList.push_back({ 0, 0, 0, 0, -1, 0, true, 0, 0, 0, 0, 0 });
	this->inventoryMap["Regular Block"] = { GameResources::RegularBlockStartCount, REGULAR };
	this->inventoryMap["Armor Block"] = { GameResources::ArmorBlockStartCount, ARMOR };
	this->inventoryMap["Gun Block"] = { GameResources::GunBlockStartCount, GUN };
	this->inventoryMap["Thruster Block"] = { GameResources::ThrusterBlockStartCount, THRUSTER };
	this->previousLocation = this->location;
}

void PlatformerCharacter::Act()
{
	//Check if any blocks on this character have a remove timer set
	this->refreshBlockDeathTimers();

	//Before moving the voxel character, set its current position to be
	//unoccupied in the world grid.

	this->DeactivateBlocks();
	if (this->deathTimer > 0)
	{
		--this->deathTimer;
	}
	else if (this->deathTimer == 0)
	{
		this->Remove();
		return;
	}

	if (this->weaponFrameCount > 0)
	{
		--this->weaponFrameCount;
	}

	if (this->myBrain != NULL)
	{
		this->myBrain->Think();
	}

	this->calculateNextAnimationFrame();

	this->previousLocation = this->location;

	//Move the voxel character.	
	this->UpdateForces();

	//if (!static_cast<PlatformerWorld*>(this->parentWorld)->CheckPlatformerCollision(this, true))
	{
		this->UpdatePosition();
	}
	
	//Update the voxel grid to be occupied by the new location.
	this->RefreshWorldBlockLocations();

	if (this->IsRested() && this->removeOnRest)
	{
		this->Remove();
	}
}

void PlatformerCharacter::InitializeAIControl(const PlatformerBrain::AIBrainParameters& parameters)
{
	this->myBrain = new PlatformerBrain(this, parameters);
	this->myBrain->SetTargetLocation(this->location);
	for (int a = 0; a < this->thrusters.size(); ++a)
	{
		this->thrusters[a]->maximumMagnitude = this->thrusters[a]->currentMagnitude;
		this->thrusters[a]->currentMagnitude = 0;
	}

	this->canWalkStairs = true;
}

bool PlatformerCharacter::AddBlock(const XMVECTOR& pos, int blockType)
{
	XMFLOAT4 blockPos;
	XMStoreFloat4(&blockPos, XMVectorDivide(XMVectorSubtract(pos, this->GetLocation()), XMVectorReplicate(this->parentBlockUniverse->GetGridSpacing())));
	const int xPos = static_cast<int>(blockPos.x + (blockPos.x > 0 ? .5f : -.5f));
	const int yPos = static_cast<int>(blockPos.y + (blockPos.y > 0 ? .5f : -.5f));
	if (this->IsValidBlockAddLocation(xPos, yPos))
	{
		this->blockGrid[xPos + this->gridWidth / 2][yPos + this->gridWidth / 2] = static_cast<int>(this->blockList.size());
		//store indices of special blocks
		if (blockType != 0)
		{
			this->blockTypeList[blockType].push_back(this->blockList.size());
		}

		++this->modificationVersion;
		this->blockList.push_back({ xPos, yPos, blockType, 0, -1, this->modificationVersion, true, xPos, yPos, 0, 0, 0 });

		++this->activeBlockCount;
		return true;
	}
	else if (this->IsValidBlockReplaceLocation(pos))
	{
		const int blockIndex = this->blockGrid[xPos + this->gridWidth / 2][yPos + this->gridWidth / 2];
		++this->blockList[blockIndex].blockLevel;

		++this->modificationVersion;
		return true;
	}

	return false;
}

bool PlatformerCharacter::AddBlock(const XMVECTOR& pos, const string& blockType)
{
	if (this->inventoryMap[blockType].count <= 0)
	{
		return false;
	}

	if (this->AddBlock(pos, this->inventoryMap[blockType].blockType))
	{
		--this->inventoryMap[blockType].count;
		return true;
	}

	return false;
}

void PlatformerCharacter::KillBlock(int blockIndex)
{
	if (this->isHeartVulnerable && this->blockList[blockIndex].blockType == HEART)
	{
		this->die();
	}
	else
	{
		this->blockList[blockIndex].isAlive = false;
		--this->activeBlockCount;
		++this->modificationVersion;

		const float deathSpeed = 3;
		const float deathAngle = (rand() % 360) * 3.1415f / 180;
		const XMVECTOR newLoc = XMVectorAdd(this->location, XMVectorMultiply(XMVectorSet(static_cast<float>(this->blockList[blockIndex].x), static_cast<float>(this->blockList[blockIndex].y), 0, 0), XMVectorReplicate(this->GetParentBlockUniverse()->GetGridSpacing())));
		const XMVECTOR newVel = XMVectorAdd(this->GetVelocity(), XMVectorSet(cosf(deathAngle) * deathSpeed * .25f, fabs(sinf(deathAngle)) * deathSpeed, 0, 0));
		PlatformerCharacter* newBlock = new PlatformerCharacter(this->parentBlockUniverse, false, this->GetGlobalID(), this->owningPlayer, ObjectParameters(this->parentWorld), MoveableParameters(newLoc, XMVectorReplicate(0), newVel));
		newBlock->illuminateVelocity = true;
		newBlock->deathTimer = 1000;
	}

	this->deactivateBlock(blockIndex);
}

void PlatformerCharacter::KillBlocks(const vector<int>& blockKillList)
{
	for (int a = 0; a < blockKillList.size(); ++a)
	{
		if (this->blockList[blockKillList[a]].blockLevel > 0)
		{
			--this->blockList[blockKillList[a]].blockLevel;
		}
		else
		{
			this->KillBlock(blockKillList[a]);
		}
	}

	this->killDisconnectedBlocks();
}

//This function searches for blocks with no connection to the
//Character's heart block
void PlatformerCharacter::killDisconnectedBlocks()
{
	for (int a = 0; a < this->blockList.size(); ++a)
	{
		if (this->blockList[a].isAlive)
		{
			vector<int> visitedList;

			if (!this->hasPathToBlockOfType(a, HEART, visitedList))
			{
				//If the current block has no path to a HEART block, then all visited blocks
				//in the current traversal will also have no path to a HEART block, so mark
				//all blocks in visited list for death.
				for (int visitedListIndex = 0; visitedListIndex < visitedList.size(); ++visitedListIndex)
				{
					//Setting the block's death timer to a small random number produces a cool cascading effect.
					this->blockList[visitedList[visitedListIndex]].deathTimer = rand() % 100;
					this->blockList[visitedList[visitedListIndex]].isAlive = false;
				}
			}
		}
	}
}

bool PlatformerCharacter::AddBlockAnimation(int blockIndex, unsigned int animationFrameCount, int xTranslation, int yTranslation)
{
	//animation states must be adjacent to previous state
	if (abs(xTranslation) > 1 || abs(yTranslation > 1))
	{
		return false;
	}

	BlockCoordinate& targetBlock = this->blockList[blockIndex];
	if (targetBlock.animationStageList.size() > 0 &&animationFrameCount <= targetBlock.animationStageList[targetBlock.animationStageList.size() - 1].when)
	{
		return false;
	}

	const int previousX = targetBlock.animationStageList.size() != 0 ? targetBlock.animationStageList[targetBlock.animationStageList.size() - 1].newX : targetBlock.originalX;
	const int previousY = targetBlock.animationStageList.size() != 0 ? targetBlock.animationStageList[targetBlock.animationStageList.size() - 1].newY : targetBlock.originalY;
	
	const int newGridX = previousX + this->gridWidth / 2 + xTranslation;
	const int newGridY = previousY + this->gridWidth / 2 + yTranslation;

	/*if (this->blockGrid[newGridX][newGridY] >= 0 && !(this->blockGrid[newGridX][newGridY] == blockIndex))
	{
		return false;
	}*/
		
	//code to validate final character state (all blocks must be adjacent to at least one other block;
	//no blocks can overlap
	//should go here

	//targetBlock.totalAnimationFrameCount += animationFrameCount;
	targetBlock.animationStageList.push_back({ animationFrameCount, xTranslation, yTranslation, previousX + xTranslation, previousY + yTranslation });
	++targetBlock.animationStageCount;

	return true;
}

void PlatformerCharacter::die()
{
	this->Remove();
	for (int a = 0; a < this->blockList.size(); ++a)
	{
		if (this->blockList[a].isAlive)
		{
			const float deathSpeed = 3;
			const float deathAngle = (rand() % 360) * 3.1415f / 180;
			const XMVECTOR newLoc = XMVectorAdd(this->location, XMVectorMultiply(XMVectorSet(static_cast<float>(this->blockList[a].x), static_cast<float>(this->blockList[a].y), 0, 0), XMVectorReplicate(this->GetParentBlockUniverse()->GetGridSpacing())));
			const XMVECTOR newVel = XMVectorAdd(this->GetVelocity(), XMVectorSet(cosf(deathAngle) * deathSpeed * .25f, fabs(sinf(deathAngle)) * deathSpeed, 0, 0));
			PlatformerCharacter* newBlock = new PlatformerCharacter(this->parentBlockUniverse, false, this->GetGlobalID(), this->owningPlayer, ObjectParameters(this->parentWorld), MoveableParameters(newLoc, XMVectorReplicate(0), newVel));
			newBlock->illuminateVelocity = true;
			newBlock->deathTimer = 1000;

			this->deactivateBlock(a);
		}
	}
}

void PlatformerCharacter::calculateNextAnimationFrame()
{
	//the final state after an animation frame will need to be validated, because
	//blocks could have been removed since the animation was added.

	for (int blockIndex = 0; blockIndex < this->blockList.size(); ++blockIndex)
	{
		BlockCoordinate& currentBlock = this->blockList[blockIndex];
		if (currentBlock.isAlive)
		{
			if (currentBlock.animationStageList.size() > 0 &&
				currentBlock.animationStageList[currentBlock.animationStageList.size() - 1].newX == currentBlock.originalX &&
				currentBlock.animationStageList[currentBlock.animationStageList.size() - 1].newY == currentBlock.originalY)
			{
				bool animationThisFrame = false;
				
				int newAnimationIndex = currentBlock.animationStageIndex;
				if (currentBlock.currentAnimationFrame >= currentBlock.animationStageList[currentBlock.animationStageIndex].when)
				{					
					animationThisFrame = true;
				}
				if (animationThisFrame)
				{
					this->blockGrid[currentBlock.x + this->gridWidth / 2][currentBlock.y + this->gridWidth / 2] = -1;
					currentBlock.x += currentBlock.animationStageList[newAnimationIndex].xTranslation;
					currentBlock.y += currentBlock.animationStageList[newAnimationIndex].yTranslation;
					this->blockGrid[currentBlock.x + this->gridWidth / 2][currentBlock.y + this->gridWidth / 2] = blockIndex;

					++this->currentAnimationCount;
					++newAnimationIndex;
					if (newAnimationIndex == currentBlock.animationStageList.size())
					{
						newAnimationIndex = 0;
						currentBlock.currentAnimationFrame = 0;
					}

					currentBlock.animationStageIndex = newAnimationIndex;
				}

				++currentBlock.currentAnimationFrame;				
			}
		}
	}
}

void PlatformerCharacter::ResetAnimationState()
{
	for (int blockIndex = 0; blockIndex < this->blockList.size(); ++blockIndex)
	{
		BlockCoordinate& currentBlock = this->blockList[blockIndex];
		if (currentBlock.isAlive)
		{
			if (currentBlock.animationStageList.size() > 0)
			{
				currentBlock.x = currentBlock.originalX;
				currentBlock.y = currentBlock.originalY;
			}
		}
	}

	for (int blockIndex = 0; blockIndex < this->blockList.size(); ++blockIndex)
	{
		BlockCoordinate& currentBlock = this->blockList[blockIndex];
		if (currentBlock.isAlive)
		{
			if (currentBlock.animationStageList.size() > 0)
			{
				currentBlock.currentAnimationFrame = 0;
				currentBlock.animationStageIndex = 0;
			}
		}
	}
}

//Do a recursive depth first search from the block to find any connected blocks of a specified type
bool PlatformerCharacter::hasPathToBlockOfType(int blockIndex, int targetBlockType, vector<int>& visitedList) const
{
	if (this->blockList[blockIndex].blockType == targetBlockType)
	{
		return true;
	}

	visitedList.push_back(blockIndex);
	bool foundPath = false;

	const int xInd = this->blockList[blockIndex].x;
	const int yInd = this->blockList[blockIndex].y;

	struct tempCoordStruct { int x, y; };

	tempCoordStruct targetCoords[8] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };

	for (int a = 0; a < 8; ++a)
	{
		const int targetIndex = this->blockGrid[targetCoords[a].x + xInd + this->gridWidth / 2][targetCoords[a].y + yInd + this->gridWidth / 2];
		if (targetIndex >= 0 && this->blockList[targetIndex].isAlive)
		{
			bool visited = false;

			for (int b = 0; b < visitedList.size(); ++b)
			{
				if (visitedList[b] == targetIndex)
				{
					visited = true;
					break;
				}
			}

			if (!visited)
			{
				foundPath = this->hasPathToBlockOfType(targetIndex, targetBlockType, visitedList);
				if (foundPath)
				{
					break;
				}
			}
		}
	}

	return foundPath;
}

//this function checks whether the selected location connects to a block
bool PlatformerCharacter::IsValidBlockAddLocation(int x, int y) const
{
	const int nX = x + this->gridWidth / 2;
	const int nY = y + this->gridWidth / 2;

	struct tempCoordStruct { int x, y; };

	const tempCoordStruct targetCoords[8] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };

	//return false if coordinate is already occupied.
	if (nX < 0 || nY < 0 || nX >= this->gridWidth || nY >= this->gridWidth || (this->blockGrid[nX][nY] >= 0 && this->blockList[this->blockGrid[nX][nY]].isAlive))
	{
		return false;
	}

	for (int a = 0; a < 8; ++a)
	{
		const int xLoc = nX + targetCoords[a].x;
		const int yLoc = nY + targetCoords[a].y;

		if (xLoc > 0 && yLoc > 0 && xLoc < this->gridWidth - 1 && yLoc < this->gridWidth - 1 && this->blockGrid[xLoc][yLoc] >= 0 && this->blockList[this->blockGrid[xLoc][yLoc]].isAlive)
		{
			return true;
		}
	}

	return false;
}

//this function checks whether the selected location connects to a block
bool PlatformerCharacter::IsValidBlockAddLocation(const XMVECTOR& pos) const
{
	XMFLOAT4 blockPos;
	XMStoreFloat4(&blockPos, XMVectorDivide(XMVectorSubtract(pos, this->GetLocation()), XMVectorReplicate(this->parentBlockUniverse->GetGridSpacing())));
	const int xPos = static_cast<int>(blockPos.x);
	const int yPos = static_cast<int>(blockPos.y);

	return this->IsValidBlockAddLocation(xPos, yPos);
}

//this function checks whether the selected location is a block that can be replaced
bool PlatformerCharacter::IsValidBlockReplaceLocation(const XMVECTOR& pos) const
{
	XMFLOAT4 blockPos;
	XMStoreFloat4(&blockPos, XMVectorDivide(XMVectorSubtract(pos, this->GetLocation()), XMVectorReplicate(this->parentBlockUniverse->GetGridSpacing())));
	const int xPos = static_cast<int>(blockPos.x + (blockPos.x > 0 ? .5f : -.5f)) + this->gridWidth / 2;
	const int yPos = static_cast<int>(blockPos.y + (blockPos.y > 0 ? .5f : -.5f)) + this->gridWidth / 2;
	//const int xPos = static_cast<int>(blockPos.x) + this->gridWidth / 2;
	//const int yPos = static_cast<int>(blockPos.y) + this->gridWidth / 2;
	
	return xPos >= 0 && yPos >= 0 &&
		xPos < this->gridWidth && yPos < this->gridWidth &&
		this->blockGrid[xPos][yPos] >= 0 &&
		this->blockList[this->blockGrid[xPos][yPos]].blockType == this->selectedBlockType &&
		this->blockList[this->blockGrid[xPos][yPos]].blockLevel < this->maxBlockLevel;// this->inventoryMap.at(this->selectedBlockType).blockType;
}

//deactivate this block in the world
void PlatformerCharacter::deactivateBlock(int blockIndex)
{
	const XMVECTOR newLoc = XMVectorAdd(this->location, XMVectorMultiply(XMVectorSet(static_cast<float>(this->blockList[blockIndex].x), static_cast<float>(this->blockList[blockIndex].y), 0, 0), XMVectorReplicate(this->GetParentBlockUniverse()->GetGridSpacing())));
	XMFLOAT4 newLocF;
	XMStoreFloat4(&newLocF, newLoc);
	const int blockX = static_cast<int>(newLocF.x / this->GetParentBlockUniverse()->GetGridSpacing());
	const int blockY = static_cast<int>(newLocF.y / this->GetParentBlockUniverse()->GetGridSpacing());
	if (this->parentBlockUniverse->GetBlockOwner(blockX, blockY) == this->GetGlobalID())
	{
		this->parentBlockUniverse->SetBlockType(blockX, blockY, 0, -1, -1); //Set grid square unoccupied 
	}
}

void PlatformerCharacter::DeactivateBlocks()
{
	for (int a = 0; a < this->blockList.size(); ++a)
	{
		this->deactivateBlock(a);
	}
}

//notify the world of the location of each block on this character
void PlatformerCharacter::RefreshWorldBlockLocations()
{
	for (int a = 0; a < this->blockList.size(); ++a)
	{
		const XMVECTOR newLoc = XMVectorAdd(this->location, XMVectorMultiply(XMVectorSet(static_cast<float>(this->blockList[a].x), static_cast<float>(this->blockList[a].y), 0, 0), XMVectorReplicate(this->GetParentBlockUniverse()->GetGridSpacing())));
		XMFLOAT4 newLocF;
		XMStoreFloat4(&newLocF, newLoc);
		const int blockX = static_cast<int>(newLocF.x / this->GetParentBlockUniverse()->GetGridSpacing());
		const int blockY = static_cast<int>(newLocF.y / this->GetParentBlockUniverse()->GetGridSpacing());
		if (this->parentBlockUniverse->GetBlockType(blockX, blockY) == 0 && this->blockList[a].blockType >= 0)
		{
			this->parentBlockUniverse->SetBlockType(blockX, blockY, this->blockType, this->GetGlobalID(), a); //Set grid square occupied
		}
	}
}

void PlatformerCharacter::FireGuns(const XMVECTOR& targetLocation)
{
	if (this->weaponFrameCount == 0)
	{
		this->weaponFrameCount = this->weaponCooldown;

		const int maxErrorAngle = 20;

		for (int a = 0; a < this->blockTypeList[GUN].size(); ++a)
		{
			if (this->blockList[this->blockTypeList[GUN][a]].isAlive)
			{
				const float errorAngle = maxErrorAngle != 0 ? ((rand() % (maxErrorAngle * 1000) / 1000.f) - maxErrorAngle * .5f) * 3.1415f / 180.f : 0;

				const XMVECTOR newLoc = XMVectorAdd(this->location, XMVectorSet(this->parentBlockUniverse->GetGridSpacing() * this->blockList[this->blockTypeList[GUN][a]].x,
					this->parentBlockUniverse->GetGridSpacing() * this->blockList[this->blockTypeList[GUN][a]].y, 0, 0));


				const XMVECTOR newVel = XMVector3Transform(XMVectorMultiply(XMVector3Normalize(XMVectorSubtract(targetLocation, newLoc)), XMVectorSet(10, 10, 0, 1)), XMMatrixRotationZ(errorAngle));
				PlatformerMissile* newBlock = new PlatformerMissile(this->parentBlockUniverse, this->GetGlobalID(), this->GetGlobalID(), this->owningPlayer, this->blockList[this->blockTypeList[GUN][a]].blockLevel, ObjectParameters(this->parentWorld), MoveableParameters(newLoc, XMVectorReplicate(0), newVel));
			}
		}
	}
}

void PlatformerCharacter::FireGuns(float angle) // in radians
{
	if (this->weaponFrameCount == 0)
	{
		this->weaponFrameCount = this->weaponCooldown;

		for (int a = 0; a < this->blockTypeList[GUN].size(); ++a)
		{
			if (this->blockList[this->blockTypeList[GUN][a]].isAlive)
			{
				const XMVECTOR newLoc = XMVectorAdd(this->location, XMVectorSet(this->parentBlockUniverse->GetGridSpacing() * this->blockList[this->blockTypeList[GUN][a]].x,
					this->parentBlockUniverse->GetGridSpacing() * this->blockList[this->blockTypeList[GUN][a]].y, 0, 0));

				const XMVECTOR newVel = XMVector3Transform(XMVectorSet(10, 0, 0, 1), XMMatrixRotationZ(angle));
				PlatformerMissile* newBlock = new PlatformerMissile(this->parentBlockUniverse, this->GetGlobalID(), this->GetGlobalID(), this->owningPlayer, this->blockList[this->blockTypeList[GUN][a]].blockLevel, ObjectParameters(this->parentWorld), MoveableParameters(newLoc, XMVectorReplicate(0), newVel));
			}
		}
	}
}

bool PlatformerCharacter::IsBlockFoot(int blockIndex) const
{
	const int index = this->GetBlockIndex(this->blockList[blockIndex].x, this->blockList[blockIndex].y - 1);
	return index == -1;
}

void PlatformerCharacter::refreshBlockDeathTimers()
{
	for (int a = 0; a < this->blockList.size(); ++a)
	{
		if (this->blockList[a].deathTimer == 0)
		{
			this->KillBlock(a);
			--this->blockList[a].deathTimer;
		}
		else if (this->blockList[a].deathTimer > 0)
		{
			--this->blockList[a].deathTimer;
		}
	}
}

void PlatformerCharacter::loadBlockModel(const std::string& modelName)
{
	this->blockList = GameResources::BlockModelDefinitions[GameResources::BlockModelResourceMap[modelName]].blockList;
	this->rebuildBlockGrid();
	this->activeBlockCount = this->blockList.size();
	for (int a = 0; a < this->activeBlockCount; ++a)
	{
		this->blockTypeList[this->blockList[a].blockType].push_back(a);
	}
}

void PlatformerCharacter::rebuildBlockGrid()
{
	for (int a = 0; a < this->gridWidth; ++a)
	{
		for (int b = 0; b < this->gridWidth; ++b)
		{
			this->blockGrid[a][b] = -1;
		}
	}

	for (int a = 0; a < this->blockList.size(); ++a)
	{
		const int xInd = this->blockList[a].x + this->gridWidth / 2;
		const int yInd = this->blockList[a].y + this->gridWidth / 2;
		this->blockGrid[xInd][yInd] = a;
	}
}

void PlatformerCharacter::OnBlockCollision(const BlockDefinition& blockDefinitions, const XMVECTOR& collisionLocation)
{

}