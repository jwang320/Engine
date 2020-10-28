#include "pch.h"
#include "FractalTreeWorld.h"
#include "..\FractalTree Common\Game Objects\Objects\Terrain.h"

using namespace DirectX;

namespace SimulationEngine
{
	FractalTreeWorld::FractalTreeWorld() :
		terrain(NULL)
	{

	}

	void FractalTreeWorld::Update()
	{
		if (FractalTreeResources::isPaused)
		{
			return;
		}

		if(this->active)
		{
			this->ApplyPathing();
		}

		__super::Update();
	}

	void FractalTreeWorld::ApplyPathing()
	{
		for(int a = 0; a < this->GetObjectCount(); ++a)
		{
			if((this->objectList[a]->GetComponentFlags() & PhysicalWorld::PATH) > 0)
			{
				XMVECTOR loc = this->GetObject(a)->location;
				XMFLOAT4 newLoc;
				XMStoreFloat4(&newLoc, loc);
				if(this->terrain != NULL)
				{
					const float terrainHeight = this->terrain->GetHeight(newLoc.x, newLoc.z);
					if(newLoc.y - terrainHeight < 1.f)
					{
						newLoc.y = terrainHeight;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;
						this->GetObject(a)->resting = true;
						const XMVECTOR drag = XMVectorMultiply(this->GetObject(a)->velocity, XMVectorReplicate(this->GetObject(a)->mass * -.1f));
						this->GetObject(a)->ApplyForceAtPoint(this->GetObject(a)->location, drag);
					}
					else
					{
						this->GetObject(a)->resting = false;
					}
				}
				else
				{
					if(newLoc.y < 0)
					{
						newLoc.y = 0;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;
						this->GetObject(a)->resting = true;
						const XMVECTOR drag = XMVectorMultiply(this->GetObject(a)->velocity, XMVectorReplicate(this->GetObject(a)->mass * -.1f));
						this->GetObject(a)->ApplyForceAtPoint(this->GetObject(a)->location, drag);
					}
					else
					{
						this->GetObject(a)->resting = false;
					}
				}
			}
			else if((this->objectList[a]->GetComponentFlags() & PhysicalWorld::PHYSICS) > 0)
			{

				Actor* obj = this->GetObject(a);
				XMVECTOR loc = obj->location;
				XMVECTOR scale = obj->GetScale();
				XMFLOAT4 newLoc;
				XMFLOAT4 newScale;
				XMStoreFloat4(&newLoc, loc);
				XMStoreFloat4(&newScale, scale);

				//Calculate the drag
				XMFLOAT4 velF;
				XMStoreFloat4(&velF, obj->velocity);
				const XMVECTOR signVec = XMVectorSet(velF.x == 0 ? 0 : (velF.x < 0 ? -1.f : 1.f), velF.y == 0 ? 0 : (velF.y < 0 ? -1.f : 1.f), velF.z == 0 ? 0 : (velF.z < 0 ? -1.f : 1.f), 0);

				/*const XMVECTOR dragForce = XMVectorMultiply(XMVectorMultiply(XMVectorReplicate(-.5f *.5f), XMVectorMultiply(obj->GetVelocity(), obj->GetVelocity())),
					signVec);*/
				//const XMVECTOR dragForce = XMVectorMultiply(XMVectorReplicate(-obj->GetDragCoefficient()), obj->GetVelocity());
				//obj->ApplyForceAtPoint(obj->GetLocation(), XMVectorMultiply(dragForce, XMVectorReplicate(obj->GetMass())));
				
				float radius = newScale.x;

				if(this->terrain != NULL/* && !obj->IgnoreForceEffects()*/)
				{
					float terrainHeight = this->terrain->GetHeight(newLoc.x, newLoc.z);
					if(newLoc.y - terrainHeight - radius < 0)
					{
						newLoc.y = terrainHeight + radius;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;

						const float h0 = this->terrain->GetHeight(newLoc.x + 0, newLoc.z - 1);
						const float h1 = this->terrain->GetHeight(newLoc.x - 1, newLoc.z + 0);
						const float h2 = this->terrain->GetHeight(newLoc.x + 1, newLoc.z + 0);
						const float h3 = this->terrain->GetHeight(newLoc.x + 0, newLoc.z + 1);

						const XMVECTOR normal = XMVector3Normalize(XMVectorSet(h1 - h2, 2, h0 - h3, 0));
						const XMVECTOR dotProduct = XMVector3Dot(normal, obj->velocity);
						XMFLOAT4 dotP;
						XMStoreFloat4(&dotP, dotProduct);
						if(dotP.x > 0)
						{

						}
						else
						{
							XMVECTOR normalForce = XMVectorMultiply(normal, obj->velocity);
							normalForce = XMVectorMultiply(normal, XMVectorReplicate(-2 * obj->mass * dotP.x * .94f));
							obj->SetVelocity(XMVectorMultiply(obj->velocity, XMVectorReplicate(.9f)));
							obj->ApplyForceAtPoint(loc, normalForce);
							const CollisionData collisionData = {NULL, loc, normal};
							this->GetObject(a)->OnCollision(collisionData);
						}
					}
					else
					{
						//this->GetObject(a)->SetRested(false);
					}
				}
				else
				{
					if(newLoc.y - radius < 0)
					{
						newLoc.y = radius;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;

						const XMVECTOR normal = XMVectorSet(0, 1, 0, 0);
						const XMVECTOR dotProduct = XMVector3Dot(normal, obj->velocity);
						XMFLOAT4 dotP;
						XMStoreFloat4(&dotP, dotProduct);
						if(dotP.x > 0)
						{

						}
						else
						{
							XMVECTOR normalForce = XMVectorMultiply(normal, obj->velocity);
							const float aaas = this->GetObject(a)->mass * -.1f;
							const XMVECTOR drag = XMVectorMultiply(this->GetObject(a)->velocity, XMVectorSet(aaas, 0, aaas, 0));
							this->GetObject(a)->ApplyForceAtPoint(this->GetObject(a)->location, drag);
							normalForce = XMVectorMultiply(normal, XMVectorReplicate(-2 * obj->mass * dotP.x * damping));
							//obj->SetVelocity(XMVectorMultiply(obj->GetVelocity(), XMVectorReplicate(.9f)));
							obj->ApplyForceAtPoint(XMVectorAdd(loc, XMVectorSet(0, -radius, 0, 0)), normalForce);

							const CollisionData collisionData = {NULL, loc, normal};
							this->GetObject(a)->OnCollision(collisionData);
						}
					}
					else
					{
						this->GetObject(a)->resting = false;
					}

					const float boundWidth = 2000000;

					if(newLoc.x - radius < -boundWidth)
					{
						newLoc.x = radius + -boundWidth;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;

						const XMVECTOR normal = XMVectorSet(1, 0, 0, 0);
						const XMVECTOR dotProduct = XMVector3Dot(normal, obj->velocity);
						XMFLOAT4 dotP;
						XMStoreFloat4(&dotP, dotProduct);
						if(dotP.x > 0)
						{

						}
						else
						{
							XMVECTOR normalForce = XMVectorMultiply(normal, obj->velocity);
							normalForce = XMVectorMultiply(normal, XMVectorReplicate(-2 * obj->mass * dotP.x * damping));
					//		obj->SetVelocity(XMVectorMultiply(obj->GetVelocity(), XMVectorReplicate(.9f)));
							obj->ApplyForceAtPoint(loc, normalForce);
							const CollisionData collisionData = {NULL, loc, normal};
							this->GetObject(a)->OnCollision(collisionData);
						}
					}
					else if(newLoc.x + radius > boundWidth)
					{
						newLoc.x = boundWidth - radius;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;

						const XMVECTOR normal = XMVectorSet(-1, 0, 0, 0);
						const XMVECTOR dotProduct = XMVector3Dot(normal, obj->velocity);
						XMFLOAT4 dotP;
						XMStoreFloat4(&dotP, dotProduct);
						if(dotP.x > 0)
						{

						}
						else
						{
							XMVECTOR normalForce = XMVectorMultiply(normal, obj->velocity);
							normalForce = XMVectorMultiply(normal, XMVectorReplicate(-2 * obj->mass * dotP.x * damping));
					//		obj->SetVelocity(XMVectorMultiply(obj->GetVelocity(), XMVectorReplicate(.9f)));
							obj->ApplyForceAtPoint(loc, normalForce);
							const CollisionData collisionData = {NULL, loc, normal};
							this->GetObject(a)->OnCollision(collisionData);
						}
					}

					if(newLoc.z - radius < -boundWidth)
					{
						newLoc.z = radius + -boundWidth;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;

						const XMVECTOR normal = XMVectorSet(0, 0, 1, 0);
						const XMVECTOR dotProduct = XMVector3Dot(normal, obj->velocity);
						XMFLOAT4 dotP;
						XMStoreFloat4(&dotP, dotProduct);
						if(dotP.x > 0)
						{
						}
						else
						{
							XMVECTOR normalForce = XMVectorMultiply(normal, obj->velocity);
							normalForce = XMVectorMultiply(normal, XMVectorReplicate(-2 * obj->mass * dotP.x * damping));
						//	obj->SetVelocity(XMVectorMultiply(obj->GetVelocity(), XMVectorReplicate(.9f)));
							obj->ApplyForceAtPoint(loc, normalForce);
							const CollisionData collisionData = {NULL, loc, normal};
							this->GetObject(a)->OnCollision(collisionData);
						}
					}
					else if(newLoc.z + radius > boundWidth)
					{
						newLoc.z = boundWidth - radius;
						loc = XMLoadFloat4(&newLoc);
						this->GetObject(a)->location = loc;

						const XMVECTOR normal = XMVectorSet(0, 0, -1, 0);
						const XMVECTOR dotProduct = XMVector3Dot(normal, obj->velocity);
						XMFLOAT4 dotP;
						XMStoreFloat4(&dotP, dotProduct);
						if(dotP.x > 0)
						{
							
						}
						else
						{
							XMVECTOR normalForce = XMVectorMultiply(normal, obj->velocity);
							normalForce = XMVectorMultiply(normal, XMVectorReplicate(-2 * obj->mass * dotP.x * damping));
					//		obj->SetVelocity(XMVectorMultiply(obj->GetVelocity(), XMVectorReplicate(.9f)));
							obj->ApplyForceAtPoint(loc, normalForce);
							const CollisionData collisionData = {NULL, loc, normal};
							this->GetObject(a)->OnCollision(collisionData);
						}
					}
				}

				if (obj->ignorePhysics)
				{
					continue;
				}

				for(int b = a + 1; b < this->GetObjectCount(); ++b)
				{
					if((this->objectList[b]->GetComponentFlags() & PhysicalWorld::PHYSICS) > 0)
					{
						Actor* objB = this->GetObject(b);
						if (objB->ignorePhysics)
						{
							continue;
						}
						CollisionResult collisionResult;

						if(this->CheckCollision(collisionResult, obj, objB))
						{
							const float aMass = obj->mass;
							const float bMass = objB->mass;

							//Calculate the positions needed to move the objects apart so that they are no longer penetrating;
							//move the heavier object less distance.
							const XMVECTOR newLocA = XMVectorSubtract(obj->location,
								XMVectorMultiply(collisionResult.collisionNormal, XMVectorReplicate(collisionResult.collisionDistance*( bMass / (aMass + bMass)))));

							const XMVECTOR newLocB = XMVectorAdd(objB->location,
								XMVectorMultiply(collisionResult.collisionNormal, XMVectorReplicate(collisionResult.collisionDistance*( aMass / (aMass + bMass)))));
							

							const float massRatio = .95f / (aMass + bMass);

							//Calculate the velocity from each object along the collision normal -- this velocity is transferred to the other
							//object during the collision
							const XMVECTOR velocityFromA = XMVectorMultiply(XMVectorMultiply(-collisionResult.collisionNormal, XMVector3Dot(obj->velocity, -collisionResult.collisionNormal)), XMVectorReplicate(bMass * massRatio));
							const XMVECTOR velocityFromB = XMVectorMultiply(XMVectorMultiply(collisionResult.collisionNormal, XMVector3Dot(objB->velocity, collisionResult.collisionNormal)), XMVectorReplicate(aMass * massRatio));

							//Calculate the final change in velocity of each object by subtracting the
							//transferred velocity.  The velocityFromA is applied to B but subtracted from A, and the velocityFromB is applied to A
							//but subtracted from B.
							 
							const XMVECTOR totalVelocityChangeA = XMVectorSubtract(velocityFromB, velocityFromA);
							const XMVECTOR totalVelocityChangeB = XMVectorSubtract(velocityFromA, velocityFromB);

							if(obj->collideMoveable)
							{
								obj->ApplyForceAtPoint(collisionResult.collisionPoint, totalVelocityChangeA);
								//obj->SetLocked(true);
							}
							if(objB->collideMoveable)
							{
								objB->ApplyForceAtPoint(collisionResult.collisionPoint, totalVelocityChangeB);
								//objB->SetLocked(true);
							}

							if(obj->collideMoveable)
								obj->location = newLocA;
							if(objB->collideMoveable)
								objB->location = newLocB;

							const CollisionData collisionData1 = {objB, loc, collisionResult.collisionNormal};
							obj->OnCollision(collisionData1);
							const CollisionData collisionData2 = {obj, loc, collisionResult.collisionNormal};
							objB->OnCollision(collisionData2);
						}
					}
				}
			}
		}
	}

