#include "pch.h"
#include "TestUserOfSkeleton.h"
#include "DecentMissile.h"
#include <stack>
#include "..\Common Source\json\json.hpp"
#include "..\Common Source\Skeleton\SkeletonSegment.h"
#include "..\Common Source\Skeleton\Skeleton.h"

using namespace DirectX;
using namespace InverseKinematics;

static bool hasSkeletonTextYet = false;
int skeletonTextIndex;


namespace SimulationEngine
{
	void parseSegment(nlohmann::json& j, Skeleton* const skeleton, const XMVECTOR& qOrientation, SkeletonSegment* const parentSegment, TestUserOfSkeleton* const sk, const XMMATRIX& parentTransformation);
	float TestUserOfSkeleton::ikDistance = 200;
	bool TestUserOfSkeleton::fireNext = false;
	TestUserOfSkeleton::TestUserOfSkeleton(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		destination(XMVectorSet(5, 60, 10, 1))
	{
		this->collisionType = NONE;

		this->componentFlags |= PhysicalWorld::PHYSICS;
		this->componentFlags |= PhysicalWorld::PATH;

		this->ignoreForceEffects = true;
		this->objectType = ObjectTypes::TestUserOfSkeleton;

		this->GenerateEverything2();
	}

	void TestUserOfSkeleton::GenerateEverything2()
	{
		using namespace nlohmann;
		using namespace std;
		ifstream inputFile("C:/Users/Matt/Source/Fractal Tree/FractalTree/Resources/Robot Skeletons/TestSkeleton9.json");

		json j = json::parse(inputFile);
		inputFile.close();
		this->skeleton = new Skeleton();
		json j2 = j.at("segments");
		for (auto seg : j2)
		{
			parseSegment(seg,
				this->skeleton,
				this->qOrientation,
				nullptr,
				this,
				XMMatrixTranslationFromVector(this->location));
		}

		for (SkeletonSegment* const segment : this->skeleton->GetSegments())
		{
			if (!segment->parentName.empty())
			{
				SkeletonSegment* const parent = this->skeletonSegmentMap.at(segment->parentName);
				segment->parentSegment = parent;
				parent->childSegments.push_back(segment);
			}
		}
		this->initializeCollisionHulls();
	}

	void parseSegment(nlohmann::json& j,
		Skeleton* const skeleton,
		const XMVECTOR& qOrientation,
		SkeletonSegment* const parentSegment,
		TestUserOfSkeleton* const sk,
		const XMMATRIX& parentTransformation)
	{
		using namespace nlohmann;
		using namespace std;
		bool isGoal = false;
		bool isFootGoal = false;
		float length = 50;
		float xOffset = 0;
		float yOffset = 0;
		float zOffset = 0;
		float zRot = 0;
		float zRotAll = 0;
		float yRot = 0;
		float yRotAll = 0;
		float xRot = 0;
		float xRotAll = 0;
		float minX = -1000;
		float minY = -1000;
		float minZ = -1000;
		float maxX = 1000;
		float maxY = 1000;
		float maxZ = 1000;
		int repeat = 1;
		bool xAvailable = false;
		bool yAvailable = false;
		bool zAvailable = false;
		bool calculateFoot = false;
		bool skip = false;
		bool isRoot = false;
		string baseName = "";
		string parentName = "";
		string endName = "";
		json* children = nullptr;
		for (json::iterator it = j.begin(); it != j.end(); ++it)
		{
			if (it.key() == "isGoal")
			{
				isGoal = *it;
			}
			else if (it.key() == "isFootGoal")
			{
				isFootGoal = *it;
			}
			else if (it.key() == "length")
			{
				length = *it;
			}
			else if (it.key() == "xOffset")
			{
				xOffset = *it;
			}
			else if (it.key() == "yOffset")
			{
				yOffset = *it;
			}
			else if (it.key() == "zOffset")
			{
				zOffset = *it;
			}
			else if (it.key() == "repeat")
			{
				repeat = *it;
			}
			else if (it.key() == "xAvailable")
			{
				xAvailable = *it;
			}
			else if (it.key() == "yAvailable")
			{
				yAvailable = *it;
			}
			else if (it.key() == "zAvailable")
			{
				zAvailable = *it;
			}
			else if (it.key() == "zRot")
			{
				zRot = *it;
			}
			else if (it.key() == "yRot")
			{
				yRot = *it;
			}
			else if (it.key() == "xRot")
			{
				xRot = *it;
			}
			else if (it.key() == "xRotAll")
			{
				xRotAll = *it;
			}
			else if (it.key() == "yRotAll")
			{
				yRotAll = *it;
			}
			else if (it.key() == "zRotAll")
			{
				zRotAll = *it;
			}
			else if (it.key() == "minX")
			{
				minX = *it;
			}
			else if (it.key() == "minY")
			{
				minY = *it;
			}
			else if (it.key() == "minZ")
			{
				minZ = *it;
			}
			else if (it.key() == "maxX")
			{
				maxX = *it;
			}
			else if (it.key() == "maxY")
			{
				maxY = *it;
			}
			else if (it.key() == "maxZ")
			{
				maxZ = *it;
			}
			else if (it.key() == "segments")
			{
				children = &*it;
			}
			else if (it.key() == "name")
			{
				baseName = it.key();
			}
			else if (it.key() == "isRoot")
			{
				isRoot = *it;
			}
			else if (it.key() == "parent")
			{
				//parentName = (std::string)*it;
				parentName = it.key();
			}
			else if (it.key() == "endName")
			{
				//endName = (std::string)*it;
				endName = it.key();
			}
			else if (it.key() == "skip")
			{
				skip = *it;
				if (skip)
					break;
			}
		}

		if (skip)
		{
			return;
		}

		XMMATRIX currentParentTransformation = parentTransformation;
		XMVECTOR currentParentRotation = qOrientation;
		SkeletonSegment* newSegment = parentSegment;
		string name = baseName;
		for (int repeatIndex = 0; repeatIndex < repeat; ++repeatIndex)
		{
			const XMVECTOR offset = XMVectorSet(xOffset, yOffset, zOffset, 1);
			const XMVECTOR pitchYawRoll = XMVectorSet(xRot + xRotAll, yRot + yRotAll, zRot + zRotAll, 0);
			SkeletonSegment* const parentSegment = newSegment;
			const int doFCount = (xAvailable ? 1 : 0) + (yAvailable ? 1 : 0) + (zAvailable ? 1 : 0);

			const Connection connection = { offset, doFCount, xAvailable, yAvailable, zAvailable, pitchYawRoll};
			const XMVECTOR localRotationQuaternion = XMQuaternionRotationRollPitchYawFromVector(XMVectorSet(0, 0, 0, 0));
			const XMMATRIX localTranslationMatrix = XMMatrixTranslationFromVector(XMVectorSet(xOffset, length, zOffset, 0));
			const XMMATRIX localRotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMVectorSet(0, 0, 0, 0));
			const XMMATRIX localTransform = XMMatrixMultiply(localRotationMatrix, localTranslationMatrix);
			const XMMATRIX finalTransform = XMMatrixMultiply(localTransform, currentParentTransformation);
			const XMVECTOR finalQuaternionRotation = XMQuaternionMultiply(currentParentRotation, localRotationQuaternion);
			currentParentRotation = finalQuaternionRotation;
			currentParentTransformation = finalTransform;
			const XMVECTOR createLocation = XMVector3Transform(XMVectorZero(), finalTransform);
			TestChainLink* const link = new TestChainLink(ObjectParameters(sk->GetParentWorld()), length,
				MoveableParameters(XMVectorZero(), finalQuaternionRotation));

			name = repeatIndex == 0 ? baseName : ((repeatIndex == repeat - 1) ? endName : baseName + to_string(repeatIndex));
			newSegment = new SkeletonSegment(skeleton,
				parentSegment,
				link->length,
				isGoal && (repeatIndex == repeat - 1),
				isFootGoal && (repeatIndex == repeat - 1),
				XMFLOAT4(minX, minY, minZ, 0),
				XMFLOAT4(maxX, maxY, maxZ, 0),
				name,
				baseName + to_string(repeatIndex),
				connection,
				parentName);
			//this->theFirstChainLink = link;
			if (!name.empty())
				sk->skeletonSegmentMap[name] = newSegment;
			isRoot = false;
			xOffset = 0; yOffset = 0; zOffset = 0; xRot = 0; yRot = 0; zRot = 0;
			//name = // +to_string(repeatIndex + 1);
			parentName = "";
		}

		if (children != nullptr)
		{
			for (auto seg : *children)
			{
				parseSegment(seg,
					skeleton,
					currentParentRotation,
					newSegment,
					sk,
					currentParentTransformation);
			}
		}
	}

