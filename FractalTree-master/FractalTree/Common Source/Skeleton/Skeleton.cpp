#include "pch.h"
#include "Skeleton.h"
#include "SkeletonSegment.h"
#include "..\Src\UIRenderer.h"
#include <cmath>

using namespace DirectX;
using namespace std;
using Eigen::JacobiSVD;
static int skeletonIndex;
static int foundGoalIndex;

namespace InverseKinematics
{
	Skeleton::Skeleton() :
		hasChanged(false)
	{
		this->goals[GoalType::TARGET_DISTANCE] = vector<SkeletonSegment*>();
		this->goals[GoalType::SEGMENT_HOLD_POSITION] = vector<SkeletonSegment*>();
		this->goals[GoalType::TARGET_FACING] = vector<SkeletonSegment*>();
		this->goals[GoalType::SEGMENT_ABSOLUTE_FACING] = vector<SkeletonSegment*>();
		this->goals[GoalType::SEGMENT_END_Y] = vector<SkeletonSegment*>();
#ifndef _NO_GRAPHICS
		skeletonIndex = RenderingEngine::UIRenderer::AddDebugString("Skeleton Angle: ");
		foundGoalIndex = RenderingEngine::UIRenderer::AddDebugString("Found goal: ");
#endif
	}

	void Skeleton::Update()
	{
		vector<XMVECTOR> angleDeltas;
		int iterationNumber = 0;
		bool foundGoal = true;

		do  //do while goal not found and iterationNumber is less than max iterations per frame
		{
			//get possible list of angles to move skeleton to get closer to goal
			//an angle delta is the angle to add to a joint such that if the jacobian were a perfect representation
			//of the function's inverse, would result in the skeleton reaching all the goals
			//(it is not a perfect representation, so this process is repeated, scaling angle deltas by a constant alpha
			angleDeltas = this->getAnglesForIKIteration();

			//if no angle deltas are returned, then a solution has been found (may or may not satisfy constraints)
			if (angleDeltas.empty())
			{
				//check to make sure the solution satisfies constraints
				for (int a = 0; a < this->segments.size(); ++a)
				{
					//check for constraint satisfaction; move segment into valid constraint space if violated (possibly invalidating solution)
					if (this->segments[a]->checkAndClampAngleRanges())
						foundGoal = false;
				}

				//constraints are satisfied and solution is found; break out of IK loop as its purpose is achieved
				if (foundGoal)
					break;
			}

			//if execution makes it to here no solution found this iteration, or solution violated constraints
			foundGoal = false;

			//check for infinity; reset angles to default if infinity (or strangely large velocity) is found
			bool foundInfinity = false;
			for (int a = 0; a < angleDeltas.size(); ++a)
			{
				XMFLOAT4 testAngles; XMStoreFloat4(&testAngles, angleDeltas[a]);
				if (isnan(testAngles.x) || isnan(testAngles.y) || isnan(testAngles.z)
					|| abs(testAngles.x) > 100 || abs(testAngles.y) > 100 || abs(testAngles.z) > 100)
				{
					foundInfinity = true;
					this->resetSegments();
					break;
				}
			}
			
			if (foundInfinity)
				continue; //infinity was found; skip rest of this loop iteration (continue means "skip", not sure why it's called continue)

			//add the angle deltas computed from this iteration of the IK solver to each segment joint
			//scale the angle deltas by ikAlpha (small multiplier) to account for fact that jacobian derivative
			//is only useful nearby current configuration, so a large change in angle from the same jacobian would be erroneous
			for (int a = 0; a < angleDeltas.size(); ++a)
			{
				this->segments[a]->Update(XMVectorMultiply(angleDeltas[a], XMVectorReplicate(FractalTreeResources::ikAlpha)));
			}

		} while (!foundGoal && iterationNumber++ < FractalTreeResources::ikIterationCount);

		for (int a = 0; a < this->segments.size(); ++a)
		{
			RenderingEngine::UIRenderer::SetDebugString(this->debugIndexes[a], "Angles: " + getVectorStringDeg(this->segments[a]->GetPitchYawRoll()) +
				" Default: " + getVectorStringDeg(this->segments[a]->defaultPitchYawRoll) + " Min: " + getVectorStringDeg(XMLoadFloat4(&this->segments[a]->minValues))
				+ " Max: " + getVectorStringDeg(XMLoadFloat4(&this->segments[a]->maxValues)));
		}

		if (foundGoal)
		{
			this->failFrames = 0;
		}
		else
		{
			++this->failFrames;
		}

		RenderingEngine::UIRenderer::SetDebugString(foundGoalIndex, "Found goal: " + to_string(foundGoal) + " (Fail frames: " + to_string(this->failFrames) + ")");
		
		//failed to find a solution in the allotted number of frames; reset to a new starting configuration and try again
		if (this->failFrames >= FractalTreeResources::maxFailCount || FractalTreeResources::resetIK)
		{
			FractalTreeResources::resetIK = false;
			this->resetSegmentsToBoundaries();
			this->failFrames = 1;
			for (int a = 0; a < this->segments.size(); ++a)
			{
				RenderingEngine::UIRenderer::SetDebugString(this->debugIndexes[a], "Angles: " + getVectorString(this->segments[a]->GetPitchYawRoll()));
			}
		}
	}

