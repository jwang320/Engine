#include "pch.h"
#include "SkeletonSegment.h"
#include "Skeleton.h"

using namespace DirectX;
using namespace SimulationEngine;

namespace InverseKinematics
{
	SkeletonSegment::SkeletonSegment(
		Skeleton* const parentSkeleton,
		SkeletonSegment* const parent,
		const float length,
		const bool hasGoal,
		const bool isFootGoal,
		const XMFLOAT4& minValues,
		const XMFLOAT4& maxValues,
		const std::string& name,
		const std::string& middleName,
		const Connection& parentConnection,
		const std::string& parentName) :
		parentSkeleton(parentSkeleton),
		parentSegment(parent),
		length(length),
		name(name),
		middleName(middleName),
		offset(parentConnection.offset),
		dofCount(parentConnection.dofCount),
		xAvailable(parentConnection.xAvailable),
		yAvailable(parentConnection.yAvailable),
		zAvailable(parentConnection.zAvailable),
		pitchYawRoll(parentConnection.pitchYawRoll),
		defaultPitchYawRoll(parentConnection.pitchYawRoll),
		parentName(parentName),
		localId(parentSkeleton->AddSegment(this, parent != nullptr))
	{
		if (parent != nullptr)
		{
			parent->addChild(this);
		}
		this->setMinAndMax(minValues, maxValues);

		//figure out why setting this here does not work
		//this->originalPosition = this->GetSegmentStartAndEnd().endpoint;

		//legacy version of goal setting
		if (hasGoal)
		{
			this->toggleGoal(GoalType::TARGET_DISTANCE);
			this->toggleGoal(GoalType::TARGET_FACING);
		}
		else if (isFootGoal)
		{
			this->toggleGoal(GoalType::SEGMENT_HOLD_POSITION);
		}
	}

	void SkeletonSegment::Update(const XMVECTOR& angleDelta)
	{
		if (!this->hasInitializedOriginalEndPoint)
		{
			this->originalPosition = this->GetSegmentStartAndEnd().endpoint;
			this->hasInitializedOriginalEndPoint = true;
		}

		this->SetPitchYawRoll(clampAngle(XMVectorAdd(this->pitchYawRoll, angleDelta)));
	}

	//toggle whether this segment can rotate on x axis
	void SkeletonSegment::SetXAvailable(const bool available)
	{
		if (this->xAvailable != available)
		{
			if (!this->xAvailable)
			{
				++this->dofCount;
				this->parentSkeleton->IncrementDofCount();
			}
			else
			{
				--this->dofCount;
				this->parentSkeleton->DecrementDofCount();
			}
		}

		this->xAvailable = available;
	}

	//toggle whether this segment can rotate on y axis
	void SkeletonSegment::SetYAvailable(const bool available)
	{
		if (this->yAvailable != available)
		{
			if (!this->yAvailable)
			{
				++this->dofCount;
				this->parentSkeleton->IncrementDofCount();
			}
			else
			{
				--this->dofCount;
				this->parentSkeleton->DecrementDofCount();
			}
		}

		this->yAvailable = available;
	}

	//toggle whether this segment can rotate on z axis
	void SkeletonSegment::SetZAvailable(const bool available)
	{
		if (this->zAvailable != available)
		{
			if (!this->zAvailable)
			{
				++this->dofCount;
				this->parentSkeleton->IncrementDofCount();
			}
			else
			{
				--this->dofCount;
				this->parentSkeleton->DecrementDofCount();
			}
		}

		this->zAvailable = available;
	}

	void SkeletonSegment::toggleGoal(const GoalType goalType)
	{
		if (const auto f = this->goals.find(goalType); f != this->goals.end())
			this->goals.erase(f);
		else
			this->goals.insert(goalType);
		this->parentSkeleton->ResetEndSegments();
	}

	void SkeletonSegment::setMinAndMax(XMFLOAT4 min, XMFLOAT4 max)
	{
		XMFLOAT4 def;
		XMStoreFloat4(&def, this->defaultPitchYawRoll);
		def.x = clampAngle(def.x);
		def.y = clampAngle(def.y);
		def.z = clampAngle(def.z);

		if (min.x > -100)
		{
			min.x = clampAngle(min.x + def.x);
			max.x = clampAngle(max.x + def.x);
		}
		if (min.y > -100)
		{
			min.y = clampAngle(min.y + def.y);
			max.y = clampAngle(max.y + def.y);
		}
		if (min.z > -100)
		{
			min.z = clampAngle(min.z + def.z);
			max.z = clampAngle(max.z + def.z);
		}

		this->minValues = { min.x, min.y, min.z, 0 };
		this->maxValues = { max.x, max.y, max.z, 0 };
		this->defaultPitchYawRoll = XMLoadFloat4(&def);
	}

	void SkeletonSegment::changeXMax(const float delta)
	{
		this->maxValues.x = delta;
		this->setMinAndMax(this->minValues, this->maxValues);
	}

	void SkeletonSegment::changeXMin(const float delta)
	{
		this->minValues.x = delta;
		this->setMinAndMax(this->minValues, this->maxValues);
	}

	void SkeletonSegment::changeYMax(const float delta)
	{
		this->maxValues.y = delta;
		this->setMinAndMax(this->minValues, this->maxValues);
	}

	void SkeletonSegment::changeYMin(const float delta)
	{
		this->minValues.y = delta;
		this->setMinAndMax(this->minValues, this->maxValues);
	}

	void SkeletonSegment::changeZMax(const float delta)
	{
		this->maxValues.z = delta;
		this->setMinAndMax(this->minValues, this->maxValues);
	}

	void SkeletonSegment::changeZDefault(const float delta)
	{
		this->defaultPitchYawRoll = DirectX::XMVectorSetZ(this->defaultPitchYawRoll, delta);
		this->setMinAndMax(this->minValues, this->maxValues);
	}

	void SkeletonSegment::changeZMin(const float delta)
	{
		this->minValues.z = delta;
		this->setMinAndMax(this->minValues, this->maxValues);
	}


	SkeletonSegment::~SkeletonSegment()
	{
		//FIX
		for (const SkeletonSegment* childSegment : this->childSegments)
		{
			//delete childSegment;
		}
	}
}
