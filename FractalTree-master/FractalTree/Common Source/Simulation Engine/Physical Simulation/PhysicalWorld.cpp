#include "pch.h"
#include "PhysicalWorld.h"
#include "..\..\Skeleton\BasicTypes.h"
#ifndef _NO_GRAPHICS
#include "UIRenderer.h"
#endif
#include "IConstraintD.h"

using namespace DirectX;
using namespace std;
using namespace MathUtil;

static int cInd = -1;

vector<int> debugInds;
static int totalErrorInd;
static int oErr, oErr2;
static int sysEnInd;
namespace SimulationEngine
{
	PhysicalWorld::PhysicalWorld() :
		nextAvailableLocalEffectID(0),
		nextAvailableGlobalEffectID(0),
		damping(.95f)
	{
		this->globalEffectIDMap.resize(10000, -1);
		this->dummyObject = new Actor(ObjectParameters(NULL, 0, -1));
#ifndef _NO_GRAPHICS
		if (cInd == -1)
		{
			cInd = RenderingEngine::UIRenderer::AddDebugString("Constraint avg iter: ");
			totalErrorInd = RenderingEngine::UIRenderer::AddDebugString("Total Constraint error: ");;
			oErr2 = RenderingEngine::UIRenderer::AddDebugString("Final Constraint before update error: ");;
			oErr = RenderingEngine::UIRenderer::AddDebugString("Final Constraint after update error: ");;
			sysEnInd = RenderingEngine::UIRenderer::AddDebugString("System energy: ");;
			for (int i = 0; i < ResourceManager::constraintIterationCount && i < 10; ++i)
			{
				//debugInds.push_back(RenderingEngine::UIRenderer::AddDebugString("Constraint error: "));
			}
		}
#endif
	}

	void PhysicalWorld::Update()
	{
		if(this->active)
		{
			this->ApplyEffects();
			this->ApplyPhysics();

			this->ClearEffects();
		}
		++this->frameCount;
	}

	inline static DirectX::XMVECTOR toEulerianAngle(const DirectX::XMVECTOR& qq)
	{
		DirectX::XMFLOAT4 q;
		DirectX::XMStoreFloat4(&q, qq);
		double ysqr = q.y * q.y;
		double t0 = -2.0f * (ysqr + q.z * q.z) + 1.0f;
		double t1 = +2.0f * (q.x * q.y - q.w * q.z);
		double t2 = -2.0f * (q.x * q.z + q.w * q.y);
		double t3 = +2.0f * (q.y * q.z - q.w * q.x);
		double t4 = -2.0f * (q.x * q.x + ysqr) + 1.0f;

		t2 = t2 > 1.0f ? 1.0f : t2;
		t2 = t2 < -1.0f ? -1.0f : t2;

		const double pitch = std::asin(t2);
		const double roll = std::atan2(t3, t4);
		const double yaw = std::atan2(t1, t0);

		return DirectX::XMVectorSet(roll, pitch, yaw, 0);
	}

	static XMVECTOR addAA(const XMVECTOR& a1, const XMVECTOR& a2)
	{
		XMVECTOR result; float ang;
		XMQuaternionToAxisAngle(&result, &ang, XMQuaternionMultiply(a1, a2));
		return XMVectorMultiply(XMVector3Normalize(result), XMVectorReplicate(ang));
	}

	static const float impC = 200;
	static const XMVECTOR minImpulse = XMVectorSet(-impC, -impC, -impC, -impC);
	static const XMVECTOR maxImpulse = XMVectorSet(impC, impC, impC, impC);
	static const Eigen::Vector3d minImpulseE = Eigen::Vector3d(-impC, -impC, -impC);
	static const Eigen::Vector3d maxImpulseE = Eigen::Vector3d(impC, impC, impC);

	static DirectX::XMVECTOR clampImpulse(IConstraint* const constraint, const XMVECTOR& dCdt)
	{
		/*XMFLOAT4 testFloat;
		XMStoreFloat4(&testFloat, dCdt);
		if (isnan(testFloat.x) || isnan(testFloat.y) || isnan(testFloat.z) || isnan(testFloat.w) 
			|| isinf(testFloat.x) || isinf(testFloat.y) || isinf(testFloat.z)) {
			constraint->accumulatedImpulse = XMVectorZero();
			return XMVectorZero();
		}*/

		const XMVECTOR oldImpulse = constraint->accumulatedImpulse;
		XMVECTOR impulse = XMVector3Transform(XMVectorAdd(-dCdt, constraint->velocityBias), constraint->invMass);
		//XMVECTOR impulse = solveInv(constraint->invMass, XMVectorAdd(-dCdt, constraint->velocityBias));
		constraint->accumulatedImpulse = XMVectorAdd(constraint->accumulatedImpulse, impulse);
		constraint->accumulatedImpulse = XMVectorClamp(constraint->accumulatedImpulse, minImpulse, maxImpulse);
		impulse = XMVectorSubtract(constraint->accumulatedImpulse, oldImpulse);

		return impulse;
	}

