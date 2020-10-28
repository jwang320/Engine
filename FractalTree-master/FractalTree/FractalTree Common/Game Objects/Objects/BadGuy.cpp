#include "pch.h"
#include "BadGuy.h"
#include "DecentMissileLauncher.h"
#ifndef _NO_GRAPHICS
#include "UIRenderer.h"
#include <sstream>
#endif
#include "..\..\FractalTreeWorld.h"
using namespace DirectX;

int localVelocityIndex;
int orientIndex;
int bFacingIndex;
int bVelIndex;
int pryIndex;
int isLevelIndex;
int locIndex;
int targetLocIndex;
static bool hasTextYet = false;
namespace SimulationEngine
{
	BadGuy::BadGuy(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters)
	{
#ifndef _NO_GRAPHICS
		if (!hasTextYet)
		{
			hasTextYet = true;
			this->ownsTheText = true;
			localVelocityIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy Local Velocity: ");
			orientIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy Orientation: ");
			bFacingIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy Facing: ");
			bVelIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy Normalized Velocity: ");
			pryIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy Pitch Roll Yaw: ");
			isLevelIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy is Level: ");
			locIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy loc: ");
			targetLocIndex = RenderingEngine::UIRenderer::AddDebugString("BadGuy target loc: ");
		}
#endif


		this->initializeCollisionHull();


		this->mass = 1;
		this->rotationalInertia = 11150;
		float thrusterMag = 330.25f;
		float turnMag = 10.315f;
		this->collisionType = BOX;

		Thruster* const bottomCenterThruster = new Thruster();
		this->AddThruster(bottomCenterThruster);
		this->thrusters[BOTTOM_CENTER]->requireResting = false;
		this->thrusters[BOTTOM_CENTER]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[BOTTOM_CENTER]->offset = XMVectorSet(0, 1, 0, 1);
		this->thrusters[BOTTOM_CENTER]->maximumMagnitude = thrusterMag;
		this->thrusters[BOTTOM_CENTER]->currentMagnitude = thrusterMag;

		/*Thruster* const backCenterThruster = new Thruster(); 
		this->AddThruster(backCenterThruster);
		this->thrusters[BACK_CENTER]->requireResting = false;
		this->thrusters[BACK_CENTER]->direction = XMVectorSet(0, 0, 1, 0);
		this->thrusters[BACK_CENTER]->offset = XMVectorSet(0, 0, -1, 1);
		this->thrusters[BACK_CENTER]->maximumMagnitude = thrusterMag * 5;
		this->thrusters[BACK_CENTER]->currentMagnitude = 0;*/

		Thruster* const backLeftThruster = new Thruster(); 
		this->AddThruster(backLeftThruster);
		backLeftThruster->requireResting = false;
		backLeftThruster->direction = XMVectorSet(0, 0, 1, 0);
		backLeftThruster->offset = XMVectorSet(-10, 0, -1, 1);
		backLeftThruster->maximumMagnitude = turnMag;
		backLeftThruster->currentMagnitude = 0;

		Thruster* const backRightThruster = new Thruster();
		this->AddThruster(backRightThruster);
		backRightThruster->requireResting = false;
		backRightThruster->direction = XMVectorSet(0, 0, 1, 0);
		backRightThruster->offset = XMVectorSet(10, 0, -1, 1);
		backRightThruster->maximumMagnitude = turnMag;
		backRightThruster->currentMagnitude = 0;


		Thruster* const frontLeftThruster = new Thruster();
		this->AddThruster(frontLeftThruster);
		frontLeftThruster->requireResting = false;
		frontLeftThruster->direction = XMVectorSet(0, 0, -1, 0);
		frontLeftThruster->offset = XMVectorSet(-10, 0, 1, 1);
		frontLeftThruster->maximumMagnitude = turnMag;
		frontLeftThruster->currentMagnitude = 0;

		Thruster* const frontRightThruster = new Thruster();
		this->AddThruster(frontRightThruster);
		frontRightThruster->requireResting = false;
		frontRightThruster->direction = XMVectorSet(0, 0, -1, 0);
		frontRightThruster->offset = XMVectorSet(10, 0, 1, 1);
		frontRightThruster->maximumMagnitude = turnMag;
		frontRightThruster->currentMagnitude = 0;

		Thruster* const bottomLeftThruster = new Thruster();
		this->AddThruster(bottomLeftThruster);
		bottomLeftThruster->requireResting = false;
		bottomLeftThruster->direction = XMVectorSet(0, 1, 0, 0);
		bottomLeftThruster->offset = XMVectorSet(-10, -1, 0, 1);
		bottomLeftThruster->maximumMagnitude = thrusterMag;
		bottomLeftThruster->currentMagnitude = 0;

		Thruster* const bottomRightThruster = new Thruster();
		this->AddThruster(bottomRightThruster);
		bottomRightThruster->requireResting = false;
		bottomRightThruster->direction = XMVectorSet(0, 1, 0, 0);
		bottomRightThruster->offset = XMVectorSet(10, -1, 0, 1);
		bottomRightThruster->maximumMagnitude = thrusterMag;
		bottomRightThruster->currentMagnitude = 0;

		Thruster* const bottomFrontThruster = new Thruster();
		this->AddThruster(bottomFrontThruster);
		bottomFrontThruster->requireResting = false;
		bottomFrontThruster->direction = XMVectorSet(0, 1, 0, 0);
		bottomFrontThruster->offset = XMVectorSet(0, -1, 5, 1);
		bottomFrontThruster->maximumMagnitude = turnMag;
		bottomFrontThruster->currentMagnitude = 0;

		Thruster* const bottomBackThruster = new Thruster();
		this->AddThruster(bottomBackThruster);
		bottomBackThruster->requireResting = false;
		bottomBackThruster->direction = XMVectorSet(0, 1, 0, 0);
		bottomBackThruster->offset = XMVectorSet(0, -1, -5, 1);
		bottomBackThruster->maximumMagnitude = turnMag;
		bottomBackThruster->currentMagnitude = 0;

		Thruster* const topLeftThruster = new Thruster();
		this->AddThruster(topLeftThruster);
		topLeftThruster->requireResting = false;
		topLeftThruster->direction = XMVectorSet(0, -1, 0, 0);
		topLeftThruster->offset = XMVectorSet(-10, 1, 0, 1);
		topLeftThruster->maximumMagnitude = turnMag;
		topLeftThruster->currentMagnitude = 0;

		Thruster* const topRightThruster = new Thruster();
		this->AddThruster(topRightThruster);
		topRightThruster->requireResting = false;
		topRightThruster->direction = XMVectorSet(0, -1, 0, 0);
		topRightThruster->offset = XMVectorSet(10, 1, 0, 1);
		topRightThruster->maximumMagnitude = turnMag;
		topRightThruster->currentMagnitude = 0;


		/*
		Thruster* const topFrontThruster = new Thruster();
		this->AddThruster(topFrontThruster);
		this->thrusters[TOP_FRONT]->requireResting = false;
		this->thrusters[TOP_FRONT]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[TOP_FRONT]->offset = XMVectorSet(0, 1, 5, 1);
		this->thrusters[TOP_FRONT]->maximumMagnitude = turnMag;
		this->thrusters[TOP_FRONT]->currentMagnitude = 0;

		Thruster* const topBackThruster = new Thruster();
		this->AddThruster(topBackThruster);
		this->thrusters[TOP_BACK]->requireResting = false;
		this->thrusters[TOP_BACK]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[TOP_BACK]->offset = XMVectorSet(0, 1, -5, 1);
		this->thrusters[TOP_BACK]->maximumMagnitude = turnMag;
		this->thrusters[TOP_BACK]->currentMagnitude = 0;

		*/


		int skipCount = 2;

		Thruster* const frontCenterThruster = new Thruster();
		this->AddThruster(frontCenterThruster);
		frontCenterThruster->requireResting = false;
		frontCenterThruster->direction = XMVectorSet(0, 0, -1, 0);
		frontCenterThruster->offset = XMVectorSet(0, 0, 1, 1);
		frontCenterThruster->maximumMagnitude = thrusterMag;
		frontCenterThruster->currentMagnitude = 0;

		Thruster* const leftThruster = new Thruster();
		this->AddThruster(leftThruster);
		leftThruster->requireResting = false;
		leftThruster->direction = XMVectorSet(1, 0, 0, 0);
		leftThruster->offset = XMVectorSet(-1, 0, 0, 1);
		leftThruster->maximumMagnitude = thrusterMag*.55f;
		leftThruster->currentMagnitude = 0;

		Thruster* const rightThruster = new Thruster();
		this->AddThruster(rightThruster);
		rightThruster->requireResting = false;
		rightThruster->direction = XMVectorSet(-1, 0, 0, 0);
		rightThruster->offset = XMVectorSet(1, 0, 0, 1);
		rightThruster->maximumMagnitude = thrusterMag*.55f;
		rightThruster->currentMagnitude = 0;
		
		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::ACT;
		this->decentMissileLauncher = new DecentMissileLauncher(this);

		this->objectType = ObjectTypes::BadGuy;
		//this->ignoreForceEffects = true;

		//this->pitchYawRollVelocity = XMVectorSet(0,  .0025f, 0, 0);
		//this->SetPitchYawRoll(XMVectorSet(0, 80 / 180.f * 3.1415f, 0, 0));
		this->rotationalInertia = 11;
		this->dragCoefficient = 0;// 0.05f;// 0.0001f;
		this->mass = 114;
		this->brain.motorCortex.Calibrate();
		this->radius = 15;
		//this->collideMoveable = false;
		//this->ignoreForceEffects = true;
	}

