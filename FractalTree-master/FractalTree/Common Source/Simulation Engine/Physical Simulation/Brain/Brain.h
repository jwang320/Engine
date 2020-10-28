#pragma once
#include <queue>

namespace SimulationEngine
{
	class Actor;
	struct Thruster;

	struct BrainCommand
	{
		int when;
		TimedThrusterFire thrusterFire;
	};

	struct BrainCommandCompare
	{
		bool operator()(const BrainCommand& a, const BrainCommand& b) const
		{
			return a.when > b.when;
		}
	};

	class BrainArea
	{

	};

	//a thruster combination describes
	//a combination of thrusters and their ratios
	//to apply force affecting only a single axis of rotation
	//or translation
	struct ThrusterCombination
	{
		std::vector<Thruster> thrusters;
		std::vector<float> burnRatios;
	};

	struct ThrusterAxisMagnitude
	{
		Thruster* thruster;
		float magnitude;
	};

	class MotorCortex : public BrainArea
	{
	public:
		bool active = false;
		int currentFrame = 0;
		Actor* const owner;
		std::priority_queue<BrainCommand, std::vector<BrainCommand>, BrainCommandCompare> commandQueue;
		DirectX::XMVECTOR targetLocation = DirectX::XMVectorReplicate(0);
		DirectX::XMVECTOR targetPitchYawRoll = DirectX::XMVectorReplicate(0);
		DirectX::XMVECTOR targetVelocity = DirectX::XMVectorReplicate(0);
		DirectX::XMVECTOR targetPitchYawRollVelocity = DirectX::XMVectorReplicate(0);

		bool monitorXVelocity = true;
		bool monitorYVelocity = true;
		bool monitorZVelocity = true;
		std::vector<DirectX::XMVECTOR> thrusterTorques;
		std::vector<DirectX::XMVECTOR> thrusterForces;

		double axisWeights[6];

	public:
		MotorCortex(Actor* owner);
		void AddCommand(int when, const TimedThrusterFire& thrusterFire);
		void Update();
		void Calibrate();

	private:
		void levelX();
		void levelZ();
		void approachTargetAngularVelocity();
		void approachLevel();
		void approachTargetVelocity();
		DirectX::XMVECTOR calculateRequiredForce() const; //calculate currently required force to meet target velocity;
		DirectX::XMVECTOR calculateRequiredTorque() const;
	};

	class Brain
	{
	public:
		Actor* const owner;
		int currentFrame = 0;
		MotorCortex motorCortex;

	public:
		Brain(Actor* owner);	
		void Update();
	};
}