	static Eigen::Vector3d clampImpulse2(IConstraintD* const constraint, const Eigen::Vector3d& dCdt, const double impulseFactor)
	{
		using namespace Eigen;
		const Vector3d oldImpulse = constraint->accumulatedImpulse;
		Vector3d impulse = constraint->invMass * (-dCdt + constraint->velocityBias);
		constraint->accumulatedImpulse = constraint->accumulatedImpulse + impulse;
		constraint->accumulatedImpulse = clampV(constraint->accumulatedImpulse, minImpulseE, maxImpulseE);
		impulse = constraint->accumulatedImpulse - oldImpulse;

		return impulse;
	}

	void PhysicalWorld::ApplyPhysics()
	{

		for (int actorIndex = 0; actorIndex < this->nextAvailableLocalID; ++actorIndex)
		{
			//Check if this object is registered as an actor.
			if ((this->objectList[actorIndex]->GetComponentFlags() & PhysicalWorld::ACT) > 0)
			{
				Actor* const actor = static_cast<Actor*>(this->objectList[actorIndex]);
				if (!actor->locked)
				{
					actor->ApplyThrusters();
				}
			}
		}

		const float timeStep = 1.0f / ResourceManager::timeStepCount;

		for (int actorIndex = 0; actorIndex < this->nextAvailableLocalID; ++actorIndex)
		{
			//Check if this object is registered as an actor.
			if ((this->objectList[actorIndex]->GetComponentFlags() & PhysicalWorld::ACT) > 0)
			{
				Actor* const actor = static_cast<Actor*>(this->objectList[actorIndex]);
				if (!actor->locked)
				{
					actor->UpdateForces(1);
				}
			}
		}

		const float impulseFactor = 1;
		const float positionFactor = timeStep;
		int lastIterationCount = 0;

		/*XMVECTOR err222 = XMVectorZero();
		for (int i = 0; i < this->constraints.size(); ++i)
		{
			const Constraint& c = this->constraints[i];
			err222 = XMVectorAdd(err222, XMVectorAbs(c.computeError()));
		}

#ifndef _NO_GRAPHICS
		RenderingEngine::UIRenderer::SetDebugString(oErr2, "Constraint error before: " + getVectorString(err222));
#endif*/

		for (int stepNumber = 0; stepNumber < ResourceManager::timeStepCount; ++stepNumber)
		{
			for (int constraintIndex = 0; constraintIndex < this->constraints.size(); ++constraintIndex)
			{
				Constraint& constraint = this->constraints[constraintIndex];
				constraint.UpdateFrame();
			}

			for (int constraintIndex = 0; constraintIndex < this->constraintsD.size(); ++constraintIndex)
			{
				ConstraintD& constraint = this->constraintsD[constraintIndex];
				constraint.UpdateFrame();
			}

			for (int constraintIndex = 0; constraintIndex < this->angleConstraints.size(); ++constraintIndex)
			{
				AngleConstraint& constraint = this->angleConstraints[constraintIndex];
				constraint.UpdateFrame();
			}

			for (int constraintIndex = 0; constraintIndex < this->angleConstraintsD.size(); ++constraintIndex)
			{
				AngleConstraintD& constraint = this->angleConstraintsD[constraintIndex];
				constraint.UpdateFrame();
			}

			static const int avgItFC = 100;
			if (this->iterationCounts.size() > avgItFC)
			{
				this->totalConstraintIterations -= this->iterationCounts.front();
				this->iterationCounts.pop_front();
			}

			int iterationCount = 0;
			for (; iterationCount < ResourceManager::constraintIterationCount; ++iterationCount)
			{
				XMVECTOR totalError = XMVectorZero();
				for (int constraintIndex = 0; constraintIndex < this->constraints.size(); ++constraintIndex)
				{
					//point constraints

					Constraint& constraint = this->constraints[constraintIndex];

					const XMVECTOR velocityTerm1 = constraint.object1->velocity;
					const XMVECTOR angularVelocity1 = constraint.object1->pitchYawRollVelocity;

					if (constraint.object2 == nullptr || constraint.immovableObject2)
					{
						//object 1 is connected directly to the world or an immovable object

						const XMVECTOR point1Velocity = XMVectorAdd(-velocityTerm1, -XMVector3Cross(angularVelocity1, constraint.leverArm1));

						const XMVECTOR dCdt = point1Velocity;
						const XMVECTOR impulse = clampImpulse(&constraint, dCdt);

						const XMVECTOR newVelocity1 = XMVectorSubtract(velocityTerm1, XMVectorMultiply(impulse, XMVectorReplicate(constraint.object1->inverseMass)));
						const XMVECTOR newAngularVelocity1 = XMVectorSubtract(angularVelocity1, XMVector3Cross(constraint.leverArm1, impulse));

						constraint.object1->SetVelocity(newVelocity1);
						constraint.object1->SetPitchYawRollVelocity(newAngularVelocity1);

#ifndef _NO_GRAPHICS
						if (iterationCount < 0)
							RenderingEngine::UIRenderer::SetDebugString(debugInds[iterationCount], "Constraint error " + to_string(iterationCount) + ": der err: " + getVectorString(dCdt) + " bias " + getVectorString(constraint.velocityBias));
#endif
						totalError = XMVectorAdd(totalError, XMVectorAbs(dCdt));
					}
					else
					{
						//object 1 and 2 are both moveable objects

						const XMVECTOR velocityTerm2 = constraint.object2->velocity;
						const XMVECTOR angularVelocity2 = constraint.object2->pitchYawRollVelocity;

						//the velocities of the constraint point on each object
						const XMVECTOR point1Velocity = XMVectorAdd(-velocityTerm1, -XMVector3Cross(angularVelocity1, constraint.leverArm1));
						const XMVECTOR point2Velocity = XMVectorAdd(velocityTerm2, XMVector3Cross(angularVelocity2, constraint.leverArm2));

						//rate of change of the distance between the constraint points
						//any non-zero velocity is constraint derivative violation
						const XMVECTOR dCdt = XMVectorAdd(point1Velocity, point2Velocity);
						const XMVECTOR impulse = clampImpulse(&constraint, dCdt);

						const XMVECTOR newVelocity1 = XMVectorSubtract(velocityTerm1, XMVectorMultiply(impulse, XMVectorReplicate(constraint.object1->inverseMass)));
						const XMVECTOR newAngularVelocity1 = XMVectorSubtract(angularVelocity1, XMVector3Cross(constraint.leverArm1, impulse));

						const XMVECTOR newVelocity2 = XMVectorAdd(velocityTerm2, XMVectorMultiply(impulse, XMVectorReplicate(constraint.object2->inverseMass)));
						const XMVECTOR newAngularVelocity2 = XMVectorAdd(angularVelocity2, XMVector3Cross(constraint.leverArm2, impulse));

						constraint.object1->SetVelocity(newVelocity1);
						constraint.object1->SetPitchYawRollVelocity(newAngularVelocity1);
						constraint.object2->SetVelocity(newVelocity2);
						constraint.object2->SetPitchYawRollVelocity(newAngularVelocity2);

						totalError = XMVectorAdd(totalError, XMVectorAbs(dCdt));
					}
				}

				for (int constraintIndex = 0; constraintIndex < this->constraintsD.size(); ++constraintIndex)
				{
					//point constraints
					using namespace Eigen;
					ConstraintD& constraint = this->constraintsD[constraintIndex];

					const Vector3d velocityTerm1 = fromDirectX(constraint.object1->velocity);
					const Vector3d angularVelocity1 = fromDirectX(constraint.object1->pitchYawRollVelocity);

					if (constraint.object2 == nullptr || constraint.immovableObject2)
					{
						//object 1 is connected directly to the world or an immovable object

						const Vector3d point1Velocity = -velocityTerm1 + -angularVelocity1.cross(constraint.leverArm1);

						const Vector3d dCdt = point1Velocity;
						const Vector3d impulse = clampImpulse2(&constraint, dCdt, impulseFactor);

						const Vector3d newVelocity1 = velocityTerm1 - impulse;
						const Vector3d newAngularVelocity1 = angularVelocity1 - constraint.leverArm1.cross(impulse);

						constraint.object1->SetVelocity(fromEigen(newVelocity1));
						constraint.object1->SetPitchYawRollVelocity(fromEigen(newAngularVelocity1));

#ifndef _NO_GRAPHICS
						if (iterationCount < 0)
							RenderingEngine::UIRenderer::SetDebugString(debugInds[iterationCount], "Constraint error " + to_string(iterationCount) + ": der err: " + getVectorString(fromEigen(dCdt)) + " bias " + getVectorString(fromEigen(constraint.velocityBias)));
#endif
						totalError = XMVectorAdd(totalError, XMVectorAbs(fromEigen(dCdt)));
					}
					else
					{
						//object 1 and 2 are both moveable objects

						const Vector3d velocityTerm2 = fromDirectX(constraint.object2->velocity);
						const Vector3d angularVelocity2 = fromDirectX(constraint.object2->pitchYawRollVelocity);

						//the velocities of the constraint point on each object
						const Vector3d point1Velocity = -velocityTerm1 + -angularVelocity1.cross(constraint.leverArm1);
						const Vector3d point2Velocity = velocityTerm2 + angularVelocity2.cross(constraint.leverArm2);

						//rate of change of the distance between the constraint points
						//any non-zero velocity is constraint derivative violation
						const Vector3d dCdt = point1Velocity + point2Velocity;
						const Vector3d impulse = clampImpulse2(&constraint, dCdt, impulseFactor);

						const Vector3d newVelocity1 = velocityTerm1 - impulse;
						const Vector3d newAngularVelocity1 = angularVelocity1 - constraint.leverArm1.cross(impulse);

						const Vector3d newVelocity2 = velocityTerm2 + impulse;
						const Vector3d newAngularVelocity2 = angularVelocity2 + constraint.leverArm2.cross(impulse);

						constraint.object1->SetVelocity(fromEigen(newVelocity1));
						constraint.object1->SetPitchYawRollVelocity(fromEigen(newAngularVelocity1));
						constraint.object2->SetVelocity(fromEigen(newVelocity2));
						constraint.object2->SetPitchYawRollVelocity(fromEigen(newAngularVelocity2));

						totalError = XMVectorAdd(totalError, XMVectorAbs(fromEigen(dCdt)));
					}
				}

				for (int constraintIndex = 0; constraintIndex < this->angleConstraintsD.size(); ++constraintIndex)
				{
					using namespace Eigen;
					AngleConstraintD& constraint = this->angleConstraintsD[constraintIndex];

					const Vector3d velocityTerm1 = fromDirectX(constraint.object1->velocity);
					const Vector3d angularVelocity1 = fromDirectX(constraint.object1->pitchYawRollVelocity);
					if (constraint.object2 == nullptr || constraint.immovableObject2)
					{
						const Vector3d dCdt = -angularVelocity1;
						const Vector3d impulse = clampImpulse2(&constraint, dCdt, impulseFactor);

						const Vector3d addRotVel1 = angularVelocity1 - impulse;

						const Vector3d addVel1 = velocityTerm1 - constraint.leverArm1.cross(impulse);
						constraint.object1->SetVelocity(fromEigen(addVel1));
						constraint.object1->SetPitchYawRollVelocity(fromEigen(addRotVel1));
						totalError = XMVectorAdd(totalError, XMVectorAbs(fromEigen(dCdt)));
					}
					else
					{
						const Vector3d angularVelocity2 = fromDirectX(constraint.object2->pitchYawRollVelocity);
						const Vector3d velocityTerm2 = fromDirectX(constraint.object2->velocity);

						const Vector3d dCdt = -angularVelocity1 + angularVelocity2;
						const Vector3d impulse = clampImpulse2(&constraint, dCdt, impulseFactor);

						const Vector3d addRotVel1 = angularVelocity1 - impulse;
						const Vector3d addRotVel2 = angularVelocity2 + impulse;

						const Vector3d addVel1 = velocityTerm1 - constraint.leverArm1.cross(impulse);
						const Vector3d addVel2 = velocityTerm2 - constraint.leverArm2.cross(impulse);

						constraint.object1->SetVelocity(fromEigen(addVel1));
						constraint.object1->SetPitchYawRollVelocity(fromEigen(addRotVel1));

						constraint.object2->SetVelocity(fromEigen(addVel2));
						constraint.object2->SetPitchYawRollVelocity(fromEigen(addRotVel2));
						totalError = XMVectorAdd(totalError, XMVectorAbs(fromEigen(dCdt)));
					}
				}

				for (int constraintIndex = 0; constraintIndex < this->angleConstraints.size(); ++constraintIndex)
				{
					AngleConstraint& constraint = this->angleConstraints[constraintIndex];

					const XMVECTOR angularVelocity1 = constraint.object1->pitchYawRollVelocity;
					if (constraint.object2 == nullptr || constraint.immovableObject2)
					{
						const XMVECTOR dCdt = -angularVelocity1;
						const XMVECTOR impulse = clampImpulse(&constraint, dCdt);

						const XMVECTOR addRotVel1 = impulse;

						const XMVECTOR addVel1 = XMVector3Cross(constraint.leverArm1, XMVectorScale(impulse, constraint.object1->inverseMass));
						constraint.object1->SetVelocity(XMVectorSubtract(constraint.object1->velocity, addVel1));
						constraint.object1->SetPitchYawRollVelocity(XMVectorSubtract(constraint.object1->pitchYawRollVelocity,
							addRotVel1));
						totalError = XMVectorAdd(totalError, XMVectorAbs(dCdt));
					}
					else
					{
						const XMVECTOR angularVelocity2 = constraint.object2->pitchYawRollVelocity;

						const XMVECTOR dCdt = XMVectorAdd(-angularVelocity1, angularVelocity2);
						const XMVECTOR impulse = clampImpulse(&constraint, dCdt);

						const XMVECTOR addRotVel1 = impulse;
						const XMVECTOR addRotVel2 = impulse;

						const XMVECTOR addVel1 = XMVector3Cross(constraint.leverArm1, XMVectorScale(impulse, constraint.object1->inverseMass));
						const XMVECTOR addVel2 = XMVector3Cross(constraint.leverArm2, XMVectorScale(impulse, constraint.object2->inverseMass));

						constraint.object1->SetVelocity(XMVectorSubtract(constraint.object1->velocity, addVel1));
						constraint.object1->SetPitchYawRollVelocity(XMVectorSubtract(constraint.object1->pitchYawRollVelocity,
							addRotVel1));

						constraint.object2->SetVelocity(XMVectorSubtract(constraint.object2->velocity, addVel2));
						constraint.object2->SetPitchYawRollVelocity(XMVectorAdd(constraint.object2->pitchYawRollVelocity,
							addRotVel2));
						totalError = XMVectorAdd(totalError, XMVectorAbs(dCdt));
					}
				}

				XMFLOAT4 totalErrorF;
				XMStoreFloat4(&totalErrorF, XMVector3Length(totalError));
				if (totalErrorF.x < ResourceManager::constraintMinError)
				{
					++iterationCount;
					break;
				}
			}

			/*for (int i = 0; i < this->constraints.size(); ++i)
			{
				const Constraint& c = this->constraints[i];
				c.object1->SetVelocity(XMVectorSubtract(c.object1->GetVelocity(), c.velocityVelocityBias));
				c.object1->SetPitchYawRollVelocity(XMVectorSubtract(c.object1->GetPitchYawRollVelocity(), c.velocityAngleBias));

				if (c.object2 != nullptr && !c.immovableObject2)
				{
					c.object2->SetVelocity(XMVectorAdd(c.object2->GetVelocity(), c.velocityVelocityBias2));
					c.object2->SetPitchYawRollVelocity(XMVectorAdd(c.object2->GetPitchYawRollVelocity(), c.velocityAngleBias2));
				}
			}*/

			this->totalConstraintIterations += iterationCount;
			this->iterationCounts.push_back(iterationCount);
			lastIterationCount = iterationCount;

			for (int actorIndex = 0; actorIndex < this->nextAvailableLocalID; ++actorIndex)
			{
				//Check if this object is registered as an actor.
				if ((this->objectList[actorIndex]->GetComponentFlags() & PhysicalWorld::ACT) > 0)
				{
					Actor* const actor = static_cast<Actor*>(this->objectList[actorIndex]);
					if (!actor->locked)
					{
						actor->UpdatePosition(positionFactor);
					}
				}
			}

			XMVECTOR err = XMVectorZero();
			for (int i = 0; i < this->constraints.size(); ++i)
			{
				const Constraint& c = this->constraints[i];
				err = XMVectorAdd(err, XMVectorAbs(c.computeError()));
			}

#ifndef _NO_GRAPHICS
			//RenderingEngine::UIRenderer::SetDebugString(oErr, "Constraint error after: " + getVectorString(err));
#endif
		}
#ifndef _NO_GRAPHICS
		RenderingEngine::UIRenderer::SetDebugString(cInd, "Constraint avg iter: " + to_string(this->totalConstraintIterations / this->iterationCounts.size()) + " (Last: " + to_string(lastIterationCount) + ")");
#endif

		XMVECTOR totalEnergy = XMVectorZero();
		for (int actorIndex = 0; actorIndex < this->nextAvailableLocalID; ++actorIndex)
		{
			Actor* const actor = static_cast<Actor*>(this->objectList[actorIndex]);
			QueryPerformanceCounter(&actor->lastUpdated);
			//Check if this object is registered as an actor.
			if ((this->objectList[actorIndex]->GetComponentFlags() & PhysicalWorld::ACT) > 0)
			{
				actor->Act();
			}
		}
	}

