#include "pch.h"
#include "Brain.h"
#include "../Actor.h"
#ifdef DO_STUFF
//#include "../../../lp_solve_5.5/lp_lib.h"
#endif
using namespace DirectX;

namespace SimulationEngine
{
	Brain::Brain(Actor* owner) :
		owner(owner),
		motorCortex(MotorCortex(owner))
	{

	}

	void MotorCortex::AddCommand(int when, const TimedThrusterFire& thrusterFire)
	{
		BrainCommand newCommand = {this->currentFrame + when, thrusterFire};
		this->commandQueue.push(newCommand);
	}

	void Brain::Update()
	{
		this->motorCortex.Update();
	}

	MotorCortex::MotorCortex(Actor* owner) :
		owner(owner)
	{
		axisWeights[0] = 1.0;
		axisWeights[1] = 1.0;
		axisWeights[2] = 1.0;
		axisWeights[3] = 1.0;
		axisWeights[4] = 1.0;
		axisWeights[5] = 1.0;
	}

    void MotorCortex::Calibrate()
	{
		for (Thruster* const thruster : this->owner->GetThrusters())
		{
			const XMVECTOR forceVector = XMVectorMultiply(thruster->direction, XMVectorReplicate(thruster->maximumMagnitude));
			const XMVECTOR torque = XMVector3Cross(thruster->offset, forceVector);

			this->thrusterTorques.push_back(torque);
			this->thrusterForces.push_back(XMVectorMultiply(thruster->direction, XMVectorReplicate(thruster->maximumMagnitude)));
		}
	}

	static const float turnMag = 1.f;
	static const float thrustMag = .15f;
	static const float fTM = .1f;

	void MotorCortex::approachLevel()
	{
		this->levelZ();
		this->levelX();
	}

	void MotorCortex::levelX()
	{
		/*XMFLOAT4 pitch, pitchVel, pitchVel2;
		//XMStoreFloat4(&pitch, owner->GetPitchYawRoll());
		XMStoreFloat4(&pitchVel, owner->GetPitchYawRollVelocity());
		XMStoreFloat4(&pitchVel2, owner->previousAngularVelocity);

		if (pitch.x > 0 && pitch.x < 3.1415f)
		{
			if (pitchVel.x >= 0 || pitchVel.x - pitchVel2.x < 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[8], turnMag });
			}
		}
		else if (pitch.x < 2 * 3.1415f && pitch.x > 3.1415f)
		{
			if (pitchVel.x <= 0 || pitchVel.x - pitchVel2.x > 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[9], turnMag });
			}
		}*/
	}

	void MotorCortex::levelZ()
	{
		/*XMFLOAT4 pitch, pitchVel, pitchVel2;
		//XMStoreFloat4(&pitch, owner->GetPitchYawRoll());
		XMStoreFloat4(&pitchVel, owner->GetPitchYawRollVelocity());
		XMStoreFloat4(&pitchVel2, owner->previousAngularVelocity);

		if (pitch.z > 0 && pitch.z < 3.1415f)
		{
			if (pitchVel.z >= 0 || pitchVel.z - pitchVel2.z < 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[12], turnMag });
			}
		}
		else if (pitch.z < 2 * 3.1415f && pitch.z > 3.1415f)
		{
			if (pitchVel.z <= 0 || pitchVel.z - pitchVel2.z > 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[13], turnMag });
			}
		}*/
	}

	void MotorCortex::approachTargetAngularVelocity()
	{
		/*XMFLOAT4 targetAngularVelocity, pitchVel, pitchVelThreshold;
		pitchVelThreshold = { 0, 0, 0, 0 };

		XMStoreFloat4(&targetAngularVelocity, this->targetPitchYawRollVelocity);
		XMStoreFloat4(&pitchVel, owner->GetPitchYawRollVelocity());

		if (pitchVel.y - targetAngularVelocity.y > pitchVelThreshold.y)
		{
			this->AddCommand(0, { 1, this->owner->GetThrusters()[4], turnMag * fTM });
			this->AddCommand(0, { 1, this->owner->GetThrusters()[3], turnMag * fTM });
		}
		else if (pitchVel.y - targetAngularVelocity.y < -pitchVelThreshold.y)
		{
			this->AddCommand(0, { 1, this->owner->GetThrusters()[5], turnMag * fTM });
			this->AddCommand(0, { 1, this->owner->GetThrusters()[2], turnMag * fTM });
		}*/
	}