	vector<XMVECTOR> Skeleton::getAnglesForIKIteration() const
	{
		//Determine the angle for each segment to be set to, so that the endpoint of this skeleton is at desiredDist
		//from the target, as well as facing the target

		//Given each segment's angles,
		//a function yielding the distance from the target, and the angle of the end segment to the target can be derived.
		//Given the inverse of this function, the angles required for the skeleton's end point to be at desiredDistance,
		//and facing angle to be facing the target can be calculated

		//when the skeleton is facing the target, the dot product between the end segment's facing vector, and the vector
		//from its end point to the target point will be 1

		//the function is: f(<segment angle list>) = <distance from target, dot product between segment facing and target>
		//deriving the inverse of this function means we have a function which:
		//inverse function: fInv(<desired distance to target, 1>) = <required angle list>;

		string dispString = "";

		//only some segments in the skeleton have a goal to satisfy (example: ends of arms, or feet)
		//a segment can have multiple goals
		//example: 
		   //goal 1: end point of segment must be certain distance from target
		   //goal 2: same segment must also be pointed towards target
		for (int i = 0; i < this->segmentsWithGoals.size(); ++i)
		{
			SkeletonSegment* const goalSegment = this->segmentsWithGoals[i];
			const bool isFootGoal = goalSegment->hasHoldPositionGoal(); //foot goals mean "keep segment endpoint the same as it was at beginning of frame"
			
			//the start and end point of the segment that has the goal
			const TwoVectors startAndEnd = goalSegment->GetSegmentStartAndEnd();
			//the target point for a foot goal is the initial location of the foot (don't want it to ever move)
			const XMVECTOR targetPoint = isFootGoal ? goalSegment->GetOriginalEndPoint() : this->currentDestination;
			const XMVECTOR vectorFromEndPointToTarget = XMVectorSubtract(targetPoint, startAndEnd.endpoint);

			if (goalSegment->hasDistanceGoal() || isFootGoal)
			{
				XMFLOAT4 distanceToTarget;
				XMStoreFloat4(&distanceToTarget, XMVector3Length(vectorFromEndPointToTarget));
				goalSegment->remainingDist = isFootGoal ? distanceToTarget.x : distanceToTarget.x - desiredDist;
				goalSegment->currentDist = distanceToTarget.x;

				dispString += "Dist1: " + to_string(distanceToTarget.x);
			}
			/*if (goalSegment->hasYCoordinateGoal())
			{
				XMFLOAT4 loc;
				XMStoreFloat4(&loc, XMVector3Length(vectorFromEndPointToTarget));
				goalSegment->remainingDist = isFootGoal ? distanceToTarget.x : distanceToTarget.x - desiredDist;
				goalSegment->currentDist = distanceToTarget.x;

				dispString += "Dist1: " + to_string(distanceToTarget.x);
			}*/
			if (goalSegment->hasFacingGoal())
			{
				const XMVECTOR vectorFromEndNormalized = XMVector3Normalize(vectorFromEndPointToTarget);
				const XMVECTOR facingVector = XMVector3Normalize(XMVectorSubtract(startAndEnd.endpoint, startAndEnd.startPoint));
				XMFLOAT4 currentDotProduct;
				XMStoreFloat4(&currentDotProduct, XMVector3Dot(facingVector, vectorFromEndNormalized));
				goalSegment->currentDotProduct = currentDotProduct.x;
				dispString += " Dot1: " + to_string(currentDotProduct.x);
			}
			if (goalSegment->hasAbsoluteFacingGoal())
			{
				const XMVECTOR facingVector = XMVector3Dot(XMVector3Normalize(XMVectorSubtract(startAndEnd.endpoint, startAndEnd.startPoint)), goalSegment->GetAbsoluteFacingGoalVector());
				XMFLOAT4 facingDot;
				XMStoreFloat4(&facingDot, facingVector);
				dispString += " Facing1: " + to_string(facingDot.x);
				goalSegment->currentSegmentDotProduct = facingDot.x;
			}
		}

#ifndef _NO_GRAPHICS
		RenderingEngine::UIRenderer::SetDebugString(skeletonIndex, dispString);
#endif
		//create a matrix of the currently required deltas to make the dot product 1, and the distance to target correct
		//populate this matrix with the current error (error being how far away goals are from being satisfied)
		MatrixType finalDeltaEMatrix(this->totalGoalCount, 1);
		bool foundGoal = true;
		int goalCount = 0;

		for (const SkeletonSegment* const goalSegment : this->segmentsWithGoals)
		{
			if (goalSegment->hasFacingGoal())
			{
				//facing goal means check dot product from segment vector to target point
				const FloatType v1 = 1 - goalSegment->currentDotProduct;
				finalDeltaEMatrix(goalCount++, 0) = v1;

				if (v1 > FractalTreeResources::dotProductTolerance)
					foundGoal = false;
			}
			else if (goalSegment->hasAbsoluteFacingGoal())
			{
				//facing goal means check dot product from segment vector to target point
				const FloatType v1 = 1 - goalSegment->currentSegmentDotProduct;
				finalDeltaEMatrix(goalCount++, 0) = v1;

				if (v1 > FractalTreeResources::dotProductTolerance)
					foundGoal = false;
			}
			if (goalSegment->hasDistanceGoal() || goalSegment->hasHoldPositionGoal())
			{
				if (goalSegment->remainingDist > 5)
					foundGoal = false;
				
				finalDeltaEMatrix(goalCount++, 0) = -goalSegment->remainingDist;
			}
		}

		if (foundGoal)
			return vector<XMVECTOR>(); //return empty list of angles to signify skeleton has satisfied all goals
		
		//the jacobian is the linear approximation to the function described above; it is a matrix of derivatives
		const MatrixType jacobian = this->getJacobian(FractalTreeResources::ikEpsilon);

		//use svd of jacobian instead of pseudoinverse to get angle delta matrix
		const JacobiSVD<MatrixType> svd(jacobian, Eigen::ComputeThinU | Eigen::ComputeThinV);

		//get the singular values and put them in a diagonal matrix
		const auto singularValues = svd.singularValues();
		MatrixType singularDiagonal = MatrixType::Zero(singularValues.size(), singularValues.size());
		for (int v = 0; v < singularValues.size(); ++v)
		{
			singularDiagonal(v, v) = singularValues(v, 0);
		}

		//a column vector of all dof's calculated angle deltas
		const MatrixType angleDisplacementVector = svd.matrixV() * singularDiagonal.inverse() * svd.matrixU().transpose() * finalDeltaEMatrix;

		int dofIndex = 0;
		vector<XMVECTOR> angleDeltas;
		angleDeltas.reserve(this->segments.size());

		for (const SkeletonSegment* const s : this->segments)
		{
			const FloatType dX = s->xAvailable ? angleDisplacementVector(dofIndex++, 0) : 0;
			const FloatType dY = s->yAvailable ? angleDisplacementVector(dofIndex++, 0) : 0;
			const FloatType dZ = s->zAvailable ? angleDisplacementVector(dofIndex++, 0) : 0;
			angleDeltas.push_back(XMVectorSet(dX, dY, dZ, 0));
		}

		return angleDeltas;
	}