	void PhysicalWorld::QueryObjectsLocation(vector<Actor*>& results, const DirectX::XMVECTOR& location, float radiusSquared) const
	{
		for(int a = 0; a < this->GetObjectCount(); ++a)
		{
			const XMVECTOR distanceSquared = XMVector3LengthSq(XMVectorSubtract(this->GetObject(a)->location, location));
			XMFLOAT4 distF;
			XMStoreFloat4(&distF, distanceSquared);

			if(distF.x <= radiusSquared)
			{
				results.push_back(this->GetObject(a));
			}
		}
	}

	void PhysicalWorld::QueryObjectsLocation(vector<Actor*>& results, const DirectX::XMVECTOR& location, float radiusSquared, ObjectTypes objectType) const
	{
		for (int a = 0; a < this->GetObjectCount(); ++a)
		{
			if (this->GetObject(a)->GetObjectType() != objectType)
				continue;

			const XMVECTOR distanceSquared = XMVector3LengthSq(XMVectorSubtract(this->GetObject(a)->location, location));
			XMFLOAT4 distF;
			XMStoreFloat4(&distF, distanceSquared);

			if (distF.x <= radiusSquared)
			{
				results.push_back(this->GetObject(a));
			}
		}
	}

	void PhysicalWorld::QueryObjectsLocation(vector<Actor*>& results, const DirectX::XMVECTOR& location, float radiusSquared, ObjectTypes* exclusionTypes, int exclusionCount, int excludedPlayer) const
	{
		for (int a = 0; a < this->GetObjectCount(); ++a)
		{
			if (this->GetObject(a)->GetOwningPlayer() == excludedPlayer)
			{
				continue;
			}

			bool exclude = false;
			for (int b = 0; b < exclusionCount; ++b)
			{
				if (this->GetObject(a)->GetObjectType() == exclusionTypes[b])
				{
					exclude = true;
					break;
				}
			}
			
			if (exclude)
			{
				continue;
			}

			const XMVECTOR distanceSquared = XMVector3LengthSq(XMVectorSubtract(this->GetObject(a)->location, location));
			XMFLOAT4 distF;
			XMStoreFloat4(&distF, distanceSquared);

			if (distF.x <= radiusSquared)
			{
				results.push_back(this->GetObject(a));
			}
		}
	}

