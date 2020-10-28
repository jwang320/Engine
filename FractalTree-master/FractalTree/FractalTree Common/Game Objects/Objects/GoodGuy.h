#pragma once

namespace SimulationEngine
{
	class TestMissileLauncher;
	class DecentMissileLauncher;
	class IneffectiveMissileLauncher;

	class GoodGuy : public Actor
	{
	public:
		enum ThrustersWhich
		{
			BOTTOM_CENTER,
			BACK_CENTER,
			BACK_LEFT,
			BACK_RIGHT,
			FRONT_LEFT,
			FRONT_RIGHT,
			TOP_FRONT,
			TOP_BACK,
			BOTTOM_FRONT,
			BOTTOM_BACK,
			TOP_LEFT,
			TOP_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT
		};
	protected:
		bool gunActive;
		bool grenadeActive;
		bool gun2Active;
		float turnSpeedFactor;
		TestMissileLauncher* missileLauncher;
		DecentMissileLauncher* decentMissileLauncher;
		IneffectiveMissileLauncher* ineffectiveMissileLauncher;

		int thrusterThrottleStatus[14];
		float horizontalTurn;
		float verticalTurn;

		bool rolling[4];
	public:
		GoodGuy(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void ReceiveCommand(const Command& command) override;
		virtual void Act();
	};
}