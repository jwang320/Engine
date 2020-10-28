#pragma once
#include "SegmentConnection.h"
#include "BasicTypes.h"

namespace SimulationEngine { class Actor; }
namespace InverseKinematics
{
	class Skeleton;

	class SkeletonSegment
	{
	public:
		Skeleton* const parentSkeleton;
		SkeletonSegment* parentSegment = nullptr; //null if root
		SimulationEngine::Actor* owningActor = nullptr;
		std::vector<SkeletonSegment*> childSegments;
		std::unordered_set<GoalType> goals; //list of goals this segment must achieve, used only in dynamic version currently
		const std::string name;
		const std::string middleName;

		int dofCount = 0;
		bool xAvailable;
		bool yAvailable;
		bool zAvailable;

		//goal data
		FloatType currentDist;
		FloatType remainingDist;
		FloatType currentDotProduct;
		FloatType currentSegmentDotProduct;

		DirectX::XMVECTOR defaultPitchYawRoll = DirectX::XMVectorZero();
		const std::string parentName;
		DirectX::XMFLOAT4 minValues = DirectX::XMFLOAT4();
		DirectX::XMFLOAT4 maxValues = DirectX::XMFLOAT4();

	private:
		DirectX::XMVECTOR originalPosition = DirectX::XMVectorSet(0, 0, 0, 1);
		DirectX::XMVECTOR offset;
		DirectX::XMVECTOR absoluteFacingGoalVector = DirectX::XMVectorSet(0, 1, 0, 0);
		FloatType yGoal = 0;
		mutable bool transformationOutOfDate = true;
		mutable DirectX::XMMATRIX cachedTransform;
		float length;
		bool hasInitializedOriginalEndPoint = false;
		//always use setter to set
		DirectX::XMVECTOR pitchYawRoll = DirectX::XMVectorZero();
		//keep this last to preserve proper initialization order
		const unsigned int localId;

	public:
		SkeletonSegment(Skeleton* const parentSkeleton, SkeletonSegment* const parent,
			const float length,
			const bool hasGoal,
			const bool isFootGoal,
			const DirectX::XMFLOAT4& minValues,
			const DirectX::XMFLOAT4& maxValues,
			const std::string& name,
			const std::string& middleName,
			const Connection& parentConnection,
			const std::string& parentName);
		void Update(const DirectX::XMVECTOR& angleDelta);
		~SkeletonSegment();

	public:
		const SkeletonSegment* GetParentSegment() const { return this->parentSegment; } //null if root
		unsigned int GetLocalId() const { return this->localId; }
		std::vector<SkeletonSegment*> GetChildSegments() const { return this->childSegments; }
		float GetLength() const { return this->length; }
		DirectX::XMVECTOR GetScale() const { return DirectX::XMVectorSet(3, this->length, 3, 1); }
		DirectX::XMVECTOR GetOriginalEndPoint() const { return this->originalPosition; }
		//toggle whether this segment can rotate on x axis
		void SetXAvailable(const bool available);
		//toggle whether this segment can rotate on y axis
		void SetYAvailable(const bool available);
		//toggle whether this segment can rotate on z axis
		void SetZAvailable(const bool available);
		void toggleGoal(const GoalType goalType);
		void SetAbsoluteFacingGoalVector(const DirectX::XMVECTOR& goalVector) { this->absoluteFacingGoalVector = DirectX::XMVector3Normalize(goalVector); }
		DirectX::XMVECTOR GetAbsoluteFacingGoalVector() const { return this->absoluteFacingGoalVector; }
		FloatType GetYGoal() const { return this->yGoal; }
		void SetYGoal(const FloatType yGoal) { this->yGoal = yGoal; }
		inline void SetPitchYawRoll(const DirectX::XMVECTOR& pitchYawRoll) { this->pitchYawRoll = pitchYawRoll; this->transformationOutOfDate = true; }
		inline DirectX::XMVECTOR GetPitchYawRoll() const { return this->pitchYawRoll; }
		inline void ResetAngles() { this->SetPitchYawRoll(this->defaultPitchYawRoll); }
		bool IsRoot() const { return this->parentSegment == nullptr; }

		inline DirectX::XMMATRIX GetFullTransformation() const
		{
			if (!this->transformationOutOfDate)
				return this->cachedTransform;
			//this->transformationOutOfDate = false;
			using namespace DirectX;
			const XMMATRIX rotMatrix = this->GetRotationMatrix();
			const XMMATRIX translationMatrix = this->GetTranslationMatrix();
			const XMMATRIX transformation = XMMatrixMultiply(translationMatrix, rotMatrix);
			if (this->IsRoot())
			{
				this->cachedTransform = transformation;
				return transformation;
			}

			this->cachedTransform = XMMatrixMultiply(transformation, this->parentSegment->GetFullTransformation());
			return this->cachedTransform;
		}