	void MotorCortex::approachTargetVelocity()
	{
		XMFLOAT4 localVelocityF, targetVelocityF, targetVelocityThresholdF;
		XMStoreFloat4(&localVelocityF, owner->GetLocalVelocity());
		XMStoreFloat4(&targetVelocityF, this->targetVelocity);


		if (this->monitorZVelocity)
		{
			const float diffVelZ = localVelocityF.z - targetVelocityF.z;
			if (diffVelZ < 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[1], .1f*fabs(diffVelZ) * this->owner->mass / this->owner->GetThrusters()[1]->maximumMagnitude });
			}
			else if (diffVelZ > 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[14], .1f*fabs(diffVelZ) * this->owner->mass / this->owner->GetThrusters()[14]->maximumMagnitude });
			}
		}
		if (this->monitorXVelocity)
		{
			const float diffVelX = localVelocityF.x - targetVelocityF.x;
			if (diffVelX < 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[15], .1f*fabs(diffVelX) * owner->mass / owner->GetThrusters()[15]->maximumMagnitude });
			}
			else if (diffVelX > 0)
			{
				this->AddCommand(0, { 1, this->owner->GetThrusters()[16], .1f*fabs(diffVelX) * owner->mass / owner->GetThrusters()[16]->maximumMagnitude });
			}
		}
	}

	XMVECTOR MotorCortex::calculateRequiredForce() const
	{
		const XMVECTOR localVelocity = this->owner->GetLocalVelocity();
		const float mass = this->owner->mass;
		return XMVectorMultiply(XMVectorReplicate(mass), XMVectorSubtract(this->targetVelocity, localVelocity));
	}

	XMVECTOR MotorCortex::calculateRequiredTorque() const
	{
		/*return XMVectorMultiply(XMVectorReplicate(this->owner->GetRotationalInertia()),
			XMVectorSubtract(this->targetPitchYawRollVelocity, this->owner->GetPitchYawRollVelocity()));*/
		return XMVectorZero();
	}

	void MotorCortex::Update()
	{
		if (this->active)
		{
#ifdef DO_STUFF
			/*this->approachTargetVelocity();
			this->approachTargetAngularVelocity();
			this->approachLevel();*/
			const int thrusterCount = this->owner->GetThrusters().size();
			lprec* lp;
			int Ncol;
			Ncol = thrusterCount + 6;
			lp = make_lp(0, Ncol);
			set_minim(lp);

			set_add_rowmode(lp, TRUE);  // makes building the model faster if it is done rows by row

			//add constraint for thruster magnitudes >= 0 and <= 1
			for (int i = 0; i < thrusterCount; ++i)
			{
				double one = 1.0;
				if (!add_constraintex(lp, 1, &one, &i, LE, 1.0))
					exit(235234234);

				if (!add_constraintex(lp, 1, &one, &i, GE, 0.0))
					exit(76543456);
			}

			XMFLOAT4 requiredForce, requiredTorque;
			XMStoreFloat4(&requiredForce, this->calculateRequiredForce());
			XMStoreFloat4(&requiredTorque, this->calculateRequiredTorque());


			//the variables are the thruster percentages; coefficients are thruster magnitudes

			//have 6 * 2 constraints; +- thruster translational; +- thruster torque
			//form: Fx1 + Fx2 + ... + Fxn - X' <= Ft, where X' is abs(target force - attained force)

			//linear
			double* const thrusterMagnitudeX = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeX2 = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeY = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeY2 = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeZ = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeZ2 = new double[thrusterCount + 7]();

			//torque
			double* const thrusterMagnitudeTX = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeTX2 = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeTY = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeTY2 = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeTZ = new double[thrusterCount + 7]();
			double* const thrusterMagnitudeTZ2 = new double[thrusterCount + 7]();

			//set coefficients for thruster variables to their magnitudes in each direction
			for (int i = 0; i < thrusterCount; ++i)
			{
				XMFLOAT4 force, torque;
				XMStoreFloat4(&force, this->thrusterForces[i]);
				XMStoreFloat4(&torque, this->thrusterTorques[i]);
				thrusterMagnitudeX[i + 1] = force.x;
				thrusterMagnitudeX2[i + 1] = -force.x;
				thrusterMagnitudeY[i + 1] = force.y;
				thrusterMagnitudeY2[i + 1] = -force.y;
				thrusterMagnitudeZ[i + 1] = force.z;
				thrusterMagnitudeZ2[i + 1] = -force.z;

				thrusterMagnitudeTX[i + 1] =  torque.x;
				thrusterMagnitudeTX2[i + 1] = -torque.x;
				thrusterMagnitudeTY[i + 1] = torque.y;
				thrusterMagnitudeTY2[i + 1] = -torque.y;
				thrusterMagnitudeTZ[i + 1] = torque.z;
				thrusterMagnitudeTZ2[i + 1] = -torque.z;
			}

			//constant for the X', Y', etc values with coefficient -1
			thrusterMagnitudeX[thrusterCount + 1] = -1;
			thrusterMagnitudeX2[thrusterCount + 1] = -1;
			thrusterMagnitudeY[thrusterCount + 2] = -1;
			thrusterMagnitudeY2[thrusterCount + 2] = -1;
			thrusterMagnitudeZ[thrusterCount + 3] = -1;
			thrusterMagnitudeZ2[thrusterCount + 3] = -1;

			thrusterMagnitudeTX[thrusterCount + 4] = -1;
			thrusterMagnitudeTX2[thrusterCount + 4] = -1;
			thrusterMagnitudeTY[thrusterCount + 5] = -1;
			thrusterMagnitudeTY2[thrusterCount + 5] = -1;
			thrusterMagnitudeTZ[thrusterCount + 6] = -1;
			thrusterMagnitudeTZ2[thrusterCount + 6] = -1;
			
			const int tCount = 6;
			add_constraintex(lp, thrusterCount + tCount, thrusterMagnitudeX, nullptr, LE, requiredForce.x);
			add_constraintex(lp, thrusterCount + tCount, thrusterMagnitudeX2, nullptr, LE, -requiredForce.x);
			add_constraintex(lp, thrusterCount + tCount, thrusterMagnitudeY, nullptr, LE, requiredForce.y);
			add_constraintex(lp, thrusterCount + tCount, thrusterMagnitudeY2, nullptr, LE, -requiredForce.y);
			add_constraintex(lp, thrusterCount + tCount, thrusterMagnitudeZ, nullptr, LE, requiredForce.z);
			add_constraintex(lp, thrusterCount + tCount, thrusterMagnitudeZ2, nullptr, LE, -requiredForce.z);

			add_constraintex(lp, thrusterCount + 6, thrusterMagnitudeTX, nullptr, LE, requiredTorque.x);
			add_constraintex(lp, thrusterCount + 6, thrusterMagnitudeTX2, nullptr, LE, -requiredTorque.x);
			add_constraintex(lp, thrusterCount + 6, thrusterMagnitudeTY, nullptr, LE, requiredTorque.y);
			add_constraintex(lp, thrusterCount + 6, thrusterMagnitudeTY2, nullptr, LE, -requiredTorque.y);
			add_constraintex(lp, thrusterCount + 6, thrusterMagnitudeTZ, nullptr, LE, requiredTorque.z);
			add_constraintex(lp, thrusterCount + 6, thrusterMagnitudeTZ2, nullptr, LE, -requiredTorque.z);

			delete[] thrusterMagnitudeX;
			delete[] thrusterMagnitudeX2;
			delete[] thrusterMagnitudeY;
			delete[] thrusterMagnitudeY2;
			delete[] thrusterMagnitudeZ;
			delete[] thrusterMagnitudeZ2;

			delete[] thrusterMagnitudeTX;
			delete[] thrusterMagnitudeTX2;
			delete[] thrusterMagnitudeTY;
			delete[] thrusterMagnitudeTY2;
			delete[] thrusterMagnitudeTZ;
			delete[] thrusterMagnitudeTZ2;

			set_add_rowmode(lp, FALSE);

			//coefficients of vars in obj (x', y', z', tx', ty', tz')
			int objectiveFnColno[] = { thrusterCount + 1, thrusterCount + 2, thrusterCount + 3, thrusterCount + 4, thrusterCount + 5, thrusterCount + 6 };
			set_obj_fnex(lp, 6, axisWeights, objectiveFnColno);

			set_minim(lp);

			const auto ret = solve(lp);
			double* const vars = new double[thrusterCount + 7];

			get_variables(lp, vars);

			for (int a = 0; a < thrusterCount; ++a)
			{
				if (vars[a] > 0)
				{
					this->AddCommand(0, { 1, this->owner->GetThrusters()[a], static_cast<float>(vars[a]) });
				}
			}

			delete[] vars;
			
			if (lp != nullptr) 
			{
				delete_lp(lp);
			}
#endif
		}

		while(this->commandQueue.size() > 0)
		{
			const BrainCommand& nextCommand = this->commandQueue.top();

			if(this->currentFrame >= nextCommand.when)
			{
				if(nextCommand.thrusterFire.thruster->activated)
				{
					BrainCommand newCommand = {this->currentFrame + 1, nextCommand.thrusterFire};
				}
				else
				{
					this->owner->FireThrusterPercent(nextCommand.thrusterFire.thruster, nextCommand.thrusterFire.frameCount, nextCommand.thrusterFire.magnitude);
				}

				this->commandQueue.pop();
			}
			else
			{
				break;
			}
		}

		++this->currentFrame;

		if(this->commandQueue.size() == 0)
		{
			this->currentFrame = 0;
		}
	}
}