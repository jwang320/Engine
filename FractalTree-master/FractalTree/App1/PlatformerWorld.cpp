#include "pch.h"
#include "PlatformerWorld.h"
#include "BlockUniverse.h"
#include "PlatformerCharacter.h"
#include "..\Src\UIRenderer.h"
#include <string>

using namespace DirectX;
using namespace SimulationEngine;

static int globalIDIndex = -1;

PlatformerWorld::PlatformerWorld() :
	blockUniverse(NULL)
{
	if (globalIDIndex != -1)
	{
		RenderingEngine::UIRenderer::HideDebugString(globalIDIndex);
	}

	globalIDIndex = RenderingEngine::UIRenderer::AddDebugString("Max Global ID: ");
}

//Update is called once per frame.
void PlatformerWorld::Update()
{
	//this->writeObjectBlocks();

	__super::Update();

	if (this->active)
	{
		this->applyPathing();
	}

	RenderingEngine::UIRenderer::SetDebugString(globalIDIndex, "Max Global ID: " + std::to_string(this->nextAvailableGlobalID));
}

#undef GetObject
void PlatformerWorld::applyPathing()
{
	for (int a = 0; a < this->GetObjectCount(); ++a)
	{
		Actor* currentObject = this->GetObject(a);
		if ((this->objectList[a]->GetComponentFlags() & PhysicalWorld::PLATFORMER) > 0)
		{
			this->CheckPlatformerCollision(static_cast<PlatformerCharacter*>(currentObject));		
		}		
	}
}

void PlatformerWorld::writeObjectBlocks()
{
	for (int a = 0; a < this->GetObjectCount(); ++a)
	{
		Actor* currentObject = this->GetObject(a);
		if ((this->objectList[a]->GetComponentFlags() & PhysicalWorld::PLATFORMER) > 0)
		{
			this->writeObjectBlock(static_cast<PlatformerCharacter*>(currentObject));
		}
	}
}

void PlatformerWorld::writeObjectBlock(const PlatformerCharacter* platformerCharacter)
{
	for (int a = 0; a < platformerCharacter->GetBlockList().size(); ++a)
	{
		const XMVECTOR newLoc = XMVectorAdd(platformerCharacter->GetLocation(), XMVectorMultiply(XMVectorSet(static_cast<float>(platformerCharacter->GetBlockList()[a].x), static_cast<float>(platformerCharacter->GetBlockList()[a].y), 0, 0), XMVectorReplicate(this->blockUniverse->GetGridSpacing())));
		XMFLOAT4 newLocF;
		XMStoreFloat4(&newLocF, newLoc);
		const int blockX = static_cast<int>(newLocF.x / this->blockUniverse->GetGridSpacing());
		const int blockY = static_cast<int>(newLocF.y / this->blockUniverse->GetGridSpacing());
		if (this->blockUniverse->GetBlockOwner(blockX, blockY) == platformerCharacter->GetGlobalID())
		{
			this->blockUniverse->SetBlockType(blockX, blockY, 0, -1, -1); //Set grid square unoccupied 
		}
	}

	for (int a = 0; a < platformerCharacter->GetBlockList().size(); ++a)
	{
		const XMVECTOR newLoc = XMVectorAdd(platformerCharacter->GetLocation(), XMVectorMultiply(XMVectorSet(static_cast<float>(platformerCharacter->GetBlockList()[a].x), static_cast<float>(platformerCharacter->GetBlockList()[a].y), 0, 0), XMVectorReplicate(this->blockUniverse->GetGridSpacing())));
		XMFLOAT4 newLocF;
		XMStoreFloat4(&newLocF, newLoc);
		const int blockX = static_cast<int>(newLocF.x / this->blockUniverse->GetGridSpacing());
		const int blockY = static_cast<int>(newLocF.y / this->blockUniverse->GetGridSpacing());
		if (this->blockUniverse->GetBlockOwner(blockX, blockY) == platformerCharacter->GetGlobalID())
		{
			this->blockUniverse->SetBlockType(blockX, blockY, platformerCharacter->GetBlockType(), platformerCharacter->GetGlobalID(), a); //Set grid square occupied
		}
	}
}