	MatrixType Skeleton::getJacobian(const FloatType der) const
	{
		//result jacobian will have one column for each degree of freedom; two row for each coordinate in result,
		//plus one row for each violated joint angle limit constraint
		MatrixType jacobian(this->totalGoalCount, this->dofCount);

		//each end segment represents a goal for the IK solver; all goals must be solved simultaneously
		int rowIndex = 0;
		for (int endSegmentIndex = 0; endSegmentIndex < this->segmentsWithGoals.size(); ++endSegmentIndex)
		{
			int columnIndex = 0;
			const SkeletonSegment* const endSegment = this->segmentsWithGoals[endSegmentIndex];
			for (const SkeletonSegment* const currentSegment : this->segments)
			{
				//estimate the partial derivatives by calculating the goal error before and after
				//adding a tiny epsilon to the angles of each segment one at a time

				//in other words, perturb each segment angle a slight bit and record how the dot product and distance from
				//target change, and call this result a derivative that estimates the function itself at the particular set of input angles

				//repeat this for each degree of freedom for all segments
				if (currentSegment->xAvailable)
				{
					this->computeJacobianRows(XMVectorSet(der, 0, 0, 0),
						endSegment,
						currentSegment,
						jacobian,
						rowIndex,
						columnIndex,
						der);

					++columnIndex;
				}

				if (currentSegment->yAvailable)
				{
					this->computeJacobianRows(XMVectorSet(0, der, 0, 0),
						endSegment,
						currentSegment,
						jacobian,
						rowIndex,
						columnIndex,
						der);

					++columnIndex;
				}

				if (currentSegment->zAvailable)
				{
					this->computeJacobianRows(XMVectorSet(0, 0, der, 0),
						endSegment,
						currentSegment,
						jacobian,
						rowIndex,
						columnIndex,
						der);

					++columnIndex;
				}
			}

			rowIndex += endSegment->goals.size();
		}

		return jacobian;
	}