	void BadGuy::ApplyForceUp(float amount)
	{
		const float p = amount / this->thrusters[BOTTOM_CENTER]->maximumMagnitude;
		TimedThrusterFire thrusterFire = {1, this->thrusters[BOTTOM_CENTER], p};
		this->brain.motorCortex.AddCommand(0, thrusterFire);
	}

	static XMVECTOR toEulerianAngle(const XMVECTOR& qq)
	{
		XMFLOAT4 q;
		XMStoreFloat4(&q, qq);
		double ysqr = q.y * q.y;
		double t0 = -2.0f * (ysqr + q.z * q.z) + 1.0f;
		double t1 = +2.0f * (q.x * q.y - q.w * q.z);
		double t2 = -2.0f * (q.x * q.z + q.w * q.y);
		double t3 = +2.0f * (q.y * q.z - q.w * q.x);
		double t4 = -2.0f * (q.x * q.x + ysqr) + 1.0f;

		t2 = t2 > 1.0f ? 1.0f : t2;
		t2 = t2 < -1.0f ? -1.0f : t2;

		const double pitch = std::asin(t2);
		const double roll = std::atan2(t3, t4);
		const double yaw = std::atan2(t1, t0);

		return XMVectorSet(roll, pitch  , yaw, 0);
	}

	//bool isHovering = false;
	void BadGuy::Act()
	{
#ifdef DO_STUFF
		if (frameCount % 2 == 0)	
		{
			if (this->targetObjectID >= 0)
			{
				if (!this->parentWorld->IsObjectAlive(this->targetObjectID))
				{
					this->targetObjectID = -1;
				}
			}

			const XMMATRIX qMatrix = XMMatrixRotationQuaternion(this->qOrientation);
			const XMVECTOR localZ = XMVector3Transform(XMVectorSet(0, 0, 1, 0), qMatrix);
			const XMVECTOR localY = XMVector3Transform(XMVectorSet(0, 1, 0, 0), qMatrix);
			const XMVECTOR localX = XMVector3Transform(XMVectorSet(1, 0, 0, 0), qMatrix);
			XMFLOAT4 localVelF, pitchVel, pitch, facingF, loc, pitchVel2, localXF, localYF, localZF;
			XMStoreFloat4(&localVelF, this->GetLocalVelocity());
			XMStoreFloat4(&facingF, this->facingVector);
			XMStoreFloat4(&loc, this->GetLocation());
			//XMStoreFloat4(&pitchVel, this->GetPitchYawRollVelocity());
			//XMStoreFloat4(&pitchVel2, this->previousAngularVelocity);
			XMStoreFloat4(&pitch, toEulerianAngle(this->qOrientation));
			XMStoreFloat4(&localYF, localY);
			XMStoreFloat4(&localZF, localZ);
			XMStoreFloat4(&localXF, localX);
			float hoverHeight = 100;
			XMFLOAT4 targetLocation = loc;
			XMFLOAT4 targetVelocity = { 0, 0, 0, 0 };
			XMFLOAT4 targetAngularVelocity = { 0, 0, 0, 0 };// pitchVel;
			XMFLOAT4 pitchVelThreshold = { 0, 0, 0, 0 };
			XMFLOAT4 targetVelThreshold = { 0, 0, 0, 0 };

			const float turnMag = 1.f;
			const float thrustMag = .15f;
			const float maxYVelocity = 1.1f;
			const float maxZVelocity = .01f;
			const float maxXVelocity = .01f;
			float ang = 0;

			this->brain.motorCortex.targetVelocity = XMVectorSet(0, 0, 0, 0);
			this->brain.motorCortex.targetPitchYawRoll = XMVectorSet(0, 0, 0, 0);
			this->brain.motorCortex.targetPitchYawRollVelocity = XMVectorSet(0, 0, 0, 0);
			this->brain.motorCortex.targetLocation = XMVectorSet(0, hoverHeight, 0, 0);
			this->brain.motorCortex.monitorZVelocity = true;
			this->brain.motorCortex.monitorXVelocity = true;


			bool level = true;
			if (moveState >= 1)
			{
				this->brain.motorCortex.active = true;
				if (this->targetObjectID >= 0)
				{
					const Actor* const target = static_cast<Actor*>(static_cast<const PhysicalWorld* const>(this->parentWorld)->GetObjectByGlobalID(this->targetObjectID));
					ang = static_cast<PhysicalWorld*>(this->parentWorld)->AngleBetweenObjects(this->GetGlobalID(), this->targetObjectID);
					const float deg = fabs(ang * 180.f / 3.1415f);
					const XMVECTOR dist = XMVector3Length(XMVectorSubtract(XMVectorSetY(this->location, 0), XMVectorSetY(target->GetLocation(), 0)));
					XMFLOAT4 distF;
					XMStoreFloat4(&distF, dist);

					if (distF.x > 200)
					{
						hoverHeight = 150;
						/*if (fabs(pitchVel.y) < 1.f / *&& localYF.y > .95f* /)
						{
							if (ang < 0)
							{
								targetAngularVelocity.y = min(maxYVelocity, (deg / 180.f));
							}
							else
							{
								targetAngularVelocity.y = -min(maxYVelocity, (deg / 180.f));
							}
						}*/
						if (deg < 45)
						{
							targetVelocity.z = min(20, 10 + distF.x / 400);
							//targetAngularVelocity.y = 0;
						}
						else
						{
							targetVelocity.z = 1;
							//targetVelocity.z = .25f*min(20, 10 + distF.x / 400);
							//this->brain.motorCortex.monitorXVelocity = false;
							//this->brain.motorCortex.monitorZVelocity = false;
						}
					}
					else
					{
						hoverHeight = 100;
						targetVelocity.z = 2;// 1 * (1.25f + (rand() % 10 - 5) * .05);
					}

#ifndef _NO_GRAPHICS
					if (this->ownsTheText)
						RenderingEngine::UIRenderer::SetDebugString(bFacingIndex, " ang: " + std::to_string(ang * 180 / 3.1415f) + " BadGuy Ang Vel: (" + std::to_string(pitchVel.x) + ", " +
							std::to_string(pitchVel.y) + ", " + std::to_string(pitchVel.z) + ")");
#endif
				}

				targetLocation.y = hoverHeight;

				static const float turnFixRate = 1;
				static const float unLevelCap = .05f;
				/*if (localZF.y > unLevelCap)
				{
					targetAngularVelocity.x = turnFixRate;
					//level = false;
				}
				else if (localZF.y < -unLevelCap)
				{
					targetAngularVelocity.x = -turnFixRate;
					//level = false;
				}
				else
				{
					targetAngularVelocity.x = 0;
					//level = true;
				}*/
				/*if (localXF.y > unLevelCap)
				{
					targetAngularVelocity.z = turnFixRate;
					//level = false;
				}
				else if (localXF.y < -unLevelCap)
				{
					targetAngularVelocity.z = -turnFixRate;
					//level = false;
				}
				else
				{
					targetAngularVelocity.z = 0;
					//level = true;
				}*/
				/*if (pitch.x > 0 && pitch.x < 3.1415f)
					//if (localZF.y > 0)
				{
					//if (pitchVel.x >= 0 || pitchVel.x - pitchVel2.x < 0)
					{
						targetAngularVelocity.x = -turnFixRate;
					}

					if (pitch.x > 3.1415f / 2)
					{
						//level = false;
					}
				}
				else if (pitch.x < 2 * 3.1415f && pitch.x > 3.1415f)
				{
					//if (pitchVel.x <= 0 || pitchVel.x - pitchVel2.x > 0)
					{
						targetAngularVelocity.x = turnFixRate;
					}
					if (pitch.x < 2 * 3.1415f - 3.1415f / 2)
					{
						//level = false;
					}
				}

				if (pitch.z > 0 && pitch.z < 3.1415f)
				{
					//if (pitchVel.z >= 0 || pitchVel.z - pitchVel2.z < 0)
					{
						targetAngularVelocity.z = -.01f;
					}

					if (pitch.z > 3.1415f / 2)
					{
						//level = false;
					}
				}
				else if (pitch.z < 2 * 3.1415f && pitch.z > 3.1415f)
				{
					//if (pitchVel.z <= 0 || pitchVel.z - pitchVel2.z > 0)
					{
						targetAngularVelocity.z = turnFixRate;
					}

					if (pitch.z < 2*3.1415f - 3.1415f / 2)
					{
						//level = false;
					}
				}*/

				const float tHeight = targetLocation.y + static_cast<FractalTreeWorld*>(this->parentWorld)->GetHeight(loc.x, loc.z);
				if (loc.y < tHeight)
				{
					//targetVelocity.y = 1;
					XMFLOAT4 tVel;
					XMStoreFloat4(&tVel, this->GetLocalVector(XMVectorSet(0, tHeight - loc.y, 0, 0)));
					targetVelocity.x += tVel.x;
					targetVelocity.y += tVel.y * .01f;
					targetVelocity.z += tVel.z;
					//this->ApplyForceUp(this->thrusters[BOTTOM_CENTER]->maximumMagnitude * (1 - min(.5f, facingF.y)));
				}
				else //if (localVelF.y < -1 && loc.y < 500 || localVelF.y < -10)
				{
					//targetVelocity.y = -10;
					XMFLOAT4 tVel;
					XMStoreFloat4(&tVel, this->GetLocalVector(XMVectorSet(0, tHeight - loc.y, 0, 0)));
					targetVelocity.x += tVel.x * .25f;
					targetVelocity.y += tVel.y * .0125f;
					targetVelocity.z += tVel.z * .25f;
					//const TimedThrusterFire thrusterFire4 = { 1, this->GetThrusters()[BOTTOM_CENTER], .5f  * powf(fabs(localVelF.y), 1) };
					//this->brain.motorCortex.AddCommand(0, thrusterFire4);
				}
			}

			XMFLOAT4 speed;
			XMStoreFloat4(&speed, XMVector3Length(this->velocity));
			if (speed.x > 30 || speed.z > 30 || !level)
			{
				targetVelocity.x = 0;
				targetVelocity.y = 0;
				targetVelocity.z = 0;
			}
			this->brain.motorCortex.targetPitchYawRollVelocity = XMLoadFloat4(&targetAngularVelocity);
			this->brain.motorCortex.targetVelocity = XMLoadFloat4(&targetVelocity);
			this->brain.motorCortex.targetLocation = XMLoadFloat4(&targetLocation);
			this->brain.Update();

#ifndef _NO_GRAPHICS
			{
				if (this->ownsTheText && updateNow % 20 == 0)
				{
					RenderingEngine::UIRenderer::SetDebugString(localVelocityIndex, "BadGuy Local Velocity: (" + std::string(((localVelF.x >= 0) ? " " : "")) + std::to_string(localVelF.x) + ", " +
						std::string(((localVelF.y >= 0) ? " " : "")) + std::to_string(localVelF.y) + ", " + std::string(((localVelF.z >= 0) ? " " : "")) + std::to_string(localVelF.z) + ")" + " target: " + std::to_string(this->targetObjectID));

					RenderingEngine::UIRenderer::SetDebugString(orientIndex, "BadGuy Target Vel: (" + std::to_string(targetVelocity.x) + ", " +
						std::to_string(targetVelocity.y) + ", " + std::to_string(targetVelocity.z) + ")");

					RenderingEngine::UIRenderer::SetDebugString(locIndex, "BadGuy Loc: (" + std::to_string(loc.x) + ", " +
						std::to_string(loc.y) + ", " + std::to_string(loc.z) + ")");

					RenderingEngine::UIRenderer::SetDebugString(targetLocIndex, "BadGuy Target Loc: (" + std::to_string(targetLocation.x) + ", " +
						std::to_string(targetLocation.y) + ", " + std::to_string(targetLocation.z) + ")");

					RenderingEngine::UIRenderer::SetDebugString(bVelIndex, "BadGuy Target AngVel: (" + std::to_string(targetAngularVelocity.x) + ", " +
						std::to_string(targetAngularVelocity.y) + ", " + std::to_string(targetAngularVelocity.z) + ")");


				}
				if (this->ownsTheText)
				{	RenderingEngine::UIRenderer::SetDebugString(pryIndex, "BadGuy Pitch: (" + std::to_string((int)(pitch.x / 3.1415f * 180)) + ", " +
						std::to_string((int)(pitch.y / 3.1415f * 180)) + ", " + std::to_string((int)(pitch.z / 3.1415f * 180)) + ")");
					RenderingEngine::UIRenderer::SetDebugString(isLevelIndex, "BadGuy is Level: " + std::to_string(level));
				}
				//if (this->ownsTheText)
				//	RenderingEngine::UIRenderer::SetDebugString(pryIndex, "BadGuy Pitch: " + std::to_string(pitch.y / 3.1415f * 180));
				++updateNow;
			}
#endif
			//this->previousAngularVelocity = this->pitchYawRollVelocity;
			this->previousVelocity = this->velocity;

		}

		++this->frameCount;
#endif
		__super::Act();
	}