		inline DirectX::XMMATRIX GetFullTransformationNoCache() const
		{
			using namespace DirectX;
			const XMMATRIX rotMatrix = this->GetRotationMatrix();
			const XMMATRIX translationMatrix = this->GetTranslationMatrix();
			const XMMATRIX transformation = XMMatrixMultiply(translationMatrix, rotMatrix);

			if (this->IsRoot())
				return transformation;
			

			return XMMatrixMultiply(transformation, this->parentSegment->GetFullTransformationNoCache());
		}

		inline DirectX::XMMATRIX GetFullStartTransformation() const
		{
			using namespace DirectX;
			const XMMATRIX rotMatrix = this->GetRotationMatrix();
			if (this->IsRoot())
				return rotMatrix;
			

			return XMMatrixMultiply(rotMatrix, this->parentSegment->GetFullTransformation());
		}

		inline TwoVectors GetSegmentStartAndEnd() const
		{
			using namespace DirectX;
			const XMMATRIX rotMatrix = this->GetRotationMatrix();
			const XMMATRIX translationMatrix = this->GetTranslationMatrix();
			const XMMATRIX transformation = XMMatrixMultiply(translationMatrix, rotMatrix);

			if (this->parentSegment == nullptr)
			{
				return { XMVectorSet(0, 0, 0, 1), XMVector3Transform(XMVectorSet(0, 0, 0, 1), transformation) };
			}

			const XMMATRIX parentTransformation = this->parentSegment->GetFullTransformation();
			const XMVECTOR startPoint = XMVector3Transform(XMVectorSet(0, 0, 0, 1), parentTransformation);
			const XMVECTOR endPoint = XMVector3Transform(XMVectorSet(0, 0, 0, 1), XMMatrixMultiply(transformation, parentTransformation));
			return { startPoint, endPoint };
		}

		inline TwoVectors GetSegmentStartAndEnd(const SkeletonSegment* const changeMyRotationAmountUsed,
			const DirectX::XMVECTOR& changeAmount) const
		{
			using namespace DirectX;
			XMMATRIX rotMatrix;
			if (changeMyRotationAmountUsed == this)
			{
				rotMatrix = this->GetRotationMatrix(changeAmount);
			}
			else
			{
				rotMatrix = this->GetRotationMatrix();
			}
			const XMMATRIX translationMatrix = this->GetTranslationMatrix();
			const XMMATRIX transformation = XMMatrixMultiply(translationMatrix, rotMatrix);

			if (this->IsRoot())
			{
				return{ XMVectorSet(0, 0, 0, 1), XMVector3Transform(XMVectorSet(0, 0, 0, 1), transformation) };
			}

			const XMMATRIX parentTransformation = this->parentSegment->GetFullTransformation(changeMyRotationAmountUsed, changeAmount);
			const XMVECTOR startPoint = XMVector3Transform(XMVectorSet(0, 0, 0, 1), parentTransformation);
			const XMVECTOR endPoint = XMVector3Transform(XMVectorSet(0, 0, 0, 1), XMMatrixMultiply(transformation, parentTransformation));
			return{ startPoint, endPoint };
		}

		inline DirectX::XMMATRIX GetFullTransformation(const SkeletonSegment* const changeMyRotationAmountUsed,
			const DirectX::XMVECTOR& changeAmount) const
		{
			using namespace DirectX;
			XMMATRIX rotMatrix;
			if (changeMyRotationAmountUsed == this)
			{
				rotMatrix = this->GetRotationMatrix(changeAmount);
			}
			else
			{
				rotMatrix = this->GetRotationMatrix();
			}
			const XMMATRIX translationMatrix = this->GetTranslationMatrix();
			const XMMATRIX transformation = XMMatrixMultiply(translationMatrix, rotMatrix);

			if (this->IsRoot())
			{
				return transformation;
			}

			return XMMatrixMultiply(transformation, this->parentSegment->GetFullTransformation(changeMyRotationAmountUsed, changeAmount));
		}

		inline DirectX::XMMATRIX GetRotationMatrix() const
		{
			using namespace DirectX;
			/*XMFLOAT4 rot;
			XMStoreFloat4(&rot, this->pitchYawRoll);
			return XMMatrixMultiply(XMMatrixRotationY(rot.y),
				XMMatrixMultiply(XMMatrixRotationZ(rot.z), XMMatrixRotationX(rot.x)));*/
			return XMMatrixRotationRollPitchYawFromVector(this->pitchYawRoll);
		}
		inline DirectX::XMMATRIX GetRotationMatrix(const DirectX::XMVECTOR& delta) const
		{
			using namespace DirectX;
			/*XMFLOAT4 rot;
			XMStoreFloat4(&rot, XMVectorAdd(this->pitchYawRoll, delta));
			return XMMatrixMultiply(XMMatrixRotationY(rot.y),
				XMMatrixMultiply(XMMatrixRotationZ(rot.z), XMMatrixRotationX(rot.x)));*/
			return XMMatrixRotationRollPitchYawFromVector(XMVectorAdd(this->pitchYawRoll, delta));
		}

