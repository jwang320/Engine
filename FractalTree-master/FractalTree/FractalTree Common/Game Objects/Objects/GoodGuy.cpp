#include "pch.h"
#include "GoodGuy.h"
#include "TestMissileLauncher.h"
#include "DecentMissileLauncher.h"
#include "IneffectiveMissileLauncher.h"
#ifndef _NO_GRAPHICS
using namespace RenderingEngine;
#include "UIRenderer.h"
#endif
#include <string>
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"

using namespace DirectX;

int thrusterDebugIndexFront;
int thrusterDebugIndexBack;
int horizontalDebugIndex;
int verticalDebugIndex;
int orientationIndex;
int orientationVelocityIndex;
int headingIndex;
int upIndex;

namespace SimulationEngine
{
	GoodGuy::GoodGuy(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		turnSpeedFactor(16.f / 9.f),
		gunActive(false),
		grenadeActive(false),
		gun2Active(false),
		horizontalTurn(0),
		verticalTurn(0)

	{

#ifndef _NO_GRAPHICS
		thrusterDebugIndexFront = UIRenderer::AddDebugString("Thruster Level: ");
		thrusterDebugIndexBack = UIRenderer::AddDebugString("Thruster Level: ");
		orientationIndex = UIRenderer::AddDebugString("Orientation: ");
		orientationVelocityIndex = UIRenderer::AddDebugString("Orientation Velocity: ");
		headingIndex = UIRenderer::AddDebugString("Heading: ");
		upIndex = UIRenderer::AddDebugString("Up Vector: ");
#endif
		for(int a = 0; a < 14; ++a)
		{
			this->thrusterThrottleStatus[a] = 0;
		}

		rolling[0] = false;
		rolling[1] = false;
		rolling[2] = false;
		rolling[3] = false;
		float thrusterMag = 543.25f; //33
		float turnMag = 23.315f;

		Thruster* bottomCenterThruster = new Thruster();
		this->AddThruster(bottomCenterThruster);
		this->thrusters[BOTTOM_CENTER]->requireResting = false;
		this->thrusters[BOTTOM_CENTER]->direction = XMVectorSet(0, 1, 0, 0);
		this->thrusters[BOTTOM_CENTER]->offset = XMVectorSet(0, -1, 0, 1);
		this->thrusters[BOTTOM_CENTER]->maximumMagnitude = thrusterMag;
		this->thrusters[BOTTOM_CENTER]->currentMagnitude = thrusterMag;
		
		Thruster* backCenterThruster = new Thruster(); 
		this->AddThruster(backCenterThruster);
		this->thrusters[BACK_CENTER]->requireResting = false;
		this->thrusters[BACK_CENTER]->direction = XMVectorSet(0, 0, 1, 0);
		this->thrusters[BACK_CENTER]->offset = XMVectorSet(0, 0, -1, 1);
		this->thrusters[BACK_CENTER]->maximumMagnitude = thrusterMag;
		this->thrusters[BACK_CENTER]->currentMagnitude = 0;

		Thruster* backLeftThruster = new Thruster(); 
		this->AddThruster(backLeftThruster);
		this->thrusters[BACK_LEFT]->requireResting = false;
		this->thrusters[BACK_LEFT]->direction = XMVectorSet(0, 0, 1, 0);
		this->thrusters[BACK_LEFT]->offset = XMVectorSet(-10, 0, -1, 1);
		this->thrusters[BACK_LEFT]->maximumMagnitude = turnMag;
		this->thrusters[BACK_LEFT]->currentMagnitude = 0;

		Thruster* backRightThruster = new Thruster();
		this->AddThruster(backRightThruster);
		this->thrusters[BACK_RIGHT]->requireResting = false;
		this->thrusters[BACK_RIGHT]->direction = XMVectorSet(0, 0, 1, 0);
		this->thrusters[BACK_RIGHT]->offset = XMVectorSet(10, 0, -1, 1);
		this->thrusters[BACK_RIGHT]->maximumMagnitude = turnMag;
		this->thrusters[BACK_RIGHT]->currentMagnitude = 0;


		Thruster* frontLeftThruster = new Thruster();
		this->AddThruster(frontLeftThruster);
		this->thrusters[FRONT_LEFT]->requireResting = false;
		this->thrusters[FRONT_LEFT]->direction = XMVectorSet(0, 0, -1, 0);
		this->thrusters[FRONT_LEFT]->offset = XMVectorSet(-10, 0, 1, 1);
		this->thrusters[FRONT_LEFT]->maximumMagnitude = turnMag;
		this->thrusters[FRONT_LEFT]->currentMagnitude = 0;

		Thruster* frontRightThruster = new Thruster();
		this->AddThruster(frontRightThruster);
		this->thrusters[FRONT_RIGHT]->requireResting = false;
		this->thrusters[FRONT_RIGHT]->direction = XMVectorSet(0, 0, -1, 0);
		this->thrusters[FRONT_RIGHT]->offset = XMVectorSet(10, 0, 1, 1);
		this->thrusters[FRONT_RIGHT]->maximumMagnitude = turnMag;
		this->thrusters[FRONT_RIGHT]->currentMagnitude = 0;

		Thruster* topFrontThruster = new Thruster();
		this->AddThruster(topFrontThruster);
		this->thrusters[TOP_FRONT]->requireResting = false;
		this->thrusters[TOP_FRONT]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[TOP_FRONT]->offset = XMVectorSet(0, 1, 5, 1);
		this->thrusters[TOP_FRONT]->maximumMagnitude = turnMag;
		this->thrusters[TOP_FRONT]->currentMagnitude = 0;

		Thruster* topBackThruster = new Thruster();
		this->AddThruster(topBackThruster);
		this->thrusters[TOP_BACK]->requireResting = false;
		this->thrusters[TOP_BACK]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[TOP_BACK]->offset = XMVectorSet(0, 1, -5, 1);
		this->thrusters[TOP_BACK]->maximumMagnitude = turnMag;
		this->thrusters[TOP_BACK]->currentMagnitude = 0;

		Thruster* bottomFrontThruster = new Thruster();
		this->AddThruster(bottomFrontThruster);
		this->thrusters[BOTTOM_FRONT]->requireResting = false;
		this->thrusters[BOTTOM_FRONT]->direction = XMVectorSet(0, 1, 0, 0);
		this->thrusters[BOTTOM_FRONT]->offset = XMVectorSet(0, -1, 5, 1);
		this->thrusters[BOTTOM_FRONT]->maximumMagnitude = turnMag;
		this->thrusters[BOTTOM_FRONT]->currentMagnitude = 0;

		Thruster* bottomBackThruster = new Thruster();
		this->AddThruster(bottomBackThruster);
		this->thrusters[BOTTOM_BACK]->requireResting = false;
		this->thrusters[BOTTOM_BACK]->direction = XMVectorSet(0, 1, 0, 0);
		this->thrusters[BOTTOM_BACK]->offset = XMVectorSet(0, -1, -5, 1);
		this->thrusters[BOTTOM_BACK]->maximumMagnitude = turnMag;
		this->thrusters[BOTTOM_BACK]->currentMagnitude = 0;
		this->rotationalInertia = 11;

		Thruster* topLeftThruster = new Thruster();
		this->AddThruster(topLeftThruster);
		this->thrusters[TOP_LEFT]->requireResting = false;
		this->thrusters[TOP_LEFT]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[TOP_LEFT]->offset = XMVectorSet(-10, 1, 0, 1);
		this->thrusters[TOP_LEFT]->maximumMagnitude = turnMag;
		this->thrusters[TOP_LEFT]->currentMagnitude = 0;

		Thruster* topRightThruster = new Thruster();
		this->AddThruster(topRightThruster);
		this->thrusters[TOP_RIGHT]->requireResting = false;
		this->thrusters[TOP_RIGHT]->direction = XMVectorSet(0, -1, 0, 0);
		this->thrusters[TOP_RIGHT]->offset = XMVectorSet(10, 1, 0, 1);
		this->thrusters[TOP_RIGHT]->maximumMagnitude = turnMag;
		this->thrusters[TOP_RIGHT]->currentMagnitude = 0;

		Thruster* bottomLeftThruster = new Thruster();
		this->AddThruster(bottomLeftThruster);
		this->thrusters[BOTTOM_LEFT]->requireResting = false;
		this->thrusters[BOTTOM_LEFT]->direction = XMVectorSet(0, 1, 0, 0);
		this->thrusters[BOTTOM_LEFT]->offset = XMVectorSet(-10, -1, 0, 1);
		this->thrusters[BOTTOM_LEFT]->maximumMagnitude = turnMag;
		this->thrusters[BOTTOM_LEFT]->currentMagnitude = 0;

		Thruster* bottomRightThruster = new Thruster();
		this->AddThruster(bottomRightThruster);
		this->thrusters[BOTTOM_RIGHT]->requireResting = false;
		this->thrusters[BOTTOM_RIGHT]->direction = XMVectorSet(0, 1, 0, 0);
		this->thrusters[BOTTOM_RIGHT]->offset = XMVectorSet(10, -1, 0, 1);
		this->thrusters[BOTTOM_RIGHT]->maximumMagnitude = turnMag;
		this->thrusters[BOTTOM_RIGHT]->currentMagnitude = 0;

		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::ACT;

		this->missileLauncher = new TestMissileLauncher(this);
		this->decentMissileLauncher = new DecentMissileLauncher(this);
		this->ineffectiveMissileLauncher = new IneffectiveMissileLauncher(this);
		this->objectType = ObjectTypes::GoodGuy;

		this->brain.motorCortex.Calibrate();
		this->dragCoefficient = .1f;
		this->mass = 500;
		this->rotationalInertia = 50;
	}

	void GoodGuy::Act()
	{

		for(int a = 0; a < 2; ++a)
		{
			if(this->thrusterThrottleStatus[a] == 1)
			{
				float onePercent = this->thrusters[a]->maximumMagnitude *.01f;
				this->thrusters[a]->currentMagnitude = min(this->thrusters[a]->maximumMagnitude,
					this->thrusters[a]->currentMagnitude + onePercent);

				this->thrusters[a]->activated = true;
			}
			else if(this->thrusterThrottleStatus[a] == -1)
			{
				float onePercent = this->thrusters[a]->maximumMagnitude *.01f;
				this->thrusters[a]->currentMagnitude -= onePercent;

				if(this->thrusters[a]->currentMagnitude <= 0)
				{
					this->thrusters[a]->currentMagnitude = 0;
					this->thrusters[a]->activated = false;
				}
			}
			else
			{
				continue;
			}
		}

		float rollMag = .1f;
		if(this->rolling[0]) // roll left
		{
			TimedThrusterFire thrusterFire = {1, this->thrusters[TOP_LEFT], abs(rollMag)};
			TimedThrusterFire thrusterFire2 = {1, this->thrusters[BOTTOM_RIGHT], abs(rollMag)};

			this->brain.motorCortex.AddCommand(0, thrusterFire);
			this->brain.motorCortex.AddCommand(0, thrusterFire2);	

			//TimedThrusterFire thrusterFire3 = {1, this->thrusters[TOP_RIGHT], abs(rollMag)};
			//TimedThrusterFire thrusterFire4 = {1, this->thrusters[BOTTOM_LEFT], abs(rollMag)};

			//this->brain.motorCortex.AddCommand(1, thrusterFire3);
			//this->brain.motorCortex.AddCommand(1, thrusterFire4);		
		}
		if(this->rolling[1]) // roll right
		{
			TimedThrusterFire thrusterFire = {1, this->thrusters[TOP_RIGHT], abs(rollMag)};
			TimedThrusterFire thrusterFire2 = {1, this->thrusters[BOTTOM_LEFT], abs(rollMag)};

			this->brain.motorCortex.AddCommand(0, thrusterFire);
			this->brain.motorCortex.AddCommand(0, thrusterFire2);	

			//TimedThrusterFire thrusterFire3 = {1, this->thrusters[TOP_LEFT], abs(rollMag)};
			//TimedThrusterFire thrusterFire4 = {1, this->thrusters[BOTTOM_RIGHT], abs(rollMag)};

			//this->brain.motorCortex.AddCommand(1, thrusterFire3);
			//this->brain.motorCortex.AddCommand(1, thrusterFire4);		
		}

		if (this->rolling[2])
		{
			TimedThrusterFire thrusterFire = { 1, this->thrusters[BACK_LEFT], .1f };
			TimedThrusterFire thrusterFire2 = { 1, this->thrusters[FRONT_RIGHT], .1f };

			this->brain.motorCortex.AddCommand(0, thrusterFire);
			this->brain.motorCortex.AddCommand(0, thrusterFire2);
		}
		else if(this->rolling[3])
		{
			TimedThrusterFire thrusterFire = { 1, this->thrusters[BACK_RIGHT], .1f };
			TimedThrusterFire thrusterFire2 = { 1, this->thrusters[FRONT_LEFT], .1f };

			this->brain.motorCortex.AddCommand(0, thrusterFire);
			this->brain.motorCortex.AddCommand(0, thrusterFire2);
		}

#ifndef _NO_GRAPHICS
		using std::to_string;

		int totalPercentBC = static_cast<int>((this->thrusters[BACK_CENTER]->currentMagnitude / this->thrusters[BACK_CENTER]->maximumMagnitude) * 100);
		int totalPercentFL = static_cast<int>((this->thrusters[FRONT_LEFT]->currentMagnitude / this->thrusters[FRONT_LEFT]->maximumMagnitude) * 100);
		int totalPercentFR = static_cast<int>((this->thrusters[FRONT_RIGHT]->currentMagnitude / this->thrusters[FRONT_RIGHT]->maximumMagnitude) * 100);
		int totalPercentBL = static_cast<int>((this->thrusters[BACK_LEFT]->currentMagnitude / this->thrusters[BACK_LEFT]->maximumMagnitude) * 100);
		int totalPercentBR = static_cast<int>((this->thrusters[BACK_RIGHT]->currentMagnitude / this->thrusters[BACK_RIGHT]->maximumMagnitude) * 100);

		UIRenderer::SetDebugString(thrusterDebugIndexFront, "Thruster Levels Front: " + 
			to_string(totalPercentFL) + "% " + to_string(0) + "% " + to_string(totalPercentFR) + "%");

		UIRenderer::SetDebugString(thrusterDebugIndexBack, "Thruster Levels Rear: " + 
			to_string(totalPercentBL) + "% " + to_string(totalPercentBC) + "% " + to_string(totalPercentBR) + "%");
#endif

		this->brain.Update();
		__super::Act();
		if(this->grenadeActive)
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
		}

#ifndef _NO_GRAPHICS
		{
			XMFLOAT4 orien;
			XMStoreFloat4(&orien, XMVectorMultiply(this->qOrientation, XMVectorReplicate(180.f / 3.1415f)));

			UIRenderer::SetDebugString(orientationIndex, "Orientation: (" + std::to_string(orien.x) + ", " +
				std::to_string(orien.y) + ", " + std::to_string(orien.z) + ")");
		}
		{
			/*XMFLOAT4 orien;
			//XMStoreFloat4(&orien, XMVectorMultiply(this->pitchYawRollVelocity, XMVectorReplicate(180.f / 3.1415f)));
			XMStoreFloat4(&orien, XMVectorMultiply(this->pitchYawRollVelocity, XMVectorReplicate(180.f / 3.1415f)));

			UIRenderer::SetDebugString(orientationVelocityIndex, "Orientation Velocity: (" + std::to_string(orien.x) + ", " +
				std::to_string(orien.y) + ", " + std::to_string(orien.z) + ")");*/
		}
		{
			XMFLOAT4 orien;
			XMStoreFloat4(&orien, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), this->qOrientation));