	void BadGuy::TrackTarget(int targetID)
	{
		this->targetObjectID = targetID;
		this->moveState = 1;
	}

	void BadGuy::Hover(int targetID)
	{
		if(moveState == 0)
		{
			moveState = 1;
					//return;
		}
		else if(moveState == 1)
		{
			moveState = 0;
		}

		if(this->targetObjectID < 0)
		{
			/*std::vector<Actor*> targets;
			static_cast<PhysicalWorld*>(this->parentWorld)->QueryObjectsLocation(targets, this->location, 10000000, ObjectTypes::Humanoid);
			
			if(targets.size() > 0)
			{
				Actor* targetObject = targets[0];

				this->targetObjectID = targetObject->GetGlobalID();
			}*/

			this->targetObjectID = targetID;
		}

		//if(moveState == 1)
		//	this->ApplyForceAtPoint(XMVectorAdd(this->location, XMVectorSet(10, 0, 10, 0)), XMVectorSet(0, 1.9f, 0, 0));
	}

	void BadGuy::Bump()
	{
		this->ApplyForceAtPoint(XMVectorAdd(this->location, XMVectorSet(rand() % 10, 0, rand() % 10, 0)), XMVectorSet(0, 123.9f, 0, 0));
		this->ApplyForceAtPoint(this->location, XMVectorSet(0, 333.9f, 0, 0));
	}

