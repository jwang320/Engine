#pragma once
#include <vector>
#include <mutex>

class LSystem;
struct BranchParameters
{
	const unsigned short newMaxDepth;
	const float zRotation; //degrees
	const float yRotation;
	const float newLength;
	const float newRadius;

	BranchParameters(unsigned short newMaxDepth = 1, float zRotation = 0, float yRotation = 0, float newLength = 1, float newRadius = 1) :
		newMaxDepth(newMaxDepth),
		zRotation(zRotation),
		yRotation(yRotation),
		newLength(newLength),
		newRadius(newRadius)
	{

	}
};

class LSystemSegment
{
protected:
	friend class LSystem;
	friend class SpatialLSystem;

	const unsigned short currentDepth;
	const unsigned short maxDepth;
	const unsigned short segmentType;
	std::vector<LSystemSegment*> neighbors;
	LSystemSegment* parent;
	LSystem* const parentSystem;
	int localSegmentID;
	int globalSegmentID;
	bool needsIteration;
	float growFactor = 1.0f;
	bool isRoot = false;

public:
	LSystemSegment(LSystem* parentSystem, LSystemSegment* parent, unsigned short segmentType, unsigned short maxDepth, unsigned short currentDepth = 0);
	virtual bool AddChildIteration() = 0;
	virtual void UpdateOther() {};
	bool Update();

public:
	const std::vector<LSystemSegment*>& GetNeighbors() const {return this->neighbors;}
	unsigned short GetCurrentDepth() const {return this->currentDepth;}
	unsigned short GetMaxDepth() const {return this->maxDepth;}
	LSystemSegment* GetParent() const {return this->parent;}
	bool IsRoot() const { return this->isRoot; }
	void SetIsRoot(const bool isRoot) { this->isRoot = isRoot;}
	LSystem* GetParentSystem() const { return this->parentSystem; }
	unsigned short GetSegmentType() const {return this->segmentType;}
	int GetLocalSegmentID() const {return this->localSegmentID;}

	void SetLocalSegmentID(const int id) { this->localSegmentID = id; }
	void SetGrowFactor(const float g) { this->growFactor = g; }
	virtual ~LSystemSegment();
};

namespace WorldGrowthSimulation { class TerrainResources; }
namespace SimulationEngine { class Actor; }

class LSystem
{
protected:
	friend class LSystemSegment;
	friend class TreeTrunkSegment;
	std::vector<LSystemSegment*> segmentList;
	LSystemSegment* rootSegment;
	std::mutex modificationMutex;
	WorldGrowthSimulation::TerrainResources* const terrainResources;
	const SimulationEngine::Actor* const actor;
	int currentBranches = 0;
	int maxBranches = 3;
	unsigned int leadNodes = 0;
	unsigned int rootNodeCount = 0;

public:
	LSystem(WorldGrowthSimulation::TerrainResources* const terrainResources, const SimulationEngine::Actor* const actor = nullptr) : actor(actor), terrainResources(terrainResources) {}
	~LSystem();
	inline void AddSegment(LSystemSegment* segment);
	bool UpdateLSystem();
	void SetRootSegment(LSystemSegment* rootSegment) { this->rootSegment = rootSegment; rootSegment->SetIsRoot(true); }
	void Lock() { this->modificationMutex.lock(); }
	bool TryLock() { return this->modificationMutex.try_lock(); }
	void Unlock() { this->modificationMutex.unlock(); }

public:
	std::vector<LSystemSegment*> getSegmentList() const { return this->segmentList; }
	WorldGrowthSimulation::TerrainResources* getTerrainResources() const { return this->terrainResources; }
	const SimulationEngine::Actor* getActor() const { return this->actor; }
	unsigned int GetRootNodeCount() const { return this->rootNodeCount; }
};

inline void LSystem::AddSegment(LSystemSegment* segment)
{
	this->segmentList.push_back(segment);
	this->segmentList[this->segmentList.size() - 1]->SetLocalSegmentID(static_cast<int>(this->segmentList.size() - 1));
}