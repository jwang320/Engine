#include "pch.h"
#include "Moveable.h"

using namespace DirectX;

namespace SimulationEngine
{
	Moveable::Moveable(const MoveableParameters& parameters) :
		location(parameters.location),
		defaultPosition(parameters.location),
		defaultOrientation(XMQuaternionRotationRollPitchYawFromVector(parameters.pitchYawRoll)),
		velocity(parameters.velocity),
		qOrientation(XMQuaternionRotationRollPitchYawFromVector(parameters.pitchYawRoll)),
		pitchYawRollVelocity(parameters.pitchYawRollVelocity),
		mass(parameters.mass)
	{

	}

	Moveable::Moveable()
	{

	}

	void Moveable::Move()
	{
		this->ApplyThrusters();
		this->UpdateForces();
		this->UpdatePosition();
	}

	static inline XMVECTOR Integrate(const XMVECTOR& q, const XMVECTOR& w) {
		const XMVECTOR halfW = XMVectorMultiply(w, XMVectorReplicate(.5f));
		const XMVECTOR spin = XMQuaternionMultiply(q, halfW);
		return XMVectorAdd(q, spin);
	}

	void Moveable::UpdateForces()
	{
		//Calculate the net acceleration for this frame from the applied force.
		//const XMVECTOR dragForce1 = XMVectorMultiply(XMVectorReplicate(-1 * this->otherDragCoefficient), this->GetVelocity());
		//this->ApplyForceAtPoint(this->GetLocation(), XMVectorMultiply(dragForce1, XMVectorReplicate(this->GetMass())));
		const XMVECTOR acceleration = XMVectorMultiply(XMVectorReciprocal(XMVectorReplicate(this->mass)), this->appliedForce);
		this->velocity = XMVectorAdd(this->velocity, acceleration);
		const XMVECTOR angularAcceleration = XMVectorMultiply(XMVectorReciprocal(XMVectorReplicate(this->rotationalInertia)), this->appliedTorque);
		//const XMVECTOR dragForce = XMVectorMultiply(XMVectorReplicate(-1 * rotationalCoefficient), this->pitchYawRollVelocity);
		this->pitchYawRollVelocity = XMVectorAdd(this->pitchYawRollVelocity, angularAcceleration);
		//5this->pitchYawRollVelocity = XMVectorAdd(this->pitchYawRollVelocity, dragForce);

		//Reset the applied force.
		this->appliedForce = XMVectorZero();
		this->appliedTorque = XMVectorZero();
	}

	void Moveable::ResetForces()
	{
		this->appliedForce = XMVectorZero();
		this->appliedTorque = XMVectorZero();
	}

	void Moveable::StopAllThrusters()
	{
		for (Thruster* t : this->thrusters)
		{
			t->activated = false;
			t->currentMagnitude = 0;
		}
	}

	const XMVECTOR minAccel = XMVectorReplicate(-20);
	const XMVECTOR maxAccel = XMVectorReplicate(20);
	const XMVECTOR minRotAccel = XMVectorReplicate(-3.145 / 2);
	const XMVECTOR maxRotAccel = XMVectorReplicate(3.1415 / 2);
	void Moveable::UpdateForces(const float dt)
	{
		//Calculate the net acceleration for this frame from the applied force.
		//const XMVECTOR dragForce1 = XMVectorMultiply(XMVectorReplicate(-1 * this->otherDragCoefficient), this->GetVelocity());
		//this->ApplyForceAtPoint(this->GetLocation(), XMVectorMultiply(dragForce1, XMVectorReplicate(this->GetMass())));
		XMVECTOR acceleration = XMVectorScale(XMVectorMultiply(XMVectorReciprocal(XMVectorReplicate(this->mass)), this->appliedForce), dt);
		XMVECTOR angularAcceleration = XMVectorScale(XMVectorMultiply(XMVectorReciprocal(XMVectorReplicate(this->rotationalInertia)), this->appliedTorque), dt);
		//const XMVECTOR dragForce = XMVectorMultiply(XMVectorReplicate(-1 * rotationalCoefficient), this->pitchYawRollVelocity);
#ifdef _DEBUG
/*
		if (getVectorLen(angularAcceleration) > 20 || getVectorLen(acceleration) > 20) {
			//int x = 0;
			//++x;
			acceleration = XMVectorZero();
			angularAcceleration = XMVectorZero();
		}*/
#endif
		acceleration = XMVectorClamp(acceleration, minAccel, maxAccel);
		angularAcceleration = XMVectorClamp(angularAcceleration, minRotAccel, maxRotAccel);
		this->velocity = XMVectorAdd(this->velocity, acceleration);
		this->pitchYawRollVelocity = XMVectorAdd(this->pitchYawRollVelocity, angularAcceleration);
		this->ClampVelocities();
		//5this->pitchYawRollVelocity = XMVectorAdd(this->pitchYawRollVelocity, dragForce);

		//Reset the applied force.
		this->appliedForce = XMVectorZero();
		this->appliedTorque = XMVectorZero();
	}