	void BadGuy::TurnLeft(float degrees, int frames)
	{
		//this->FireThrusterPercent(this->thrusters[BackRight], frames, degrees);
		//this->FireThrusterPercent(this->thrusters[FrontLeft], frames, degrees);

		/*TimedThrusterFire thrusterFire = {frames, this->thrusters[BACK_RIGHT], degrees};
		TimedThrusterFire thrusterFire2 = {frames, this->thrusters[FRONT_LEFT], degrees};
		TimedThrusterFire thrusterFire3 = {frames, this->thrusters[FRONT_RIGHT], degrees};
		TimedThrusterFire thrusterFire4 = {frames, this->thrusters[BACK_LEFT], degrees};

		this->brain.motorCortex.AddCommand(0, thrusterFire);
		this->brain.motorCortex.AddCommand(0, thrusterFire2);*/

		//this->pitchYawRoll = XMVectorAdd(this->pitchYawRoll, XMVectorSet(0, 0, .1f, 0));
		//this->pitchYawRollVelocity = XMVectorSet(0, 0, -.005f, 0);
		//this->brain.motorCortex.AddCommand(frames, thrusterFire3);
		//this->brain.motorCortex.AddCommand(frames, thrusterFire4);
	}

	void BadGuy::TurnRight(float degrees, int frames)
	{
		TimedThrusterFire thrusterFire = {frames, this->thrusters[BACK_RIGHT], degrees};
		TimedThrusterFire thrusterFire2 = {frames, this->thrusters[FRONT_LEFT], degrees};
		TimedThrusterFire thrusterFire3 = {frames, this->thrusters[FRONT_RIGHT], degrees};
		TimedThrusterFire thrusterFire4 = {frames, this->thrusters[BACK_LEFT], degrees};

		this->brain.motorCortex.AddCommand(frames, thrusterFire);
		this->brain.motorCortex.AddCommand(frames, thrusterFire2);

		this->brain.motorCortex.AddCommand(0, thrusterFire3);
		this->brain.motorCortex.AddCommand(0, thrusterFire4);
		//this->FireThrusterPercent(this->thrusters[FrontRight], frames, degrees);
		//this->FireThrusterPercent(this->thrusters[BackLeft], frames, degrees);
	}