		inline DirectX::XMMATRIX GetTranslationMatrix() const
		{
			using namespace DirectX;
			return XMMatrixTranslationFromVector(XMVectorAdd(XMVectorSet(0, this->length, 0, 0), this->offset));
		}

		void changeXMax(const float delta);

		void changeXMin(const float delta);

		void changeYMax(const float delta);

		void changeYMin(const float delta);

		void changeZMax(const float delta);

		void changeZDefault(const float delta);

		void changeZMin(const float delta);

		inline bool hasDistanceGoal() const
		{
			return this->goals.find(GoalType::TARGET_DISTANCE) != this->goals.end();
		}

		inline bool hasFacingGoal() const
		{
			return this->goals.find(GoalType::TARGET_FACING) != this->goals.end();
		}

		inline bool hasHoldPositionGoal() const
		{
			return this->goals.find(GoalType::SEGMENT_HOLD_POSITION) != this->goals.end();
		}

		inline bool hasAbsoluteFacingGoal() const
		{
			return this->goals.find(GoalType::SEGMENT_ABSOLUTE_FACING) != this->goals.end();
		}

		inline bool hasYCoordinateGoal() const
		{
			return this->goals.find(GoalType::SEGMENT_END_Y) != this->goals.end();
		}

		inline bool checkAndClampAngleRanges()
		{
			using namespace DirectX;
			XMFLOAT4 newAngles;
			XMStoreFloat4(&newAngles, this->pitchYawRoll);
			bool foundOutOfBounds = false;
			if (this->minValues.x < this->maxValues.x)
			{
				if (newAngles.x < this->minValues.x)
				{
					newAngles.x = this->minValues.x;
					foundOutOfBounds = true;
				}
				else if (newAngles.x > this->maxValues.x)
				{
					newAngles.x = this->maxValues.x;
					foundOutOfBounds = true;
				}
			}
			else if (newAngles.x < this->minValues.x && newAngles.x > this->maxValues.x)
			{
				foundOutOfBounds = true;
				//set to closest bounds
				newAngles.x = (this->minValues.x - newAngles.x > newAngles.x - this->maxValues.x) ? this->maxValues.x : this->minValues.x;
			}
			
			if (this->minValues.y < this->maxValues.y)
			{
				if (newAngles.y < this->minValues.y)
				{
					newAngles.y = this->minValues.y;
					foundOutOfBounds = true;
				}
				else if (newAngles.y > this->maxValues.y)
				{
					newAngles.y = this->maxValues.y;
					foundOutOfBounds = true;
				}
			}
			else if (newAngles.y < this->minValues.y && newAngles.y > this->maxValues.y)
			{
				foundOutOfBounds = true;
				newAngles.y = (this->minValues.y - newAngles.y > newAngles.y - this->maxValues.y) ? this->maxValues.y : this->minValues.y;
			}
			
			if (this->minValues.z < this->maxValues.z)
			{
				if (newAngles.z < this->minValues.z)
				{
					newAngles.z = this->minValues.z;
					foundOutOfBounds = true;
				}
				else if (newAngles.z > this->maxValues.z)
				{
					newAngles.z = this->maxValues.z;
					foundOutOfBounds = true;
				}
			}
			else if (newAngles.z < this->minValues.z && newAngles.z > this->maxValues.z)
			{
				foundOutOfBounds = true;
				newAngles.z = (this->minValues.z - newAngles.z > newAngles.z - this->maxValues.z) ? this->maxValues.z : this->minValues.z;
			}

			if (foundOutOfBounds)
			{
				this->SetPitchYawRoll(XMLoadFloat4(&newAngles));
				return true;
			}

			return false;
		}
		inline DirectX::XMVECTOR GetCompoundPitchYawRoll() const 
		{ 
			if (this->parentSegment == nullptr) return this->pitchYawRoll; 
			return DirectX::XMVectorAddAngles(this->parentSegment->GetCompoundPitchYawRoll(), this->pitchYawRoll);
		}

		private:
			void setMinAndMax(DirectX::XMFLOAT4 min, DirectX::XMFLOAT4 max);
			void addChild(SkeletonSegment* const child) { this->childSegments.push_back(child); }
	};
}