	void Skeleton::computeJacobianRows(const XMVECTOR& derDelta,
		const SkeletonSegment* const endSegment,
		const SkeletonSegment* const currentSegment,
		MatrixType& jacobian,
		const int rowIndex,
		const int columnIndex,
		const FloatType der) const
	{
		//the start and end point of the end segment (or goal segment) after modifying the angle of currentSegment by derDelta epsilon
		const TwoVectors newStartAndEnd = endSegment->GetSegmentStartAndEnd(currentSegment, derDelta);

		int offset = 0;
		if (endSegment->hasFacingGoal())
		{
			const XMVECTOR vectorFromEnd = XMVector3Normalize(XMVectorSubtract(this->currentDestination, newStartAndEnd.endpoint));
			const XMVECTOR facingVector = XMVector3Normalize(XMVectorSubtract(newStartAndEnd.endpoint, newStartAndEnd.startPoint));
			XMFLOAT4 newDotProduct;
			XMStoreFloat4(&newDotProduct, XMVector3Dot(facingVector, vectorFromEnd));
			jacobian(rowIndex, columnIndex) = (newDotProduct.x - endSegment->currentDotProduct) / der;
			++offset;
		}
		else if (endSegment->hasAbsoluteFacingGoal())
		{
			const XMVECTOR facingVector = XMVector3Normalize(XMVectorSubtract(newStartAndEnd.endpoint, newStartAndEnd.startPoint));
			XMFLOAT4 newDotProduct;
			XMStoreFloat4(&newDotProduct, XMVector3Dot(facingVector, endSegment->GetAbsoluteFacingGoalVector()));
			jacobian(rowIndex, columnIndex) = (newDotProduct.x - endSegment->currentSegmentDotProduct) / der;
			++offset;
		}
		if (endSegment->hasDistanceGoal())
		{
			XMFLOAT4 newDistance;
			XMStoreFloat4(&newDistance, XMVector3Length(XMVectorSubtract(newStartAndEnd.endpoint, this->currentDestination)));
			jacobian(rowIndex + offset, columnIndex) = (newDistance.x - endSegment->currentDist) / der;
			++offset;
		}
		else if (endSegment->hasHoldPositionGoal())
		{
			XMFLOAT4 newDistance;
			XMStoreFloat4(&newDistance, XMVector3Length(XMVectorSubtract(newStartAndEnd.endpoint, endSegment->GetOriginalEndPoint())));
			jacobian(rowIndex + offset, columnIndex) = (newDistance.x - endSegment->currentDist) / der;
			++offset;
		}
	}

