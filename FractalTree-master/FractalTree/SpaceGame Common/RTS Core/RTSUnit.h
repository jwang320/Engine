#pragma once
#include <deque>
#include <unordered_set>
#include "..\Common Source\Simulation Engine\Physical Simulation\Actor.h"

namespace RTSCore
{
	enum class MoveState : char
	{
		STOP,
		STRAIGHT,
		LEFT,
		RIGHT,
		MOVE
	};

	class RTSUnitOrder;

	class RTSUnit : public SimulationEngine::Actor
	{
	protected:
		bool selected;
		std::deque<RTSUnitOrder*> orderQueue;
		std::vector<bool> leftTurnThrusters; //thrusters to fire when turning left
		std::vector<bool> rightTurnThrusters; //thrusters to fire when turning right
		std::vector<bool> straightThrusters; //thrusters to fire when moving straight
		MoveState moveState;
		DirectX::XMVECTOR moveDestination; //if this unit's state is currently move, this is destination
		std::unordered_set<int> availableOrders; //order types this unit is allowed to accept

	protected:
		RTSUnit(int owningPlayer, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
			const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

	public:
		virtual void Act() override;
		void QueueOrder(RTSUnitOrder* order, bool overrideCurrent = true);
		bool Selected() const { return this->selected; }
		void SetSelected(bool selected) { this->selected = selected; }
		void TurnLeft();
		void TurnRight();
		void GoStraight();
		void Stop();
		void SetMoveState(MoveState moveState) { this->moveState = moveState; }
		void SetMoveDestination(const DirectX::XMVECTOR& destination) { this->moveDestination = destination; }

	protected:
		virtual void initializeThrusters();

	private:
		void handlePathing();
	};
}