	void PhysicalWorld::ApplyEffects()
	{
		for(int a = 0; a < this->GetEffectCount(); ++a)
		{
			this->effectList[a]->Apply();
			if(this->effectList[a]->IsInstant())
			{
				this->RemoveEffect(this->effectList[a]);
			}
		}
	}

	void PhysicalWorld::AddEffect(Effect* effect)
	{
		assert(this->globalEffectIDMap[this->nextAvailableGlobalEffectID] < 0);
		if(this->nextAvailableLocalEffectID < this->effectList.size())
		{
			this->effectList[this->nextAvailableLocalEffectID] = effect;
		}
		else
		{
			this->effectList.push_back(effect);
		}

		effect->SetLocalID(this->nextAvailableLocalEffectID);
		effect->SetGlobalID(this->nextAvailableGlobalEffectID);
		this->SetEffectGlobalIDMapping(this->nextAvailableGlobalEffectID, this->nextAvailableLocalEffectID);
		++this->nextAvailableLocalEffectID;
		++this->nextAvailableGlobalEffectID;
	}

	void PhysicalWorld::RemoveEffect(Effect* effect)
	{
		this->removeEffectList.push_back(effect);
	}

	void PhysicalWorld::ClearEffects()
	{
		for(unsigned int a = 0; a < this->removeEffectList.size(); ++a)
		{
			if(this->nextAvailableLocalEffectID > 1)
			{
				if(this->nextAvailableLocalEffectID - 1 != this->removeEffectList[a]->GetLocalID())
				{
					const int localID = this->removeEffectList[a]->GetLocalID();
					this->effectList[localID] = this->effectList[this->nextAvailableLocalEffectID - 1];
					this->effectList[localID]->SetLocalID(localID);
					this->SetEffectGlobalIDMapping(this->effectList[localID]->GetGlobalID(), localID);
				}

				--this->nextAvailableLocalEffectID;
				this->effectList[this->nextAvailableLocalEffectID] = NULL;
			}
			else
			{
				this->effectList[this->removeEffectList[a]->GetLocalID()] = NULL;
				--this->nextAvailableLocalEffectID;
			}

			delete this->removeEffectList[a];
		}

		this->removeEffectList.clear();
	}