bool PlatformerWorld::CheckPlatformerCollision(PlatformerCharacter* currentObject)
{
	//XMVECTOR loc = XMVectorAdd(currentObject->GetLocation(), currentObject->GetVelocity());
	XMVECTOR loc = currentObject->GetLocation();
	XMFLOAT4 newLoc;
	XMFLOAT4 scale;
	XMStoreFloat4(&newLoc, loc);
	XMStoreFloat4(&scale, currentObject->GetScale());

	const float offset = 0;
	const int xIndCenter = static_cast<int>((newLoc.x) / this->blockUniverse->GetGridSpacing() + offset);
	const int yIndCenter = static_cast<int>(newLoc.y / this->blockUniverse->GetGridSpacing() + offset);

	XMFLOAT4 previousLocation;
	XMStoreFloat4(&previousLocation, currentObject->GetPreviousLocation());//currentObject->GetPreviousLocation());

	const int previousXIndCenter = static_cast<int>((previousLocation.x) / this->blockUniverse->GetGridSpacing() + offset);
	const int previousYIndCenter = static_cast<int>((previousLocation.y) / this->blockUniverse->GetGridSpacing() + offset);

	const int passThroughXCount = abs(xIndCenter - previousXIndCenter);
	const int passThroughYCount = abs(yIndCenter - previousYIndCenter);

	const float minSpeed = .000001f;
	bool foundCollision = false;
	XMFLOAT4 vel;
	XMStoreFloat4(&vel, currentObject->GetVelocity());
	if (fabs(vel.x) <= minSpeed)
	{
		vel.x = 0;
		currentObject->SetVelocity(XMLoadFloat4(&vel));
	}

	for (int a = 0; a < currentObject->GetBlockList().size(); ++a)
	{
		if (!currentObject->GetBlockList()[a].isAlive)
			continue;


		//Figure out which part of the platform grid the square's center overlaps
		const int xInd = xIndCenter + currentObject->GetBlockList()[a].x;
		const int yInd = yIndCenter + currentObject->GetBlockList()[a].y;

		const int previousXInd = previousXIndCenter + currentObject->GetBlockList()[a].x;
		const int previousYInd = previousYIndCenter + currentObject->GetBlockList()[a].y;

		//Make sure the square's center is not outside the platform grid
		if (xInd >= 0 && yInd >= 0 && xInd < this->blockUniverse->GetWidth() && yInd < this->blockUniverse->GetHeight())
		{
			//moving down
			if (vel.y < 0)
			{
				for (int testIndex = 0; testIndex < max(1, passThroughYCount); ++testIndex)
				{
					const int newBlockType = this->blockUniverse->GetBlockType(previousXInd, previousYInd - testIndex - 1);
					if (newBlockType == BlockUniverse::REGULAR)
					{
						if (!currentObject->IsRested())
						{
							vel.y = 0;
							newLoc.y = (previousYIndCenter - testIndex) * this->blockUniverse->GetGridSpacing();
						}

						currentObject->SetVelocity(XMLoadFloat4(&vel));
						currentObject->SetRested(true);
						currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(xInd, previousYInd - testIndex - 1), XMLoadFloat4(&newLoc));
						foundCollision = true;
					}
				}
			}
			else if (vel.y == 0)
			{
				const int newBlockType = this->blockUniverse->GetBlockType(previousXInd, yInd - 1);
				if (newBlockType == BlockUniverse::REGULAR)
				{
					foundCollision = true;
					if (currentObject->UseMovementFriction())
					{
						vel.x *= .98f;
						currentObject->SetVelocity(XMLoadFloat4(&vel));
					}
				}
			}
			else //moving up
			{
				const int newBlockType = this->blockUniverse->GetBlockType(xInd, yInd + 1);
				if (newBlockType == BlockUniverse::REGULAR)
				{
					vel.y = 0;
					newLoc.y = (yIndCenter) * this->blockUniverse->GetGridSpacing();
					currentObject->SetVelocity(XMLoadFloat4(&vel));
					currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(xInd, yInd + 1), XMLoadFloat4(&newLoc));
					foundCollision = true;
				}
			}

			//moving right
			if (vel.x > 0)
			{
				//calculate blocks passed through since last frame (because block might be moving faster than block size

				//iterate through blocks passed through since last frame

				for (int testIndex = 0; testIndex < max(1, passThroughXCount); ++testIndex)
				{
					const int newBlockType = this->blockUniverse->GetBlockType(previousXInd + testIndex + 1, yInd);
					if (newBlockType == BlockUniverse::REGULAR)
					{
						const int aboveBlockType = this->blockUniverse->GetBlockType(previousXInd + testIndex + 1, yInd + 1);
						if (currentObject->CanWalkStairs() && aboveBlockType != BlockUniverse::REGULAR && currentObject->IsBlockFoot(a))
						{
							//vel.x = 0;
							newLoc.y = (yIndCenter + 1) * this->blockUniverse->GetGridSpacing();
							foundCollision = true;
						}
						else
						{
							vel.x = 0;
							newLoc.x = (previousXIndCenter + testIndex) * this->blockUniverse->GetGridSpacing();
							currentObject->SetVelocity(XMLoadFloat4(&vel));
							currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd + testIndex + 1, yInd), XMLoadFloat4(&newLoc));
							foundCollision = true;
						}
					}
					else if (newBlockType > 0)
					{
						const int blockOwner = this->blockUniverse->GetBlockOwner(previousXInd + testIndex + 1, yInd);
						if (blockOwner >= 0 && blockOwner != currentObject->GetGlobalID() && this->IsObjectAlive(blockOwner))
						{
							PlatformerCharacter* owner = static_cast<PlatformerCharacter*>(this->GetObjectByGlobalID(blockOwner));
							int blockIndex = this->blockUniverse->GetBlockOwnerIndex(previousXInd + testIndex + 1, yInd);
							if (owner->GetBlockList()[blockIndex].isAlive)
							{
								currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd + testIndex + 1, yInd), XMLoadFloat4(&newLoc));
								//owner->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd + testIndex + 1, yInd), XMLoadFloat4(&previousLocation));
							}
						}
					}
				}
			}
			//moving left
			else if (vel.x < 0 && xInd - 1 >= 0)
			{
				for (int testIndex = 0; testIndex < passThroughXCount; ++testIndex)
				{
					/*if (!currentObject->CanWalkStairs())
					{
					const XMVECTOR newLoc2 = XMVectorMultiply(XMVectorSet(previousXInd + testIndex, yInd, 0, 0), XMVectorReplicate(this->blockUniverse->GetGridSpacing()));
					PlatformerCharacter* newBlock = new PlatformerCharacter(this->blockUniverse, false, 0, 0, ObjectParameters(this), MoveableParameters(newLoc2));
					}*/

					const int newBlockType = this->blockUniverse->GetBlockType(previousXInd - testIndex - 1, yInd);
					if (newBlockType == BlockUniverse::REGULAR)
					{
						const int aboveBlockType = this->blockUniverse->GetBlockType(previousXInd - testIndex - 1, yInd + 1);
						if (currentObject->CanWalkStairs() && aboveBlockType != BlockUniverse::REGULAR && currentObject->IsBlockFoot(a))
						{
							//vel.x = 0;
							newLoc.y = (yIndCenter + 1) * this->blockUniverse->GetGridSpacing();
							foundCollision = true;
						}
						else
						{
							vel.x = 0;
							newLoc.x = (previousXIndCenter - testIndex) * this->blockUniverse->GetGridSpacing();
							currentObject->SetVelocity(XMLoadFloat4(&vel));
							currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd - testIndex, yInd), XMLoadFloat4(&newLoc));
							foundCollision = true;
						}
					}
					else if (newBlockType > 0)
					{
						const int blockOwner = this->blockUniverse->GetBlockOwner(previousXInd - testIndex - 1, yInd);
						if (blockOwner >= 0 && blockOwner != currentObject->GetGlobalID() && this->IsObjectAlive(blockOwner))
						{
							PlatformerCharacter* owner = static_cast<PlatformerCharacter*>(this->GetObjectByGlobalID(blockOwner));
							int blockIndex = this->blockUniverse->GetBlockOwnerIndex(previousXInd - testIndex - 1, yInd);
							if (owner->GetBlockList()[blockIndex].isAlive)
							{
								currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd - testIndex - 1, yInd), XMLoadFloat4(&newLoc));
								//owner->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd - testIndex - 1, yInd), XMLoadFloat4(&previousLocation));
							}
						}
					}
				}
			}
		}
	}

	if (!foundCollision)
	{
		currentObject->SetRested(false);
	}
	else
	{
		currentObject->DeactivateBlocks();
		currentObject->SetLocation(XMLoadFloat4(&newLoc));
		currentObject->RefreshWorldBlockLocations();
		return true;
	}

	return false;
}