	void BadGuy::initializeCollisionHull()
	{
		const float length = 90;
		const float height =  25;
		const float width = 50;
		//this->scale = XMVectorSet(length, height, width, 0);
		//this->scale = XMVectorSet(3, 3, 3, 0);
		this->collisionHulls.push_back(new CollisionHull());

		this->collisionHulls[0]->minBounds = XMVectorSet(-length / 2, -height / 2, -width / 2, 0);
		this->collisionHulls[0]->maxBounds = XMVectorSet(length / 2, height / 2, width / 2, 0);
		this->collisionHulls[0]->planes.push_back(XMVectorSet(-1, 0, 0, 0));
		this->collisionHulls[0]->vertices.push_back(XMVectorSet(-length / 2, 0, 0, 0));

		this->collisionHulls[0]->planes.push_back(XMVectorSet(1, 0, 0, 0));
		this->collisionHulls[0]->vertices.push_back(XMVectorSet(length / 2, 0, 0, 0));

		this->collisionHulls[0]->planes.push_back(XMVectorSet(0, -1, 0, 0));
		this->collisionHulls[0]->vertices.push_back(XMVectorSet(0, -height / 2, 0, 0));

		this->collisionHulls[0]->planes.push_back(XMVectorSet(0, 1, 0, 0));
		this->collisionHulls[0]->vertices.push_back(XMVectorSet(0, height / 2, 0, 0));

		this->collisionHulls[0]->planes.push_back(XMVectorSet(0, 0, -1, 0));
		this->collisionHulls[0]->vertices.push_back(XMVectorSet(0, 0, -width / 2, 0));

		this->collisionHulls[0]->planes.push_back(XMVectorSet(0, 0, 1, 0));
		this->collisionHulls[0]->vertices.push_back(XMVectorSet(0, 0, width / 2, 0));
	}
}