	bool PhysicalWorld::IsObjectRegistered(int globalObjectID, unsigned int componentFlags) const
	{
		return (this->GetObjectByGlobalID(globalObjectID)->GetComponentFlags() & componentFlags) > 0;
	}

	float PhysicalWorld::AngleBetweenObjects(int objectIDA, int objectIDB) const
	{
		const XMVECTOR locA = this->GetObjectGlobal(objectIDA)->location;
		const XMVECTOR locB = this->GetObjectGlobal(objectIDB)->location;
		const XMVECTOR vec = XMVector3Normalize(XMVectorMultiply(XMVectorSubtract(locB, locA), XMVectorSet(1, 0, 1, 0)));
		const Actor* objA = this->GetActor(objectIDA);
		const XMVECTOR localZ = XMVector3Transform(XMVectorSet(0, 0, 1, 0), XMMatrixRotationQuaternion(objA->qOrientation));
		XMFLOAT4 ang;
		XMStoreFloat4(&ang, XMVector3AngleBetweenNormals(localZ, vec));
		//position = sign((Bx - Ax)*(Y - Ay) - (By - Ay)*(X - Ax))	
		XMFLOAT4 lZ, vecF;
		XMStoreFloat4(&lZ, localZ);
		XMStoreFloat4(&vecF, vec);

		float sign = (lZ.x) * (vecF.z) - (lZ.z) * (vecF.x);
		return sign >= 0 ? ang.x : -ang.x;
	}