	double Skeleton::desiredDist = 1;

	unsigned int Skeleton::AddSegment(SkeletonSegment* const segment, const bool isRoot)
	{
		this->segments.push_back(segment);
		if (isRoot)
		{
			this->rootSegment = segment;
		}

		this->debugIndexes.push_back(RenderingEngine::UIRenderer::AddDebugString("Segment Angles: "));

		if (!segment->name.empty())
		{
			this->nameMap[segment->name] = segment;
		}
		this->dofCount += segment->dofCount;
		return this->segmentCount++;
	}

	void Skeleton::deleteLastSegment()
	{
		const SkeletonSegment* const seg = this->segments.back();
		this->segments.resize(this->segments.size() - 1);

		this->dofCount -= seg->dofCount;

		if (!seg->name.empty())
		{
			this->nameMap.erase(this->nameMap.find(seg->name));
		}
		this->ResetEndSegments();
		--this->segmentCount;
		this->hasChanged = true;
		delete seg;
	}

	void Skeleton::resetSegments()
	{
		for (SkeletonSegment* const segment : this->segments)
			segment->ResetAngles();
	}

	//reset segment angles to randomly be on upper or lower bound of angle limits
	inline void Skeleton::resetSegmentsToBoundaries()
	{
		for (int a = 0; a < this->segments.size(); ++a)
		{
			XMFLOAT4 currentAngles;
			XMStoreFloat4(&currentAngles, this->segments[a]->GetPitchYawRoll());
			if (this->segments[a]->minValues.x > -100 && this->segments[a]->xAvailable)
			{
				currentAngles.x = rand() % 2 ? this->segments[a]->minValues.x : this->segments[a]->maxValues.x;
			}
			if (this->segments[a]->minValues.y > -100 && this->segments[a]->yAvailable)
			{
				currentAngles.y = rand() % 2 ? this->segments[a]->minValues.y : this->segments[a]->maxValues.y;
			}
			if (this->segments[a]->minValues.z > -100 && this->segments[a]->zAvailable)
			{
				currentAngles.z = rand() % 2 ? this->segments[a]->minValues.z : this->segments[a]->maxValues.z;
			}

			this->segments[a]->SetPitchYawRoll(XMLoadFloat4(&currentAngles));
		}
	}

	void Skeleton::ResetEndSegments()
	{
		this->segmentsWithGoals.clear();
		this->goals.clear();
		this->goals[GoalType::TARGET_DISTANCE] = vector<SkeletonSegment*>();
		this->goals[GoalType::SEGMENT_HOLD_POSITION] = vector<SkeletonSegment*>();
		this->goals[GoalType::TARGET_FACING] = vector<SkeletonSegment*>();
		this->totalGoalCount = 0;
		for (SkeletonSegment* const s : this->segments)
		{
			if (!s->goals.empty())
			{
				this->segmentsWithGoals.push_back(s);
			}
			for (const auto p : s->goals)
			{
				this->goals[p].push_back(s);
			}
		}

		for (const auto s : this->goals)
		{
			this->totalGoalCount += s.second.size();
		}
	}

	void Skeleton::SetDestination(const XMVECTOR newDestination)
	{
		XMFLOAT4 l;
		XMStoreFloat4(&l, XMVector3Length(XMVectorSubtract(newDestination, this->currentDestination)));
		if (l.x > .001f)
		{
			this->failFrames = 1;
		}

		this->currentDestination = newDestination;
	}

	Skeleton::~Skeleton()
	{
		delete this->rootSegment;
	}
}
