#include "pch.h"
#include "PlatformerMissile.h"
#include "BlockUniverse.h"
#include "PlatformerBlock.h"
#include "GameResources.h"
using namespace SimulationEngine;
using namespace DirectX;
using namespace std;

PlatformerMissile::PlatformerMissile(BlockUniverse* parentBlockUniverse, int shooterID, int owningCharacter, int owningPlayer, int projectileLevel, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
	PlatformerCharacter(parentBlockUniverse, false, owningCharacter, owningPlayer, objectParameters, moveableParameters),
	shooterID(shooterID),
	explosionRadius(5 * (projectileLevel + 1)),
	damage(5)
{
	this->objectType = ObjectTypes::PlatformerMissile;
	this->ignoreForceEffects = true;
	this->deathTimer = 50;
}

void PlatformerMissile::Act()
{
	/*
	const XMVECTOR newLoc2 = XMVectorMultiply(XMVectorSet(previousXInd + testIndex, yInd, 0, 0), XMVectorReplicate(this->blockUniverse->GetGridSpacing()));
	PlatformerCharacter* newBlock = new PlatformerCharacter(this->blockUniverse, false, 0, 0, ObjectParameters(this), MoveableParameters(newLoc2));
	*/
	__super::Act();
}

void PlatformerMissile::OnBlockCollision(const BlockDefinition& blockDefinition, const XMVECTOR& collisionLocation)
{
	if (this->shooterID != blockDefinition.ownerID && blockDefinition.blockType != 3)
	{
		if (blockDefinition.ownerID >= 0 && this->GetParentWorld()->IsObjectAlive(blockDefinition.ownerID))
		{
			PlatformerCharacter* target = static_cast<PlatformerCharacter*>(this->GetParentWorld()->GetObjectByGlobalID(blockDefinition.ownerID));
			if (target->GetOwningCharacter() == this->GetOwningCharacter() || (!GameResources::CanDie && target->GetObjectType() == ObjectTypes::PlatformerPersonCharacter))
			{
				return;
			}

			if (target->GetBlockList()[blockDefinition.blockOwnerIndex].blockType != ARMOR)
			{
				vector<int> blockDamageList;
				//vector<int> armorBlocklist;

				for (int a = 0; a < target->GetBlockList().size(); ++a)
				{
					if (target->GetBlockList()[a].isAlive)
					{
						const XMVECTOR blockLocation = XMVectorAdd(target->GetLocation(),
							XMVectorMultiply(XMVectorSet(static_cast<float>(target->GetBlockList()[a].x), static_cast<float>(target->GetBlockList()[a].y), 0, 0), XMVectorReplicate(this->parentBlockUniverse->GetGridSpacing())));

						XMFLOAT4 blockDist;
						XMStoreFloat4(&blockDist, XMVector3Length(XMVectorSubtract(this->GetLocation(), blockLocation)));
						if (blockDist.x <= explosionRadius)
						{
							//if (target->GetBlockList()[a].blockType != ARMOR)
							{
								blockDamageList.push_back(a);
							}
							//else
						{
							//	armorBlocklist.push_back(a);
						}
						}
					}
				}

				/*		vector<int> notProtectedByArmorList;

						const float halfWidth = this->parentBlockUniverse->GetGridSpacing() * .5f;
						XMFLOAT4 missileLocation;
						XMStoreFloat4(&missileLocation, this->GetLocation());

						//check if armor occludes target
						for (int targetIndex = 0; targetIndex < blockDamageList.size(); ++targetIndex)
						{
						const XMVECTOR blockLocation = XMVectorAdd(target->GetLocation(),
						XMVectorMultiply(XMVectorSet(static_cast<float>(target->GetBlockList()[blockDamageList[targetIndex]].x), static_cast<float>(target->GetBlockList()[blockDamageList[targetIndex]].y), 0, 0), XMVectorReplicate(this->parentBlockUniverse->GetGridSpacing())));
						XMFLOAT4 blocLoc;
						XMStoreFloat4(&blocLoc, blockLocation);
						const XMFLOAT4 verts[4] = { XMFLOAT4(blocLoc.x - halfWidth, blocLoc.y - halfWidth, 0, 0),
						XMFLOAT4(blocLoc.x - halfWidth, blocLoc.y + halfWidth, 0, 0),
						XMFLOAT4(blocLoc.x + halfWidth, blocLoc.y + halfWidth, 0, 0),
						XMFLOAT4(blocLoc.x + halfWidth, blocLoc.y - halfWidth, 0, 0) };

						bool isProtected = false;

						for (int armorIndex = 0; armorIndex < armorBlocklist.size(); ++armorIndex)
						{
						XMFLOAT4 armorBlockLocation;
						XMStoreFloat4(&armorBlockLocation, XMVectorAdd(target->GetLocation(),
						XMVectorMultiply(XMVectorSet(static_cast<float>(target->GetBlockList()[armorBlocklist[armorIndex]].x), static_cast<float>(target->GetBlockList()[armorBlocklist[armorIndex]].y), 0, 0), XMVectorReplicate(this->parentBlockUniverse->GetGridSpacing()))));

						const XMFLOAT4 armorVerts[4] = { XMFLOAT4(armorBlockLocation.x - halfWidth, armorBlockLocation.y - halfWidth, 0, 0),
						XMFLOAT4(armorBlockLocation.x - halfWidth, armorBlockLocation.y + halfWidth, 0, 0),
						XMFLOAT4(armorBlockLocation.x + halfWidth, armorBlockLocation.y + halfWidth, 0, 0),
						XMFLOAT4(armorBlockLocation.x + halfWidth, armorBlockLocation.y - halfWidth, 0, 0) };

						bool foundPassingLine = false;

						//iterate through each vertex of the block in question
						for (int targetVertexIndex = 0; targetVertexIndex < 4; ++targetVertexIndex)
						{
						float fXY = (verts[targetVertexIndex].y - missileLocation.y) * armorVerts[0].x +
						(missileLocation.x - verts[targetVertexIndex].x) * armorVerts[0].y +
						(verts[targetVertexIndex].x * missileLocation.y - missileLocation.x * verts[targetVertexIndex].y);

						bool intersected = false;
						for (int armorVertexIndex = 1; armorVertexIndex < 4; ++armorVertexIndex)
						{
						const float nextVal = (verts[targetVertexIndex].y - missileLocation.y) * armorVerts[armorVertexIndex].x +
						(missileLocation.x - verts[targetVertexIndex].x) * armorVerts[armorVertexIndex].y +
						(verts[targetVertexIndex].x * missileLocation.y - missileLocation.x * verts[targetVertexIndex].y);

						if (fXY < 0 && nextVal > 0 || fXY > 0 && nextVal < 0)
						{
						intersected = true;
						break;
						}

						fXY = nextVal;
						}

						if (!intersected)
						{
						foundPassingLine = true;
						break;
						}
						}

						if (!foundPassingLine)
						{
						isProtected = true;
						break;
						}
						}
						if (!isProtected)
						{
						notProtectedByArmorList.push_back(blockDamageList[targetIndex]);
						}
						}*/

				//target->KillBlocks(notProtectedByArmorList);
				target->KillBlocks(blockDamageList);
			}
			else
			{
				this->explode(collisionLocation, 3);
			}
		}
		else
		{
			this->explode(collisionLocation);
		}

		this->Remove();
		this->deactivateBlock(0);
	}
}

void PlatformerMissile::explode(const XMVECTOR& collisionLocation, int count)
{
	for (int a = 0; a < count; ++a)
	{
		const float deathSpeed = (5.f + rand() % 10) * .5f;
		const float deathAngle = (rand() % 360) * 3.1415f / 180;
		const XMVECTOR newLoc = collisionLocation;
		const XMVECTOR newVel = XMVectorSet(cosf(deathAngle) * deathSpeed * .25f, fabs(sinf(deathAngle)) * deathSpeed, 0, 0);
		PlatformerBlock* newBlock = new PlatformerBlock(this->parentBlockUniverse, this->GetGlobalID(), this->owningPlayer, ObjectParameters(this->parentWorld), MoveableParameters(newLoc, XMVectorReplicate(0), newVel));
		newBlock->SetIlluminateVelocity(true);
		newBlock->SetDeathTimer(1000);
	}
}