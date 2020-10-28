#pragma once
#include "SimulationEvent.h"
#include "Object.h"

namespace SimulationEngine
{
	class World
	{
	private:
		//maps objects' local ID's to global ID's so that systems knowing only of global ID's can access data about objects in the world
		//the index is the global id, and the value is the local id.
		std::vector<int> globalIDMap;
		std::vector<Object*> removeObjectList; //objects queued to be removed at the end of the current simulation frame
		std::vector<Object*> addObjectList;    //objects queued to be added at the end of the current simulation frame
		std::vector<Object*> deleteQueue;

	protected:
		Object* dummyObject; // for multi-threading safety
		bool active;
		std::vector<WorldEvent> worldEventList;

	public:
		std::vector<Object*> objectList;
		//an object's local ID is its index in the objectList
		int nextAvailableLocalID;

		//an object's global ID is unique and used to refer to objects from other systems (example: over the network or from the renderer)
		int nextAvailableGlobalID;

		int deleteQueueObjectCount;

	public:
		void UpdateWorld();
		void QueueAddObject(Object* object);
		void QueueAddObject(Object* object, int globalID);
		void RemoveObject(int globalID);
		void RemoveAllObjects();
		void ExecuteWorldEvent(const WorldEvent& worldEvent);
		struct dl { const int len; char* const data; };
		void SerializeObjects(std::vector <dl> & resultObjectList, const unsigned int maxLength);
		bool IsObjectAlive(int globalID) const {return globalID >= 0 && this->globalIDMap[globalID] > -1;}
		bool DoesObjetExist(int globalID) const {return globalID >= 0 && this->globalIDMap[globalID] > -1;}//!= -1 && this->globalIDMap[globalID] != -5;}
		bool IsObjectInitializeQueued(int globalID) const { return globalID >= 0 && this->globalIDMap[globalID] == -5; }
		template <class ObjectType, class ReturnType> void GetObjectProperty(int globalID, ReturnType* resultValue) const;

		virtual ~World();

	protected:
		World();
		int GetObjectLocalID(int globalID) const;
		virtual void Update() = 0;
		void MarkObjectDead(int globalID) {this->SetGlobalIDMapping(globalID, -2);}
		virtual void AddObject(Object* object);
		virtual void AddObject(Object* object, int globalID);

	private:
		void DeleteRemovedObjects();
		void DeleteRemovedObjectsForReal();
		void AddQueuedObjects();
		void SetGlobalIDMapping(int globalID, int localID) {this->globalIDMap[globalID] = localID;}

	public:
#pragma region Getters and Setters
		int GetObjectGlobalID(int localID) const {return this->objectList[localID]->GetGlobalID();}
		int GetObjectData(int globalID) const {return 0;}
		int GetObjectCount() const {return this->nextAvailableLocalID;}
		Object* GetObjectByGlobalID(int globalID) const {return this->objectList[GetObjectLocalID(globalID)];}
		Object* GetObjectByLocalID(int localID) {return this->objectList[localID];}
		int GetNextAvailableLocalID() const {return this->nextAvailableLocalID;}
		int GetNextAvailableGlobalID() const {return this->nextAvailableGlobalID;}
		bool IsActive() const {return this->active;}
		const std::vector<WorldEvent>& GetWorldEventList() const {return this->worldEventList;}
		void SetActive(bool active) {this->active = active;}
#pragma endregion
	};

	inline int World::GetObjectLocalID(int globalID) const
	{
	//	assert(globalID >= 0);
		//assert(globalID < this->nextAvailableGlobalID);
	//	assert(this->objectList[this->globalIDMap[globalID]] != NULL);

		return this->globalIDMap[globalID];
	}
}