/*bool PlatformerWorld::CheckPlatformerCollision(PlatformerCharacter* currentObject)
{
	XMVECTOR loc = XMVectorAdd(currentObject->GetLocation(), currentObject->GetVelocity());
	XMFLOAT4 newLoc;
	XMFLOAT4 scale;
	XMStoreFloat4(&newLoc, loc);
	XMStoreFloat4(&scale, currentObject->GetScale());

	const float offset = 0;
	const int xIndCenter = static_cast<int>((newLoc.x) / this->blockUniverse->GetGridSpacing() + offset);
	const int yIndCenter = static_cast<int>(newLoc.y / this->blockUniverse->GetGridSpacing() + offset);

	XMFLOAT4 previousLocation;
	XMStoreFloat4(&previousLocation, currentObject->GetLocation());//currentObject->GetPreviousLocation());

	const int previousXIndCenter = static_cast<int>((previousLocation.x) / this->blockUniverse->GetGridSpacing() + offset);
	const int previousYIndCenter = static_cast<int>((previousLocation.y) / this->blockUniverse->GetGridSpacing() + offset);

	const int passThroughXCount = abs(xIndCenter - previousXIndCenter);
	const int passThroughYCount = abs(yIndCenter - previousYIndCenter);

	const float minSpeed = .000001f;
	bool foundCollision = false;
	XMFLOAT4 vel;
	XMStoreFloat4(&vel, currentObject->GetVelocity());
	if (fabs(vel.x) <= minSpeed)
	{
		vel.x = 0;
		currentObject->SetVelocity(XMLoadFloat4(&vel));
	}
	
	for (int a = 0; a < currentObject->GetBlockList().size(); ++a)
	{
		if (!currentObject->GetBlockList()[a].isAlive)
			continue;


		//Figure out which part of the platform grid the square's center overlaps
		const int xInd = xIndCenter + currentObject->GetBlockList()[a].x;
		const int yInd = yIndCenter + currentObject->GetBlockList()[a].y;

		const int previousXInd = previousXIndCenter + currentObject->GetBlockList()[a].x;
		const int previousYInd = previousYIndCenter + currentObject->GetBlockList()[a].y;

		//Make sure the square's center is not outside the platform grid
		if (xInd >= 0 && yInd >= 0 && xInd < this->blockUniverse->GetWidth() && yInd < this->blockUniverse->GetHeight())
		{
			//moving down
			if (vel.y < 0)
			{
				for (int testIndex = 0; testIndex < max(1, passThroughYCount); ++testIndex)
				{
					const int newBlockType = this->blockUniverse->GetBlockType(previousXInd, previousYInd - testIndex - 1);
					if (newBlockType == BlockUniverse::REGULAR)
					{
						if (!currentObject->IsRested())
						{
							vel.y = 0;
							previousLocation.y = (previousYIndCenter - testIndex) * this->blockUniverse->GetGridSpacing();
						}

						currentObject->SetVelocity(XMLoadFloat4(&vel));
						currentObject->SetRested(true);
						currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(xInd, previousYInd - testIndex - 1), XMLoadFloat4(&newLoc));
						foundCollision = true;
					}
				}
			}
			else if (vel.y == 0)
			{
				const int newBlockType = this->blockUniverse->GetBlockType(previousXInd, yInd - 1);
				if (newBlockType == BlockUniverse::REGULAR)
				{
					foundCollision = true;
					if (currentObject->UseMovementFriction())
					{
						vel.x *= .98f;
						currentObject->SetVelocity(XMLoadFloat4(&vel));
					}
				}
			}
			else //moving up
			{
				const int newBlockType = this->blockUniverse->GetBlockType(xInd, yInd + 1);
				if (newBlockType == BlockUniverse::REGULAR)
				{
					vel.y = 0;
					previousLocation.y = (yIndCenter) * this->blockUniverse->GetGridSpacing();
					currentObject->SetVelocity(XMLoadFloat4(&vel));
					currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(xInd, yInd + 1), XMLoadFloat4(&newLoc));
					foundCollision = true;
				}
			}

			//moving right
			if (vel.x > 0)
			{
				//calculate blocks passed through since last frame (because block might be moving faster than block size

				//iterate through blocks passed through since last frame

				for (int testIndex = 0; testIndex < max(1, passThroughXCount); ++testIndex)
				{
					const int newBlockType = this->blockUniverse->GetBlockType(previousXInd + testIndex + 1, yInd);
					if (newBlockType == BlockUniverse::REGULAR)
					{
						const int aboveBlockType = this->blockUniverse->GetBlockType(previousXInd + testIndex + 1, yInd + 1);
						if (currentObject->CanWalkStairs() && aboveBlockType != BlockUniverse::REGULAR && currentObject->IsBlockFoot(a))
						{
							//vel.x = 0;
							previousLocation.y = (yIndCenter + 1) * this->blockUniverse->GetGridSpacing();
							foundCollision = true;
						}
						else
						{
							vel.x = 0;
							previousLocation.x = (previousXIndCenter + testIndex) * this->blockUniverse->GetGridSpacing();
							currentObject->SetVelocity(XMLoadFloat4(&vel));
							currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd + testIndex + 1, yInd), XMLoadFloat4(&previousLocation));
							foundCollision = true;
						}
					}
					else if (newBlockType > 0)
					{
						const int blockOwner = this->blockUniverse->GetBlockOwner(previousXInd + testIndex + 1, yInd);
						if (blockOwner >= 0 && blockOwner != currentObject->GetGlobalID() && this->IsObjectAlive(blockOwner))
						{
							PlatformerCharacter* owner = static_cast<PlatformerCharacter*>(this->GetObjectByGlobalID(blockOwner));
							int blockIndex = this->blockUniverse->GetBlockOwnerIndex(previousXInd + testIndex + 1, yInd);
							if (owner->GetBlockList()[blockIndex].isAlive)
							{
								currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd + testIndex + 1, yInd), XMLoadFloat4(&previousLocation));
								//owner->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd + testIndex + 1, yInd), XMLoadFloat4(&previousLocation));
							}
						}
					}
				}
			}
			//moving left
			else if (vel.x < 0 && xInd - 1 >= 0)
			{
				for (int testIndex = 0; testIndex < passThroughXCount; ++testIndex)
				{
					
					
					const int newBlockType = this->blockUniverse->GetBlockType(previousXInd - testIndex - 1, yInd);
					if (newBlockType == BlockUniverse::REGULAR)
					{
						const int aboveBlockType = this->blockUniverse->GetBlockType(previousXInd - testIndex - 1, yInd + 1);
						if (currentObject->CanWalkStairs() && aboveBlockType != BlockUniverse::REGULAR && currentObject->IsBlockFoot(a))
						{
							//vel.x = 0;
							previousLocation.y = (yIndCenter + 1) * this->blockUniverse->GetGridSpacing();
							foundCollision = true;
						}
						else
						{
							vel.x = 0;
							previousLocation.x = (previousXIndCenter - testIndex) * this->blockUniverse->GetGridSpacing();
							currentObject->SetVelocity(XMLoadFloat4(&vel));
							currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd - testIndex, yInd), XMLoadFloat4(&previousLocation));
							foundCollision = true;
						}
					}
					else if (newBlockType > 0)
					{
						const int blockOwner = this->blockUniverse->GetBlockOwner(previousXInd - testIndex - 1, yInd);
						if (blockOwner >= 0 && blockOwner != currentObject->GetGlobalID() && this->IsObjectAlive(blockOwner))
						{
							PlatformerCharacter* owner = static_cast<PlatformerCharacter*>(this->GetObjectByGlobalID(blockOwner));
							int blockIndex = this->blockUniverse->GetBlockOwnerIndex(previousXInd - testIndex - 1, yInd);
							if (owner->GetBlockList()[blockIndex].isAlive)
							{
								currentObject->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd - testIndex - 1, yInd), XMLoadFloat4(&previousLocation));
								//owner->OnBlockCollision(this->blockUniverse->GetBlockDefinition(previousXInd - testIndex - 1, yInd), XMLoadFloat4(&previousLocation));
							}
						}
					}
				}				
			}
		}
	}

	if (!foundCollision)
	{
		currentObject->SetRested(false);
	}

	currentObject->SetLocation(XMLoadFloat4(&previousLocation));
	return false;
}*/