	/*void parseConnections(nlohmann::json& j, Skeleton* const skeleton, TestUserOfSkeleton* const testUserOfSkeleton)
	{
		using namespace nlohmann;
		using namespace std;
		SkeletonSegment* segment1;
		SkeletonSegment* segment2;
		SkeletonSegment* root;

		for (json::iterator it = j.begin(); it != j.end(); ++it)
		{
			if (it.key() == "segment1")
			{
				segment1 = testUserOfSkeleton->skeletonSegmentMap.at(*it);
			}
			if (it.key() == "segment2")
			{
				segment2 = testUserOfSkeleton->skeletonSegmentMap.at(*it);
			}
			if (it.key() == "root")
			{
				root = testUserOfSkeleton->skeletonSegmentMap.at(*it);
			}
		}

		//segment1->parent = segment2;
		//segment2->childSegments.push_back(segment1);

		//segment1->childSegments.push_back(segment2);
	}*/

	void TestUserOfSkeleton::Act()
	{
		using namespace std;
		const Actor* const target = static_cast<Actor*>(static_cast<const PhysicalWorld* const>(this->parentWorld)->GetObjectByGlobalID(this->targetObjectID));
		if (target == nullptr)
		{
			return;
		}
		static const float maxAngle = .05f;
		static const float turnSpeedFactor = .1f;
		float ang = 0;

		this->destination = XMVectorSet(0, -50, 100, 0);
		XMVECTOR offsetDest = XMVectorSubtract(target->location, this->location);
		offsetDest = XMVectorSetW(offsetDest, 1);
		offsetDest = XMVectorAdd(offsetDest, XMVectorSet(0, 20, 0, 0));
		this->destination = XMVectorAdd(XMVectorMultiply(XMVectorReplicate(ikDistance), target->GetFacing()), offsetDest);
		this->skeleton->SetDestination(this->destination);
		if (fireNext && frameCount++ % 1 == 0 && frameCount > 100 && pauseCount <= 0)
		{
			this->skeleton->Update();
		}
		else if (pauseCount > 0)
		{
			--pauseCount;
		}

		this->brain.Update();
		__super::Act();
	}

