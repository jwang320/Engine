#include "pch.h"
#include "RTSUnit.h"
#include "RTSUnitOrder.h"
#include "UIRenderer.h"
#include <string>
using namespace SimulationEngine;
using namespace DirectX;

namespace RTSCore
{
	RTSUnit::RTSUnit(int owningPlayer, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters, ActorParameters(), owningPlayer)
	{
		this->initializeThrusters();
		this->moveState = MoveState::STOP;
		this->rotationalCoefficient = .5f;
		this->otherDragCoefficient = .5f;
	}

	void RTSUnit::Act()
	{
		if (!this->orderQueue.empty())
		{
			RTSUnitOrder* const order = this->orderQueue.front();
			if (order->CheckCompletionCondition())
			{
				this->orderQueue.pop_front();
				this->Stop();
				order->Complete();

				delete order;
			}

			if (!this->orderQueue.empty())
			{
				this->orderQueue.front()->Update();
			}
		}

		this->handlePathing();
		__super::Act();
	}

	void RTSUnit::handlePathing()
	{
		if (this->moveState == MoveState::MOVE)
		{
			const XMVECTOR targetFacing = XMVector3Normalize(XMVectorSubtract(this->moveDestination, this->location));

			XMFLOAT4 angleFromTargetToX;
			XMStoreFloat4(&angleFromTargetToX, XMVector3AngleBetweenNormals(targetFacing, XMVectorSet(1, 0, 0, 0)));

			XMFLOAT4 targetFacingF;
			XMStoreFloat4(&targetFacingF, targetFacing);

			if (targetFacingF.z < 0)
			{
				angleFromTargetToX.x *= -1;
			}
			XMFLOAT4 rotatedFacingVector;
			XMStoreFloat4(&rotatedFacingVector, XMVector3Transform(this->GetFacing(), XMMatrixRotationY(angleFromTargetToX.x)));

			if (rotatedFacingVector.x > 0)
			{
				if (rotatedFacingVector.z < -.1f)
				{
					this->TurnRight();
				}
				else if (rotatedFacingVector.z > .1f)
				{
					this->TurnLeft();
				}
				else
				{
					this->Stop();
					this->GoStraight();
				}
			}
			else
			{
				if (rotatedFacingVector.z < 0.f)
				{
					this->TurnRight();
				}
				else if (rotatedFacingVector.z > 0.f)
				{
					this->TurnLeft();
				}
				else
				{
					this->Stop();
					this->GoStraight();
				}
			}
		}
	}

	void RTSUnit::QueueOrder(RTSUnitOrder* order, bool overrideCurrent)
	{
		if (this->availableOrders.find(order->getOrderType()) == this->availableOrders.end())
		{
			return;
		}

		if (overrideCurrent)
		{
			this->Stop();
			this->SetMoveState(MoveState::STOP);
			if (!this->orderQueue.empty())
			{
				this->orderQueue.front()->Cancel();
			}
			for (RTSUnitOrder* const order : this->orderQueue)
			{
				delete order;
			}
			
			this->orderQueue.clear();
		}

		order->Initialize();
		this->orderQueue.push_back(order);
	}

	void RTSUnit::TurnLeft()
	{
		//this->moveState = MoveState::LEFT;
		for (int a = 0; a < this->thrusters.size(); ++a)
		{
			if (this->leftTurnThrusters[a])
			{
				this->thrusters[a]->activated = true;
			}
			else
			{
				this->thrusters[a]->activated = false;
			}
		}
	}

	void RTSUnit::TurnRight()
	{
		//this->moveState = MoveState::RIGHT;
		for (int a = 0; a < this->thrusters.size(); ++a)
		{
			if (this->rightTurnThrusters[a])
			{
				this->thrusters[a]->activated = true;
			}
			else
			{
				this->thrusters[a]->activated = false;
			}
		}
	}

	void RTSUnit::GoStraight()
	{
		//this->moveState = MoveState::STRAIGHT;
		for (int a = 0; a < this->thrusters.size(); ++a)
		{
			if (this->straightThrusters[a])
			{
				this->thrusters[a]->activated = true;
			}
			else
			{
				this->thrusters[a]->activated = false;
			}
		}
	}

	void RTSUnit::Stop()
	{
		//this->moveState = MoveState::STOP;
		for (int a = 0; a < this->thrusters.size(); ++a)
		{
			this->thrusters[a]->activated = false;
		}
	}

	void RTSUnit::initializeThrusters()
	{
		Thruster* const leftThruster = new Thruster();
		Thruster* const leftThrusterReverse = new Thruster();

		Thruster* const rightThruster = new Thruster();
		Thruster* const rightThrusterReverse = new Thruster();

		leftThruster->direction = XMVectorSet(0, 0, 1, 0);
		rightThruster->direction = XMVectorSet(0, 0, 1, 0);
		leftThrusterReverse->direction = XMVectorSet(0, 0, -1, 0);
		rightThrusterReverse->direction = XMVectorSet(0, 0, -1, 0);

		leftThruster->offset = XMVectorSet(-1, 0, 0, 0);
		rightThruster->offset = XMVectorSet(1, 0, 0, 0);
		leftThrusterReverse->offset = XMVectorSet(-1, 0, 0, 0);
		rightThrusterReverse->offset = XMVectorSet(1, 0, 0, 0);


		const float speed = .00025f;
		leftThruster->maximumMagnitude = speed * this->mass;
		rightThruster->maximumMagnitude = speed * this->mass;
		leftThruster->currentMagnitude = speed * this->mass;
		rightThruster->currentMagnitude = speed * this->mass;
		leftThruster->requireResting = false;
		rightThruster->requireResting = false;

		leftThrusterReverse->maximumMagnitude = speed * this->mass;
		rightThrusterReverse->maximumMagnitude = speed * this->mass;
		leftThrusterReverse->currentMagnitude = speed * this->mass;
		rightThrusterReverse->currentMagnitude = speed * this->mass;
		leftThrusterReverse->requireResting = false;
		rightThrusterReverse->requireResting = false;

		this->leftTurnThrusters.push_back(true);
		this->leftTurnThrusters.push_back(false);
		this->leftTurnThrusters.push_back(false);
		this->leftTurnThrusters.push_back(true);

		this->rightTurnThrusters.push_back(false);
		this->rightTurnThrusters.push_back(true);
		this->rightTurnThrusters.push_back(true);
		this->rightTurnThrusters.push_back(false);
		this->straightThrusters.push_back(true);
		this->straightThrusters.push_back(false);
		this->straightThrusters.push_back(true);
		this->straightThrusters.push_back(false);


		this->AddThruster(leftThruster);
		this->AddThruster(leftThrusterReverse);
		this->AddThruster(rightThruster);
		this->AddThruster(rightThrusterReverse);
	}
}
