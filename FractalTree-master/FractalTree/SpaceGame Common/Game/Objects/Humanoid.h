#pragma once

namespace SpaceGameCore
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

	class Humanoid : public SimulationEngine::Actor
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
		Humanoid(const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
			const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

		virtual void Act() override;
		virtual void ReceiveCommand(const SimulationEngine::Command& command) override;
	};
}