	void TestUserOfSkeleton::TrackTarget(const int targetObjectId)
	{
		this->targetObjectID = targetObjectId;
	}

	void TestUserOfSkeleton::initializeCollisionHulls()
	{
		for (int a = 0; a < this->skeleton->GetSegments().size(); ++a)
		{
			const SkeletonSegment* const segment = this->skeleton->GetSegments()[a];
			CollisionHull* const newHull = new CollisionHull();
			newHull->minBounds = XMVectorSet(-2.5f, 0, -2.5f, 0);
			newHull->maxBounds = XMVectorSet(2.5f, segment->GetLength(), 2.5f, 0);
			newHull->useTransformation = true;
			this->collisionHulls.push_back(newHull);
		}
	}

	void TestUserOfSkeleton::updateCollisionHulls()
	{
		for (int a = 0; a < this->skeleton->GetSegments().size(); ++a)
		{
			const SkeletonSegment* const segment = this->skeleton->GetSegments()[a];
			CollisionHull* const hull = this->collisionHulls[a];
			hull->transformationMatrix = segment->GetFullStartTransformation();
		}
	}

	TestUserOfSkeleton::~TestUserOfSkeleton()
	{
		if (this->skeleton != nullptr)
		{
			delete this->skeleton;
		}
	}
}