			UIRenderer::SetDebugString(headingIndex, "Heading: (" + std::to_string(orien.x) + ", " +
				std::to_string(orien.y) + ", " + std::to_string(orien.z) + ")");
		}
		{
			XMFLOAT4 orien;
			XMStoreFloat4(&orien, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), this->qOrientation));

			UIRenderer::SetDebugString(upIndex, "Up Vector: (" + std::to_string(orien.x) + ", " +
				std::to_string(orien.y) + ", " + std::to_string(orien.z) + ")");
		}
#endif
	}
	
	bool lookUp = false;
	void GoodGuy::ReceiveCommand(const Command& command)
	{
		switch(command.command)
		{
		case StrafeFoward:
			{
				this->thrusterThrottleStatus[BACK_CENTER] = ((command.flags & 1) ? 1 : 0);
			}
			break;
		case StrafeBackward:
			{
				this->thrusterThrottleStatus[BACK_CENTER] = ((command.flags & 1) ? -1 : 0);
			}
			break;
		case StrafeLeft:
			{
				if(command.flags & 1)
				{
					this->rolling[1] = true;
				}
				else
				{
					this->rolling[1] = false;
				}
			}
			break;
		case StrafeRight:
			{
				if(command.flags & 1)
				{
					this->rolling[0] = true;
				}
				else
				{
					this->rolling[0] = false;
				}
			}
			break;
		case RollLeft:
		{
			if (command.flags & 1)
			{
				this->rolling[2] = true;
			}
			else
			{
				this->rolling[2] = false;
			}
		}
		break;
		case RollRight:
		{
			if (command.flags & 1)
			{
				this->rolling[3] = true;
			}
			else
			{
				this->rolling[3] = false;
			}
		}
		break;
		case Look:
			{
				if(command.arg1 != 0) // Horizontal look
				{
					if(command.arg1 > 0)
					{
						TimedThrusterFire thrusterFire = {1, this->thrusters[BACK_LEFT], abs(command.arg1)};
						TimedThrusterFire thrusterFire2 = {1, this->thrusters[FRONT_RIGHT], abs(command.arg1)};

						this->brain.motorCortex.AddCommand(0, thrusterFire);
						this->brain.motorCortex.AddCommand(0, thrusterFire2);						
					}
					else
					{
						TimedThrusterFire thrusterFire = {1, this->thrusters[BACK_RIGHT], abs(command.arg1)};
						TimedThrusterFire thrusterFire2 = {1, this->thrusters[FRONT_LEFT], abs(command.arg1)};

						this->brain.motorCortex.AddCommand(0, thrusterFire);
						this->brain.motorCortex.AddCommand(0, thrusterFire2);
					}
				}

				if(command.arg2 != 0) // Vertical look
				{
					if(command.arg2 < 0)
					{
						TimedThrusterFire thrusterFire = {1, this->thrusters[BOTTOM_BACK], abs(command.arg2)};
						TimedThrusterFire thrusterFire2 = {1, this->thrusters[TOP_FRONT], abs(command.arg2)};

						this->brain.motorCortex.AddCommand(0, thrusterFire);
						this->brain.motorCortex.AddCommand(0, thrusterFire2);						
					}
					else
					{
						TimedThrusterFire thrusterFire = {1, this->thrusters[BOTTOM_FRONT], abs(command.arg2)};
						TimedThrusterFire thrusterFire2 = {1, this->thrusters[TOP_BACK], abs(command.arg2)};

						this->brain.motorCortex.AddCommand(0, thrusterFire);
						this->brain.motorCortex.AddCommand(0, thrusterFire2);
					}
				}
			}
			break;
		case Jump:
			{
				if(command.flags & 1)
				{
					this->thrusters[BOTTOM_CENTER]->activated = true;
				}
				else
				{
					this->thrusters[BOTTOM_CENTER]->activated = false;
				}
			}
			break;
		case Shoot:
			{
				//this->pitchYawRollVelocity = XMVectorSet(0,0,0,0);
				this->velocity = XMVectorSet(0,0,0,0);
				//this->grenadeActive = !this->grenadeActive;
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
				//this->velocity = XMVectorSet(0,0,0,0);
			}
			break;
		}
	}
}