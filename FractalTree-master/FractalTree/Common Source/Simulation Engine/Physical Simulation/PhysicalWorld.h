#pragma once
#include <unordered_map>
#include "Actor.h"
#include "Constraint.h"
#include "AngleConstraint.h"
#include "ConstraintD.h"
#include "AngleConstraintD.h"
#include "Item.h"

namespace SimulationEngine
{
	class Effect;

	struct CollisionResult
	{
		DirectX::XMVECTOR collisionNormal;
		DirectX::XMVECTOR collisionPoint;
		float collisionDistance;
	};

	//All objects in a PhysicalWorld must derive from Actor.
	class PhysicalWorld : public World
	{
	public:
		enum ComponentFlags
		{
			ACT = 0x1,
			PATH = 0x2,
			PHYSICS = 0x4,
			MOVE = 0x8,
			PLATFORMER = 0x16
		};
		static const float rotAngX;
		static const float rotAngY;
		static const float rotAngZ;
		static const float moveY;
		Actor* testArm;

	protected:
		std::vector<Effect*> effectList;
		int nextAvailableLocalEffectID;
		int nextAvailableGlobalEffectID;
		std::vector<int> globalEffectIDMap;
		std::vector<Constraint> constraints;
		std::vector<ConstraintD> constraintsD;
		std::vector<AngleConstraint> angleConstraints;
		std::vector<AngleConstraintD> angleConstraintsD;
		float damping;

	private:
		std::vector<Effect*> removeEffectList;
		int totalConstraintIterations = 0;
		std::list<int> iterationCounts;
		unsigned long long frameCount = 0;
		std::unordered_map<int, Item*> items;

	public:
		PhysicalWorld();
		void AddEffect(Effect* effect);
		void RemoveEffect(Effect* effect);
		void ClearEffects();
		void SetEffectGlobalIDMapping(int globalEffectID, int localEffectID) {this->globalEffectIDMap[globalEffectID] = localEffectID;}
		bool IsObjectRegistered(int globalObjectID, unsigned int componentFlags) const;
		void AddConstraint(Actor* const object1, Actor* const object2);
		void AddAngleConstraint(Actor* const object1, Actor* const object2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2, const bool immutableObject2 = false);
		void AddConstraint(Actor* const object1, const DirectX::XMVECTOR& point);
		//void AddConstraint(Actor* const object1, Actor* const object2);
		void AddConstraint(Actor* const object1, const DirectX::XMVECTOR& point, const DirectX::XMVECTOR& offset1);
		void AddConstraint(Actor* const object1, Actor* const object2, const DirectX::XMVECTOR& offset1, const DirectX::XMVECTOR& offset2, const bool immutableObject2 = false);
		void QueryObjectsLocation(std::vector<Actor*>& results, const DirectX::XMVECTOR& location, float radiusSquared) const;
		void QueryObjectsLocation(std::vector<Actor*>& results, const DirectX::XMVECTOR& location, float radiusSquared, ObjectTypes objectType) const;
		void QueryObjectsLocation(std::vector<Actor*>& results, const DirectX::XMVECTOR& location, float radiusSquared, ObjectTypes* exclusionList, int exclusionCount, int excludedPlayer = -1) const;
		float AngleBetweenObjects(int objectIDA, int objectIDB) const;
		bool intersect(const float startX, const float startY, const float startZ,
			const float dirX, const float dirY, const float dirZ);
		Item* getItem(const int itemId) { return this->items.at(itemId); }
		void addItem(Item* const item) { this->items[item->getGlobalId()] = item; }
		~PhysicalWorld();

	protected:
		virtual void Update() override;
		Effect* GetEffect(int globalEffectID) {return this->effectList[this->GetEffectLocalID(globalEffectID)];}

	private:
		void ApplyEffects();
		void ApplyPhysics();
		void ApplyMovement();
		
	protected:
		//Private accessors to easily get world objects as Actor* 
		Actor* GetObject(int localID) const { return (localID >= 0) ? static_cast<Actor*>(this->objectList[localID]) : static_cast<Actor*>(this->dummyObject); }
		Actor* GetObjectGlobal(int globalID) const {return this->GetObject(this->GetObjectLocalID(globalID));}

	public:
		int GetEffectLocalID(int globalID) const {return this->globalEffectIDMap[globalID];}
		//Outside systems should only access object data indirectly via
		//global ID's to prevent inconsistencies with threading
#pragma region Get Actor Parameters by Global ID
		Actor::ObjectData GetObjectData(int globalID) const {return this->GetObjectGlobal(globalID)->GetObjectData();}
		DirectX::XMVECTOR GetActorFacing(int globalID) const {return this->GetObjectGlobal(globalID)->GetFacing();}
		DirectX::XMVECTOR GetActorViewDirection(int globalID) const {return this->GetObjectGlobal(globalID)->viewDirection;}
		int GetEffectCount() const {return this->nextAvailableLocalEffectID;}
		Actor* GetActor(const int globalId) const { return static_cast<Actor*>(this->GetObjectByGlobalID(globalId)); }
#pragma endregion

	public:
		enum RegisterProperties
		{
			Environment,
			Physics,
			Movement,
			BlockObject,
			Pathable
		};
	};
}