#pragma once
#include "..\..\..\Util\MathUtil.h"

namespace SimulationEngine
{
	struct Thruster
	{
		DirectX::XMVECTOR offset;
		DirectX::XMVECTOR direction;
		float maximumMagnitude;
		float currentMagnitude;
		bool activated;
		bool requireResting;
		float airControlFactor;
		Thruster(const DirectX::XMVECTOR& offset = DirectX::XMVectorSet(0, 0, 0, 0),
			const DirectX::XMVECTOR& direction = DirectX::XMVectorSet(0, 0, 1, 0),
			float maximumMagnitude = 5, bool requireResting = true) :
		offset(offset), direction(direction), maximumMagnitude(maximumMagnitude), currentMagnitude(0), activated(false), requireResting(requireResting), airControlFactor(1) {}
	};

	struct TimedThrusterFire
	{
		int frameCount;
		Thruster* thruster;
		float magnitude;
	};

	struct MoveableParameters
	{
		DirectX::XMVECTOR location;
		DirectX::XMVECTOR pitchYawRoll;
		DirectX::XMVECTOR velocity;
		DirectX::XMVECTOR pitchYawRollVelocity;
		float mass;

		MoveableParameters(const DirectX::XMVECTOR& location = DirectX::XMVectorSet(0, 0, 0, 1),
			const DirectX::XMVECTOR& pitchYawRoll = DirectX::XMQuaternionIdentity(),
			const DirectX::XMVECTOR& velocity = DirectX::XMVectorZero(),
			const DirectX::XMVECTOR& pitchYawRollVelocity = DirectX::XMVectorZero(),
			//const DirectX::XMVECTOR& pitchYawRollVelocity = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMVectorZero()),
			float mass = 1) :
		location(location), pitchYawRoll(pitchYawRoll), velocity(velocity), pitchYawRollVelocity(pitchYawRollVelocity), mass(mass) {}
	};

	//Interface denoting a class with location, velocity, acceleration, etc
	class Moveable
	{
	public:
		DirectX::XMVECTOR qOrientation = DirectX::XMQuaternionIdentity();
		DirectX::XMVECTOR location = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR velocity = DirectX::XMVectorZero();
		DirectX::XMVECTOR pitchYawRollVelocity = DirectX::XMVectorZero();
		DirectX::XMVECTOR constraintTargetTest = DirectX::XMQuaternionIdentity();
		float mass = 1;
		float inverseMass = 1;
		float dragCoefficient = .05f;
		float rotationalInertia = 1;
		float rotationalCoefficient = 0;
		DirectX::XMMATRIX inverseMassMatrix = DirectX::XMMatrixIdentity();
		bool resting = false;

	protected:
		DirectX::XMVECTOR appliedForce = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR appliedTorque = DirectX::XMVectorSet(0, 0, 0, 0); //pitch, yaw, roll
		DirectX::XMVECTOR facingVector = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR defaultPosition = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR defaultOrientation = DirectX::XMQuaternionIdentity();

		std::vector<Thruster*> thrusters;
		std::vector<TimedThrusterFire> thrusterFires;
		int thrusterFireCount = 0;

	public:
		void Move();
		void ApplyThrusters();
		void UpdateForces();
		void UpdatePosition();
		void UpdateForces(const float dt);
		void UpdatePosition(const float dt);
		void ResetForces();
		void StopAllThrusters();
		void AddThruster(Thruster* thruster) {this->thrusters.push_back(thruster);}
		void ApplyForceAtPoint(const DirectX::XMVECTOR& point, const DirectX::XMVECTOR& force, const bool singleFrame = false);
		void ApplyForceAtPoint(const DirectX::XMVECTOR& point, const DirectX::XMVECTOR& force, const float torqueFactor);
		void ApplyTorqueAtPoint(const DirectX::XMVECTOR& point, const DirectX::XMVECTOR& torque);
		void ApplyTorque(const DirectX::XMVECTOR& torque, const bool singleFrame = false);
		void FireThruster(Thruster* thruster, int frameCount, float magnitude);
		void FireThrusterPercent(Thruster* thruster, int frameCount, float magnitude);
		DirectX::XMVECTOR GetLocalVelocity() const;
		DirectX::XMVECTOR GetLocalVector(const DirectX::XMVECTOR& globalVector) const;
		const DirectX::XMVECTOR minVel = DirectX::XMVectorReplicate(-10);
		const DirectX::XMVECTOR maxVel = DirectX::XMVectorReplicate(10);
		const DirectX::XMVECTOR minRotVel = DirectX::XMVectorReplicate(-3.1415);
		const DirectX::XMVECTOR maxRotVel = DirectX::XMVectorReplicate(3.1415);
		void ClampVelocities()
		{
			//this->velocity = DirectX::XMVectorClamp(this->velocity, minVel, maxVel);
			//this->pitchYawRollVelocity = DirectX::XMVectorClamp(this->pitchYawRollVelocity, minRotVel, maxRotVel);
		}

		virtual ~Moveable();

		void GetSerializedForm(char* data) const;
		const int GetSerializedLength() const;

	protected:
		Moveable(const MoveableParameters& parameters);
		Moveable();

	public:
#pragma region Getters and Setters
		//DirectX::XMVECTOR GetQAngularVelocity() const {return this->qAngularVelocity;}
		const std::vector<Thruster*>& GetThrusters() const {return this->thrusters;}

		void SetPitchYawRoll(const DirectX::XMVECTOR& quaternionOrientation) { this->qOrientation = quaternionOrientation; }
		void SetVelocity(const DirectX::XMVECTOR& velocity)
		{
			//this->velocity = velocity;
			this->velocity = DirectX::XMVectorClamp(velocity, minVel, maxVel);
		}
		void SetPitchYawRollVelocity(const DirectX::XMVECTOR& pitchYawRollVelocity) {
			this->pitchYawRollVelocity = DirectX::XMVectorClamp(pitchYawRollVelocity, minRotVel, maxRotVel);
			//this->pitchYawRollVelocity = pitchYawRollVelocity;
		}
		//void SetQAngularVelocity(const DirectX::XMVECTOR& qAngularVelocity) { this->qAngularVelocity = qAngularVelocity; }
		void SetMass(const float mass) {
			this->mass = mass;
			this->inverseMass = 1.f / mass;
			this->inverseMassMatrix = MathUtil::diagonal(this->inverseMass);
		}
		void ResetPosition() { this->location = this->defaultPosition; this->SetPitchYawRoll(this->defaultOrientation); }
#pragma endregion
	};
}