	bool FractalTreeWorld::CheckCollision(CollisionResult& collisionResult, const Actor* const objectA, const Actor* const objectB) const 
	{
		using std::vector;
		switch(objectA->collisionType)
		{
		case SPHERE:
			{
				switch(objectB->collisionType)
				{
				case SPHERE:
					{
						XMFLOAT4 dist, newScale, newScale2;

						XMStoreFloat4(&newScale, objectA->GetScale());
						XMStoreFloat4(&dist, XMVector3Length(XMVectorSubtract(objectB->location, objectA->location)));

						XMStoreFloat4(&newScale2, objectB->GetScale());
						float radius = newScale.x;
						float radiusB = newScale2.x;
						dist.x = dist.x - radiusB - radius;

						if(dist.x < 0)
						{
							collisionResult.collisionNormal = XMVector3Normalize(XMVectorSubtract(objectA->location, objectB->location));
							collisionResult.collisionPoint = XMVectorAdd(XMVectorMultiply(collisionResult.collisionNormal, XMVectorReplicate(radius)), objectA->location);
							collisionResult.collisionDistance = dist.x;
							return true;
						}

						return false;
					}

					break;
				case BOX:
					{
						XMFLOAT4 rot;
						//?
						//XMStoreFloat4(&rot, objectB->GetPitchYawRoll());
						const XMVECTOR modZAxis = XMVector3Transform(XMVector3Transform(XMVectorSet(0, 0, 1, 0), XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), rot.x))),
							XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));

						const XMVECTOR modXAxis = XMVector3Transform(XMVectorSet(1, 0, 0, 0), XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));