	/*const float PhysicalWorld::rotAngX = 0;//3.1415f*.25f;
	const float PhysicalWorld::rotAngY = 0;//3.1415f*.125f;
	const float PhysicalWorld::rotAngZ = 0;//3.1415f*.3125f;*/
	const float PhysicalWorld::rotAngX = 3.1415f*.25f;
	const float PhysicalWorld::rotAngY = 3.1415f*.125f;
	const float PhysicalWorld::rotAngZ = 3.1415f*.3125f;
	const float PhysicalWorld::moveY = 100;
	bool PhysicalWorld::intersect(const float launchX, const float launchY, const float launchZ,
		const float launchDirX, const float launchDirY, const float launchDirZ)
	{
		for (int a = 0; a < this->GetObjectCount(); ++a)
		{
			Actor* const obj = static_cast<Actor*>(this->objectList[a]);
			if ((obj->GetComponentFlags() & PhysicalWorld::PHYSICS) > 0)
			{
				for (int h = 0; h < obj->GetCollisionHulls().size(); ++h)
				{
					const CollisionHull* const hull = obj->GetCollisionHulls()[h];

					XMVECTOR d;
					XMFLOAT4 startVec;
					XMFLOAT4 dirVec;
					XMFLOAT4 minBoundsF;
					XMFLOAT4 maxBoundsF;
					const XMVECTOR launchLoc = XMVectorSet(launchX, launchY, launchZ, 1);
					if (!hull->useTransformation)
					{
						const XMMATRIX translationMatrix = XMMatrixTranslationFromVector(obj->location);
						const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(obj->qOrientation);
						const XMMATRIX objectTransformation = XMMatrixMultiply(rotationMatrix, translationMatrix);
						const XMMATRIX finalTransform = XMMatrixInverse(&d, objectTransformation);
						const XMVECTOR rotatedStart = XMVector3Transform(launchLoc, finalTransform);
						const XMVECTOR rotatedDir = XMVector3Normalize(
							XMVector4Transform(XMVectorSet(launchDirX, launchDirY, launchDirZ, 0), finalTransform));

						XMStoreFloat4(&startVec, rotatedStart);
						XMStoreFloat4(&dirVec, rotatedDir);
					}
					else
					{
						const XMMATRIX translationMatrix = XMMatrixTranslationFromVector(obj->location);
//						const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(obj->GetPitchYawRoll());
						const XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(obj->qOrientation);
						const XMMATRIX objectTransformation = XMMatrixMultiply(rotationMatrix, translationMatrix);
						const XMMATRIX finalTransform = XMMatrixInverse(&d, XMMatrixMultiply(hull->transformationMatrix, objectTransformation));
						const XMVECTOR rotatedStart = XMVector3Transform(launchLoc, finalTransform);
						const XMVECTOR rotatedDir = XMVector3Normalize(
							XMVector4Transform(XMVectorSet(launchDirX, launchDirY, launchDirZ, 0), finalTransform));
						XMStoreFloat4(&startVec, rotatedStart);
						XMStoreFloat4(&dirVec, rotatedDir);
					}

					XMStoreFloat4(&minBoundsF, hull->minBounds);
					XMStoreFloat4(&maxBoundsF, hull->maxBounds);
					float tmin = (minBoundsF.x - startVec.x) / dirVec.x;
					float tmax = (maxBoundsF.x - startVec.x) / dirVec.x;

					if (tmin > tmax) swap(tmin, tmax);

					float tymin = (minBoundsF.y - startVec.y) / dirVec.y;
					float tymax = (maxBoundsF.y - startVec.y) / dirVec.y;

					if (tymin > tymax) swap(tymin, tymax);

					if ((tmin > tymax) || (tymin > tmax))
						continue;

					if (tymin > tmin)
						tmin = tymin;

					if (tymax < tmax)
						tmax = tymax;

					float tzmin = (minBoundsF.z - startVec.z) / dirVec.z;
					float tzmax = (maxBoundsF.z - startVec.z) / dirVec.z;

					if (tzmin > tzmax) swap(tzmin, tzmax);

					if ((tmin > tzmax) || (tzmin > tmax))
						continue;

					if (tzmin > tmin)
						tmin = tzmin;

					if (tzmax < tmax)
						tmax = tzmax;

					const XMVECTOR launchDirVec = XMVectorSet(launchDirX, launchDirY, launchDirZ, 0);
					const XMVECTOR intersectionPoint = XMVectorSet(launchX + tmin * launchDirX, launchY + tmin * launchDirY, launchZ + tmin * launchDirZ, 1);
					//const XMVECTOR intersectionPoint = XMVectorSet(startVec.x + tmin * dirVec.x, startVec.y + tmin * dirVec.y, startVec.z + tmin * dirVec.z, 1);
					//ForceEffect* explosionEffect = new ForceEffect(EffectParameters(this, true), ForceEffectParameters(intersectionPoint, XMVectorSet(0, 0, 0, 0), 50000, false, false, false, 400 * 400, .1f));
#ifndef _RTS_GAME
					DecentMissile* p = new DecentMissile(ObjectParameters(this),
						MoveableParameters(intersectionPoint,
							XMVectorSet(0, 0, 0, 0),
							XMVectorSet(0, 0, 0, 0),
							XMVectorSet(0, 0, 0, 0)), true, 1);
#endif
					if (!hull->useTransformation)
					{
						obj->ApplyForceAtPoint(intersectionPoint, XMVectorMultiply(launchDirVec, XMVectorReplicate(ResourceManager::gunForce)), .15f);
						obj->pauseCount = 100;
						obj->ePoint = intersectionPoint;
						obj->eDir = launchDirVec;
						//testArm->pauseCount = 50;
					}
					return true;
				}
			}
		}

		//return false;
		const XMMATRIX xRot = XMMatrixRotationX(rotAngX);
		const XMMATRIX yRot = XMMatrixRotationY(rotAngY);
		const XMMATRIX zRot = XMMatrixRotationY(rotAngZ);

		XMVECTOR d;
		const XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMVectorSet(0, 0, 0, 0));
		const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMVectorSet(rotAngX, rotAngY, rotAngZ, 0));
		const XMMATRIX finalTransform = XMMatrixInverse(&d, 
			XMMatrixMultiply(translationMatrix, rotationMatrix));
		const XMVECTOR rotatedStart = XMVector3Transform(XMVectorSet(launchX, launchY - moveY, launchZ, 1), finalTransform);
		const XMVECTOR rotatedDir = XMVector3Transform(XMVectorSet(launchDirX, launchDirY, launchDirZ, 0), finalTransform);
		XMFLOAT4 startVec;
		XMFLOAT4 dirVec;
		XMStoreFloat4(&startVec, rotatedStart);
		XMStoreFloat4(&dirVec, rotatedDir);
		const float minX = -100;
		const float maxX = 100;
		const float minY = -100;
		const float maxY = 100;//1000;
		const float minZ = -100;
		const float maxZ = 100;
		float tmin = (minX - startVec.x) / dirVec.x;
		float tmax = (maxX - startVec.x) / dirVec.x;

		if (tmin > tmax) swap(tmin, tmax);

		float tymin = (minY - startVec.y) / dirVec.y;
		float tymax = (maxY - startVec.y) / dirVec.y;

		if (tymin > tymax) swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (minZ - startVec.z) / dirVec.z;
		float tzmax = (maxZ - startVec.z) / dirVec.z;

		if (tzmin > tzmax) swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		const XMVECTOR intersectionPoint = XMVectorSet(launchX + tmin * launchDirX, launchY + tmin * launchDirY, launchZ + tmin * launchDirZ, 1);
		//const XMVECTOR intersectionPoint = XMVectorSet(startVec.x + tmin * dirVec.x, startVec.y + tmin * dirVec.y, startVec.z + tmin * dirVec.z, 1);
		//ForceEffect* explosionEffect = new ForceEffect(EffectParameters(this, true), ForceEffectParameters(intersectionPoint, XMVectorSet(0, 0, 0, 0), 50000, false, false, false, 400 * 400, .1f));
