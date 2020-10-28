#include "pch.h"
#include "Humanoid.h"
#include <DirectXMath.h>
#ifndef _NO_GRAPHICS
#include "..\Src\UIRenderer.h"
#endif

using namespace DirectX;
using namespace SimulationEngine;
int facingStringIndex = -1;

namespace SpaceGameCore
{
	Humanoid::Humanoid(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		turnSpeedFactor(16.f / 9.f),
		gunActive(false),
		grenadeActive(false),
		gun2Active(false)
	{
#ifndef _NO_GRAPHICS
		if (facingStringIndex == -1)
			facingStringIndex = RenderingEngine::UIRenderer::AddDebugString("Facing: ");
#endif
		this->persist = false;
		this->moveDir[0] = false;
		this->moveDir[1] = false;
		this->moveDir[2] = false;
		this->moveDir[3] = false;
		this->collisionType = NONE;
		Thruster* forwardThruster = new Thruster();
		Thruster* backwardThruster = new Thruster();
		Thruster* leftThruster = new Thruster();
		Thruster* rightThruster = new Thruster();

		backwardThruster->direction = XMVectorSet(0, 0, -1, 0);
		leftThruster->direction = XMVectorSet(-1, 0, 0, 0);
		rightThruster->direction = XMVectorSet(1, 0, 0, 0);
		forwardThruster->direction = XMVectorSet(0, 0, 1, 0);
		this->freeMove = false;
		forwardThruster->requireResting = !this->freeMove;
		leftThruster->requireResting = !this->freeMove;
		rightThruster->requireResting = !this->freeMove;
		backwardThruster->requireResting = !this->freeMove;

		this->AddThruster(forwardThruster);
		this->AddThruster(backwardThruster);
		this->AddThruster(leftThruster);
		this->AddThruster(rightThruster);

		//this->scale = XMVectorSet(10, 10, 10, 0);
		//this->location = XMVectorSet(0, 50, -200, 0);
		this->facing = XMVectorSet(0, 0, 10, 0);
		/*if(FractalTreeResources::fastEverything)
		{
			this->thrusters[0]->currentMagnitude = 3;
			this->thrusters[1]->currentMagnitude = 3;
			this->thrusters[2]->currentMagnitude = 3;
			this->thrusters[3]->currentMagnitude = 3;
		}
		else*/
		const float speed = .25f;
		this->mass = 100000;
		{
			this->thrusters[0]->currentMagnitude = speed * this->mass;
			this->thrusters[1]->currentMagnitude = speed * this->mass;
			this->thrusters[2]->currentMagnitude = speed * this->mass;
			this->thrusters[3]->currentMagnitude = speed * this->mass;

			this->thrusters[0]->maximumMagnitude = speed * this->mass;
			this->thrusters[1]->maximumMagnitude = speed * this->mass;
			this->thrusters[2]->maximumMagnitude = speed * this->mass;
			this->thrusters[3]->maximumMagnitude = speed * this->mass;
		}

		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::PATH;

		/*this->missileLauncher = new TestMissileLauncher(this);
		this->decentMissileLauncher = new DecentMissileLauncher(this);
		this->ineffectiveMissileLauncher = new IneffectiveMissileLauncher(this);*/
		this->objectType = ObjectTypes::Humanoid;
		//this->ignoreForceEffects = this->freeMove;
		this->thrusters[0]->requireResting = false;
		this->thrusters[1]->requireResting = false;
		this->thrusters[2]->requireResting = false;
		this->thrusters[3]->requireResting = false;
		this->thrusters[0]->airControlFactor = 1;
		this->thrusters[1]->airControlFactor = 1;
		this->thrusters[2]->airControlFactor = 1;
		this->thrusters[3]->airControlFactor = 1;
		this->scale = XMVectorSet(1, 25, 1, 1);
	}

	void Humanoid::Act()
	{
		__super::Act();
		/*if(this->grenadeActive)
		{
			this->missileLauncher->ShootWeapon();
		}

		if(this->gunActive)
		{
			this->decentMissileLauncher->ShootWeapon();
		}

		if(this->gun2Active)
		{
			this->ineffectiveMissileLauncher->ShootWeapon();
		}*/
	}

	void Humanoid::ReceiveCommand(const Command& command)
	{
#ifndef _NO_GRAPHICS
		RenderingEngine::UIRenderer::SetDebugString(facingStringIndex, "Facing: " + getVectorString(this->facing));
#endif
		switch(command.command)
		{
		case StrafeFoward:
			{
				if(command.flags & 1)
				{
					//this->brain.motorCortex.active = true;
					//this->brain.motorCortex.targetVelocity = XMVectorSet(5, 0, FractalTreeResources::runSpeed, 0);
					//if (!freeMove)
					this->thrusters[0]->activated = true;
					moveDir[0] = true;
				}
				else
				{
					//this->brain.motorCortex.targetVelocity = XMVectorSet(0, 0, 0, 0);
					//this->brain.motorCortex.active = false;
					this->thrusters[0]->activated = false;
					moveDir[0] = false;
				}
			}
			break;
		case StrafeBackward:
			{
				if(command.flags & 1)
				{
					//				if (!freeMove)
					this->thrusters[1]->activated = true;
					moveDir[1] = true;
				}
				else
				{
					this->thrusters[1]->activated = false;
					moveDir[1] = false;
				}
			}
			break;
		case StrafeLeft:
			{
				if(command.flags & 1)
				{
					//				if (!freeMove)
					this->thrusters[2]->activated = true;
					moveDir[2] = true;
				}
				else
				{
					this->thrusters[2]->activated = false;
					moveDir[3] = false;
				}
			}
			break;
		case StrafeRight:
			{
				if(command.flags & 1)
				{
					//				if (!freeMove)
					this->thrusters[3]->activated = true;
					moveDir[3] = true;
				}
				else
				{
					this->thrusters[3]->activated = false;
					moveDir[3] = false;
				}
			}
			break;
		case Look:
			{
				if(command.arg1 != 0) // Horizontal look
				{
					this->qOrientation = XMQuaternionMultiply(this->qOrientation, XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), command.arg1));
					this->viewDirection = XMVectorAdd(this->viewDirection, XMVectorSet(0, command.arg1, 0, 0));
				}

				if(command.arg2 != 0) // Vertical look
				{
					this->facing = XMVector3Rotate(this->facing, XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), command.arg2 * this->turnSpeedFactor));
					this->viewDirection = XMVectorAdd(this->viewDirection, XMVectorSet(command.arg2 * this->turnSpeedFactor, 0, 0, 0));
				}
			}
			break;
		case Jump:
			{
				if(this->resting || /*FractalTreeResources::freeMove ||*/ this->freeMove)
				{
					this->ApplyForceAtPoint(this->location, XMVectorSet(0, 9.25f * this->mass, 0, 0));
				}
			}
			break;
		case Shoot:
			{
				this->grenadeActive = !this->grenadeActive;
			}
			break;
		case Shoot2:
			{
				this->gunActive = !this->gunActive;
			}
			break;
		case Shoot3:
			{
				this->gun2Active = !this->gun2Active;
			}
			break;
		case Shoot4:
			{
				//this->decentMissileLauncher->ShootWeapon();
			}
			break;
		}
	}
}