						XMFLOAT4 newScale;
						XMStoreFloat4(&newScale, objectA->GetScale());
						float radius = newScale.x;

						const vector<CollisionHull*>& collisionHulls = objectB->GetCollisionHulls();
						for(int a = 0; a < collisionHulls.size(); ++a)
						{
							const CollisionHull* const currentHull = collisionHulls[a];
							bool foundCollision = true;
							float smallestDistance = 10000000;
							XMVECTOR winningNormal;

							for(int whichPlane = 0; whichPlane < currentHull->planes.size(); ++whichPlane)
							{
								XMVECTOR vert = currentHull->vertices[whichPlane];
								vert = XMVector3Transform(vert, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));//v3(0, 1, 0), this->orientation.y);
								vert = XMVector3Transform(vert, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modXAxis, rot.x)));//rotateVectorQ(vert, modXaxis, this->orientation.x);
								vert = XMVector3Transform(vert, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modZAxis, rot.z)));//rotateVectorQ(vert, modZaxis, this->orientation.z);

								const XMVECTOR transformedVertex = XMVectorAdd(vert, objectB->location);

								XMVECTOR normal = currentHull->planes[whichPlane];

								normal = XMVector3Transform(normal, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));
								normal = XMVector3Transform(normal, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modXAxis, rot.x)));
								normal = XMVector3Transform(normal, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modZAxis, rot.z)));

								const XMVECTOR planeDistance = XMVector3Dot(XMVectorSubtract(objectA->location, transformedVertex), normal);

								XMFLOAT4 pDist;
								XMStoreFloat4(&pDist, planeDistance);

								if(pDist.x - radius <= 0)
								{
									if(fabs(pDist.x - radius) < fabs(smallestDistance))
									{
										smallestDistance = fabs(pDist.x - radius);
										winningNormal = normal;
									}
								}
								else
								{
									foundCollision = false;
									break;
								}
							}

							if(foundCollision)
							{
								collisionResult.collisionDistance = smallestDistance;
								collisionResult.collisionNormal = winningNormal;
								collisionResult.collisionPoint = XMVectorAdd(objectA->location, XMVectorMultiply(collisionResult.collisionNormal, XMVectorReplicate(radius)));
								return true;
							}
						}

						return false;
					}

					break;
				}
			}

			break;

		case BOX:
			{
				switch (objectB->collisionType)
				{
				case SPHERE:
					{
						XMFLOAT4 rot;
//						XMStoreFloat4(&rot, objectA->GetPitchYawRoll());
						const XMVECTOR modZAxis = XMVector3Transform(XMVector3Transform(XMVectorSet(0, 0, 1, 0), XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), rot.x))),
							XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));

						const XMVECTOR modXAxis = XMVector3Transform(XMVectorSet(1, 0, 0, 0), XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));

						XMFLOAT4 newScale;
						XMStoreFloat4(&newScale, objectB->GetScale());
						float radius = newScale.x;

						const vector<CollisionHull*>& collisionHulls = objectA->GetCollisionHulls();
						for(int a = 0; a < collisionHulls.size(); ++a)
						{
							const CollisionHull* const currentHull = collisionHulls[a];
							bool foundCollision = true;
							float smallestDistance = 10000000;
							XMVECTOR winningNormal = XMVectorZero();

							for(int whichPlane = 0; whichPlane < currentHull->planes.size(); ++whichPlane)
							{
								XMVECTOR vert = currentHull->vertices[whichPlane];
								vert = XMVector3Transform(vert, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));//v3(0, 1, 0), this->orientation.y);
								vert = XMVector3Transform(vert, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modXAxis, rot.x)));//rotateVectorQ(vert, modXaxis, this->orientation.x);
								vert = XMVector3Transform(vert, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modZAxis, rot.z)));//rotateVectorQ(vert, modZaxis, this->orientation.z);

								const XMVECTOR transformedVertex = XMVectorAdd(vert, objectA->location);

								XMVECTOR normal = currentHull->planes[whichPlane];

								normal = XMVector3Transform(normal, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), rot.y)));
								normal = XMVector3Transform(normal, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modXAxis, rot.x)));
								normal = XMVector3Transform(normal, XMMatrixRotationQuaternion(XMQuaternionRotationAxis(modZAxis, rot.z)));

								const XMVECTOR planeDistance = XMVector3Dot(XMVectorSubtract(objectB->location, transformedVertex), normal);

								XMFLOAT4 pDist;
								XMStoreFloat4(&pDist, planeDistance);

								if(pDist.x - radius <= 0)
								{
									if(fabs(pDist.x - radius) < fabs(smallestDistance))
									{
										smallestDistance = fabs(pDist.x - radius);
										winningNormal = normal;
									}
								}
								else
								{
									foundCollision = false;
									break;
								}
							}

							if(foundCollision)
							{
								collisionResult.collisionDistance = smallestDistance;
								collisionResult.collisionNormal = winningNormal;
								collisionResult.collisionPoint = XMVectorAdd(objectB->location, XMVectorMultiply(collisionResult.collisionNormal, XMVectorReplicate(radius)));
								return true;
							}
						}

						return false;
					}
					break;

				case BOX:
					{
						XMFLOAT4 dist;
						XMStoreFloat4(&dist, XMVector3Length(XMVectorSubtract(objectB->location, objectA->location)));

						float radius = objectA->GetRadius();
						float radiusB = objectB->GetRadius();
						dist.x = dist.x - radiusB - radius;

						if (dist.x < 0)
						{
							collisionResult.collisionNormal = XMVector3Normalize(XMVectorSubtract(objectA->location, objectB->location));
							collisionResult.collisionPoint = XMVectorAdd(XMVectorMultiply(collisionResult.collisionNormal, XMVectorReplicate(radius)), objectA->location);
							collisionResult.collisionDistance = dist.x;
							return true;
						}

						return false;
						break;
					}
				}
				
			}
			
			break;
		}

		return false;
	}

	float FractalTreeWorld::GetHeight(float x, float z) const
	{
		if(this->terrain != NULL)
		{
			return this->terrain->GetHeight(x, z);
		}

		return 0;
	}
}