#ifndef _RTS_GAME
		DecentMissile* p = new DecentMissile(ObjectParameters(this),
			MoveableParameters(intersectionPoint,
				XMVectorSet(0, 0, 0, 0),
				XMVectorSet(0, 0, 0, 0),
				XMVectorSet(0, 0, 0, 0)), true);
#endif
		return true;
	}

#if (false)
	void PhysicalWorld::AddConstraint(Actor* const object1, Actor* const object2)
	{
		this->constraintsD.push_back(ConstraintD(object1, object2));
	}

	void PhysicalWorld::AddConstraint(Actor* const object1, const XMVECTOR& point)
	{
		this->constraintsD.push_back(ConstraintD(object1, point));
	}

	void PhysicalWorld::AddConstraint(Actor* const object1, const XMVECTOR& point, const XMVECTOR& offset1)
	{
		this->constraintsD.push_back(ConstraintD(object1, point, offset1));
	}

	void PhysicalWorld::AddConstraint(Actor* const object1, Actor* const object2, const XMVECTOR& offset1, const XMVECTOR& offset2, const bool immutableObject2)
	{
		this->constraintsD.push_back(ConstraintD(object1, object2, offset1,offset2, immutableObject2));
	}

	void PhysicalWorld::AddAngleConstraint(Actor* const object1, Actor* const object2, const XMVECTOR& offset1, const XMVECTOR& offset2, const bool immutableObject2)
	{
		this->angleConstraintsD.push_back(AngleConstraintD(object1, object2, offset1, offset2, immutableObject2));
	}
#else

	void PhysicalWorld::AddAngleConstraint(Actor* const object1, Actor* const object2, const XMVECTOR& offset1, const XMVECTOR& offset2, const bool immutableObject2)
	{
		this->angleConstraints.push_back(AngleConstraint(object1, object2, offset1, offset2, immutableObject2));
	}
	void PhysicalWorld::AddConstraint(Actor* const object1, Actor* const object2)
	{
		this->constraints.push_back(Constraint(object1, object2));
	}

	void PhysicalWorld::AddConstraint(Actor* const object1, const XMVECTOR& point)
	{
		this->constraints.push_back(Constraint(object1, point));
	}

	void PhysicalWorld::AddConstraint(Actor* const object1, const XMVECTOR& point, const XMVECTOR& offset1)
	{
		this->constraints.push_back(Constraint(object1, point, offset1));
	}

	void PhysicalWorld::AddConstraint(Actor* const object1, Actor* const object2, const XMVECTOR& offset1, const XMVECTOR& offset2, const bool immutableObject2)
	{
		this->constraints.push_back(Constraint(object1, object2, offset1, offset2, immutableObject2));
	}
#endif

	PhysicalWorld::~PhysicalWorld()
	{

	}
}