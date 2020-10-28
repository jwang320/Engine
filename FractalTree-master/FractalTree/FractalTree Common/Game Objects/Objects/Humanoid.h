#pragma once

namespace SimulationEngine
{
	class TestMissileLauncher;
	class DecentMissileLauncher;
	class IneffectiveMissileLauncher;

	enum StrafeDirection
	{
		Foward,
		Backward,
		Left,
		Right
	};

	class Humanoid : public Actor
	{
	protected:
		bool gunActive;
		bool grenadeActive;
		bool gun2Active;
		float turnSpeedFactor;
		TestMissileLauncher* missileLauncher;
		DecentMissileLauncher* decentMissileLauncher;
		IneffectiveMissileLauncher* ineffectiveMissileLauncher;
		bool freeMove = false;
		bool moveDir[4];

	public:
		Humanoid(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
		virtual void ReceiveCommand(const Command& command) override;
	};
}