	void Moveable::UpdatePosition(const float dt)
	{
		this->location = XMVectorAdd(this->location, XMVectorScale(this->velocity, dt));
		this->location = XMVectorSetW(this->location, 1);
		this->qOrientation = XMQuaternionNormalize(this->qOrientation);
		this->qOrientation = Integrate(this->qOrientation, XMVectorScale(this->pitchYawRollVelocity, dt));
		//this->qOrientation = XMQuaternionMultiply(this->qOrientation, this->qAngularVelocity);
		this->facingVector = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), this->qOrientation);
	}

	void Moveable::UpdatePosition()
	{
		this->location = XMVectorAdd(this->location, this->velocity);
		this->location = XMVectorSetW(this->location, 1);
		this->qOrientation = XMQuaternionNormalize(this->qOrientation);
		this->qOrientation = Integrate(this->qOrientation, this->pitchYawRollVelocity);
		//this->qOrientation = XMQuaternionMultiply(this->qOrientation, this->qAngularVelocity);
		this->facingVector = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), this->qOrientation);
	}

	void Moveable::ApplyThrusters()
	{
		for(int a = 0; a < this->thrusterFireCount; ++a)
		{
			if(this->thrusterFires[a].frameCount <= 0)
			{
				Thruster* const thruster = this->thrusterFires[a].thruster;
				thruster->currentMagnitude -= this->thrusterFires[a].magnitude;
				if(fabs(thruster->currentMagnitude) <= fabs(thruster->maximumMagnitude) *.001f)
				{
					thruster->currentMagnitude = 0;
					thruster->activated = false;
				}

				--this->thrusterFireCount;

				if(a < this->thrusterFireCount)
				{
					this->thrusterFires[a] = this->thrusterFires[this->thrusterFireCount];
					--a;
				}
			}
			else
			{
				--this->thrusterFires[a].frameCount;
			}
		}

		for(unsigned int a = 0; a < this->thrusters.size(); ++a)
		{
			if(this->thrusters[a]->activated && (this->resting || !this->thrusters[a]->requireResting))
			{
				const float airControlFactor = this->resting ? 1 : this->thrusters[a]->airControlFactor;
				const XMMATRIX rotationQuaternion = XMMatrixRotationQuaternion(this->qOrientation);
				//Rotate the thruster to our orientation.
				const XMVECTOR applyDirection = XMVector3Transform(this->thrusters[a]->direction, rotationQuaternion);
				const XMVECTOR applyPoint = XMVector3Transform(this->thrusters[a]->offset, rotationQuaternion);

				this->ApplyForceAtPoint(XMVectorAdd(this->location, applyPoint),
					XMVectorMultiply(applyDirection, XMVectorReplicate(min(this->thrusters[a]->currentMagnitude, this->thrusters[a]->maximumMagnitude) * airControlFactor)));
			}
		}
	}

	void Moveable::ApplyForceAtPoint(const XMVECTOR& point, const XMVECTOR& force, const bool singleFrame)
	{
		this->appliedForce = XMVectorAdd(this->appliedForce, force);
		XMVECTOR leverArm = XMVectorSubtract(point, this->location);
		XMFLOAT4 mag;
		XMStoreFloat4(&mag, XMVectorAbs(XMVector3Dot(leverArm, leverArm)));

		if(mag.x <= 0)
		{
			return;
		}
		const XMVECTOR torque = XMVector3Cross(leverArm, force);
		this->appliedTorque = XMVectorAdd(this->appliedTorque, torque);
		//this->appliedTorque = XMVectorAdd(this->appliedTorque, XMVectorMultiply(torque, XMVectorReplicate(.01f)));
		/*XMFLOAT4 orient;
		XMStoreFloat4(&orient, this->pitchYawRoll);
		const XMVECTOR modXAxis = XMVector3Rotate(this->localX, XMQuaternionRotationAxis(this->localZ, -orient.z));

		const XMVECTOR modYAxis = XMVector3Rotate(XMVector3Rotate(this->localY, XMQuaternionRotationAxis(this->localZ, -orient.z)),
			XMQuaternionRotationAxis(modXAxis, -orient.x));

		leverArm = XMVector3Rotate(leverArm, XMQuaternionRotationAxis(localZ, -orient.z));
		leverArm = XMVector3Rotate(leverArm, XMQuaternionRotationAxis(modXAxis, -orient.x));
		leverArm = XMVector3Rotate(leverArm, XMQuaternionRotationAxis(modYAxis, -orient.y));


		XMVECTOR tDir = XMVector3Rotate(force, XMQuaternionRotationAxis(localZ, -orient.z));
		tDir = XMVector3Rotate(tDir, XMQuaternionRotationAxis(modXAxis, -orient.x));
		tDir = XMVector3Rotate(tDir, XMQuaternionRotationAxis(modYAxis, -orient.y));

		const XMVECTOR torque = XMVector3Cross(leverArm, tDir);
		this->appliedTorque = XMVectorAdd(this->appliedTorque, torque);*/
	}

	void Moveable::ApplyForceAtPoint(const XMVECTOR& point, const XMVECTOR& force, const float torqueFactor)
	{
		XMVECTOR leverArm = XMVectorSubtract(point, this->location);
		this->appliedForce = XMVectorAdd(this->appliedForce, force);
		XMFLOAT4 mag;
		XMStoreFloat4(&mag, XMVectorAbs(XMVector3Dot(leverArm, leverArm)));

		if (mag.x <= 0 || torqueFactor == 0)
		{
			return;
		}
		const XMVECTOR torque = XMVector3Cross(leverArm, force);
		this->appliedTorque = XMVectorAdd(this->appliedTorque, XMVectorMultiply(torque, XMVectorReplicate(torqueFactor)));

		/*XMFLOAT4 orient;
		XMStoreFloat4(&orient, this->pitchYawRoll);
		const XMVECTOR modXAxis = XMVector3Rotate(this->localX, XMQuaternionRotationAxis(this->localZ, -orient.z));

		const XMVECTOR modYAxis = XMVector3Rotate(XMVector3Rotate(this->localY, XMQuaternionRotationAxis(this->localZ, -orient.z)),
			XMQuaternionRotationAxis(modXAxis, -orient.x));

		leverArm = XMVector3Rotate(leverArm, XMQuaternionRotationAxis(localZ, -orient.z));
		leverArm = XMVector3Rotate(leverArm, XMQuaternionRotationAxis(modXAxis, -orient.x));
		leverArm = XMVector3Rotate(leverArm, XMQuaternionRotationAxis(modYAxis, -orient.y));


		XMVECTOR tDir = XMVector3Rotate(force, XMQuaternionRotationAxis(localZ, -orient.z));
		tDir = XMVector3Rotate(tDir, XMQuaternionRotationAxis(modXAxis, -orient.x));
		tDir = XMVector3Rotate(tDir, XMQuaternionRotationAxis(modYAxis, -orient.y));

		const XMVECTOR torque = XMVector3Cross(leverArm, tDir);
		this->appliedTorque = XMVectorAdd(this->appliedTorque, XMVectorMultiply(torque, XMVectorReplicate(torqueFactor)));*/
	}

	void Moveable::ApplyTorqueAtPoint(const XMVECTOR& point, const XMVECTOR& torque)
	{
		const XMVECTOR vecToCenter = XMVectorSubtract(point, this->location);
	}

	void Moveable::ApplyTorque(const XMVECTOR& torque, const bool singleFrame)
	{
		this->appliedTorque = XMVectorAdd(this->appliedTorque, torque);
	}

	void Moveable::FireThruster(Thruster* thruster, int frameCount, float magnitude)
	{
		TimedThrusterFire thrusterFire = {frameCount, thruster, thruster->currentMagnitude + magnitude};
		thruster->activated = true;
		thruster->currentMagnitude+= thrusterFire.magnitude;
		thruster->currentMagnitude = thruster->currentMagnitude <= thruster->maximumMagnitude ? thruster->currentMagnitude : thruster->maximumMagnitude;

		if(this->thrusterFireCount < this->thrusterFires.size())
		{
			this->thrusterFires[this->thrusterFireCount] = thrusterFire;
		}
		else
		{
			this->thrusterFires.push_back(thrusterFire);
		}
		
		++this->thrusterFireCount;
	}

	void Moveable::FireThrusterPercent(Thruster* thruster, int frameCount, float magnitude)
	{
		this->FireThruster(thruster, frameCount, thruster->maximumMagnitude * magnitude);
	}

	XMVECTOR Moveable::GetLocalVector(const XMVECTOR& globalVector) const
	{
		const XMVECTOR velLen = XMVector3Length(globalVector);
		XMFLOAT4 len;
		XMStoreFloat4(&len, velLen);
		if (len.x == 0)
		{
			return XMVectorSet(0, 0, 0, 0);
		}

		const XMVECTOR velNorm = XMVectorDivide(globalVector, velLen);
		//forward velocity
		const XMVECTOR localZVelocity = XMVectorMultiply(velLen, XMVector3Dot(this->facingVector, velNorm));

		//left - right velocity
		const XMVECTOR localXAxis = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), this->qOrientation);
		const XMVECTOR localXVelocity = XMVector3Dot(localXAxis, globalVector);

		const XMVECTOR localYAxis = XMVector3Cross(this->facingVector, localXAxis);
		const XMVECTOR localYVelocity = XMVector3Dot(localYAxis, globalVector);

		XMFLOAT4 lXV, lYV, lZV;
		XMStoreFloat4(&lXV, localXVelocity);
		XMStoreFloat4(&lYV, localYVelocity);
		XMStoreFloat4(&lZV, localZVelocity);

		return XMVectorSet(lXV.x, lYV.y, lZV.z, 0);
	}

	XMVECTOR Moveable::GetLocalVelocity() const
	{
		const XMVECTOR velLen = XMVector3Length(this->velocity);
		XMFLOAT4 len;
		XMStoreFloat4(&len, velLen);
		if (len.x == 0)
		{
			return XMVectorSet(0, 0, 0, 0);
		}

		const XMVECTOR velNorm = XMVectorDivide(this->velocity, velLen);
		//forward velocity
		const XMVECTOR localZVelocity = XMVectorMultiply(velLen, XMVector3Dot(this->facingVector, velNorm));

		//left - right velocity
		const XMVECTOR localXAxis  = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), this->qOrientation);
		const XMVECTOR localXVelocity = XMVector3Dot(localXAxis, this->velocity);

		const XMVECTOR localYAxis = XMVector3Cross(this->facingVector, localXAxis);
		const XMVECTOR localYVelocity = XMVector3Dot(localYAxis, this->velocity);

		XMFLOAT4 lXV, lYV, lZV;
		XMStoreFloat4(&lXV, localXVelocity);
		XMStoreFloat4(&lYV, localYVelocity);
		XMStoreFloat4(&lZV, localZVelocity);

		return XMVectorSet(lXV.x, lYV.y, lZV.z, 0);
	}

	Moveable::~Moveable()
	{
		for(unsigned int a = 0; a < this->thrusters.size(); ++a)
		{
			if(this->thrusters[a] != NULL)
			{
				delete this->thrusters[a];
			}
		}
	}

	void Moveable::GetSerializedForm(char* data) const
	{
		memcpy(data, &this->location, sizeof(this->location));
		memcpy(data + sizeof(this->location), &this->qOrientation, sizeof(this->qOrientation));
		memcpy(data + sizeof(this->location) + sizeof(this->qOrientation), &this->velocity, sizeof(this->velocity));
//		memcpy(data + sizeof(this->location) + sizeof(this->qOrientation) + sizeof(this->velocity), &this->qAngularVelocity, sizeof(this->qAngularVelocity));
		memcpy(data + sizeof(this->location) + sizeof(this->qOrientation) + sizeof(this->velocity), &this->pitchYawRollVelocity, sizeof(this->pitchYawRollVelocity));
	}

	const int Moveable::GetSerializedLength() const
	{
		return sizeof(this->location) +
			sizeof(this->qOrientation) + 
			sizeof(this->velocity) + 
			//sizeof(this->qAngularVelocity);
			sizeof(this->pitchYawRollVelocity);
	}
}
