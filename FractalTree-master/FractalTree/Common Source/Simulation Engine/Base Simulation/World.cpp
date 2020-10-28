#include "pch.h"
#include <assert.h>

namespace SimulationEngine
{
	World::World() :
		nextAvailableLocalID(0),
		nextAvailableGlobalID(0),
		active(true),
		deleteQueueObjectCount(0)
	{
		this->globalIDMap.resize(100000, -1);
		this->objectList.reserve(20000);
	}

	//Called once per frame
	void World::UpdateWorld()
	{
		this->DeleteRemovedObjects();
		//Check if it's safe to delete objects referenced by other threads.
		this->DeleteRemovedObjectsForReal();

		this->AddQueuedObjects();
		this->Update();
	}

	//This overload must ONLY be called by objects on SERVERS to ensure consistency
	//with global object ID's.
	void World::AddObject(Object* object)
	{
		assert(this->globalIDMap[this->nextAvailableGlobalID] < 0);
		if(this->nextAvailableLocalID < this->objectList.size())
		{
			this->objectList[this->nextAvailableLocalID] = object;
		}
		else
		{
			this->objectList.push_back(object);
		}

		object->SetLocalID(this->nextAvailableLocalID);
		object->SetGlobalID(this->nextAvailableGlobalID);
		this->SetGlobalIDMapping(this->nextAvailableGlobalID, this->nextAvailableLocalID);
		++this->nextAvailableLocalID;
		++this->nextAvailableGlobalID;
	}

	//This overload must ONLY be called by objects on SERVERS to ensure consistency
	//with global object ID's.
	void World::AddObject(Object* object, int globalID)
	{
		assert(globalID >= 0);
		assert(this->globalIDMap[globalID] < 0);

		if(this->nextAvailableLocalID < this->objectList.size())
		{
			this->objectList[this->nextAvailableLocalID] = object;
		}
		else
		{
			this->objectList.push_back(object);
		}

		object->SetLocalID(this->nextAvailableLocalID);
		this->SetGlobalIDMapping(globalID, this->nextAvailableLocalID);
		++this->nextAvailableLocalID;
	}

	void World::QueueAddObject(Object* object)
	{
		int objectID = object->GetGlobalID();
		if(objectID < 0)
		{
			object->SetGlobalID(this->nextAvailableGlobalID);
			objectID = this->nextAvailableGlobalID;
			++this->nextAvailableGlobalID;
		}
		else if(this->globalIDMap[objectID] != -1 && this->globalIDMap[objectID] != -2) //already exists;
		{
			return;
		}

		this->globalIDMap[objectID] = OBJECT_QUEUED_ADD;
		this->addObjectList.push_back(object);
	}

	void World::QueueAddObject(Object* object, int globalID)
	{
		this->addObjectList.push_back(object);
	}

	void World::RemoveObject(int globalID)
	{
		if(this->IsObjectAlive(globalID) && !this->GetObjectByGlobalID(globalID)->IsRemoved())
		{
			Object* const object = this->GetObjectByGlobalID(globalID);
			object->onRemove();
			this->removeObjectList.push_back(object);
			object->SetRemoved();
		}
	}

	void World::RemoveAllObjects()
	{
		for(int a = 0; a < this->GetObjectCount(); ++a)
		{
			if (this->objectList[a] != nullptr)
				this->RemoveObject(this->objectList[a]->GetGlobalID());
		}
	}

	void World::ExecuteWorldEvent(const WorldEvent& worldEvent)
	{
		this->worldEventList.push_back(worldEvent);

		switch (worldEvent.eventType)
		{
		case WorldEventType::ObjectDeath:
			this->RemoveObject(worldEvent.objectID);
			break;
		default:
			break;
		}
	}

	void World::AddQueuedObjects()
	{
		for(unsigned int a = 0; a < this->addObjectList.size(); ++a)
		{
			this->AddObject(this->addObjectList[a], this->addObjectList[a]->GetGlobalID());
		}

		this->addObjectList.clear();
	}

	void World::DeleteRemovedObjects()
	{
		for(unsigned int a = 0; a < this->removeObjectList.size(); ++a)
		{
			if(this->nextAvailableLocalID > 1)
			{
				if(this->nextAvailableLocalID - 1 != this->removeObjectList[a]->GetLocalID())
				{
					const int localID = this->removeObjectList[a]->GetLocalID();
					this->objectList[localID] = this->objectList[this->nextAvailableLocalID - 1];
					this->objectList[localID]->SetLocalID(localID);
					this->SetGlobalIDMapping(this->objectList[localID]->GetGlobalID(), localID);
				}

				--this->nextAvailableLocalID;
				this->objectList[this->nextAvailableLocalID] = this->dummyObject;
			}
			else
			{
				this->objectList[this->removeObjectList[a]->GetLocalID()] = this->dummyObject;
				--this->nextAvailableLocalID;
			}

			const WorldEvent newEvent = {WorldEventType::ObjectDeath, this->removeObjectList[a]->GetGlobalID(), static_cast<int>(this->worldEventList.size())};
			this->worldEventList.push_back(newEvent);
			this->MarkObjectDead(this->removeObjectList[a]->GetGlobalID());

			//Check to see if another thread currently has a pointer to this object before deleting.
			//If so, add the object to a list of objects that are waiting for other threads to let go.
			if(this->removeObjectList[a]->GetThreadHandleCount() > 0)
			{
				if (this->deleteQueue.size() > this->deleteQueueObjectCount)
				{
					this->deleteQueue[this->deleteQueueObjectCount] = this->removeObjectList[a];
				}
				else
				{
					this->deleteQueue.push_back(this->removeObjectList[a]);
				}
				
				++this->deleteQueueObjectCount;
			}
			else
			{
				delete this->removeObjectList[a];
				//forgot to do something with deleteQueue?
			}
		}

		if (this->removeObjectList.size() > 0)
		{
			this->removeObjectList.clear();
			this->removeObjectList.shrink_to_fit();
		}
	}

	void World::DeleteRemovedObjectsForReal()
	{
		for (int a = 0; a < this->deleteQueueObjectCount; ++a)
		{
			if (this->deleteQueue[a]->GetThreadHandleCount() == 0 && this->deleteQueue[a]->GetDeathCounter() < 0)
			{
				delete this->deleteQueue[a];
				this->deleteQueue[a] = NULL;

				if (this->deleteQueueObjectCount > a)
				{
					this->deleteQueue[a] = this->deleteQueue[this->deleteQueueObjectCount - 1];
					this->deleteQueue[this->deleteQueueObjectCount - 1] = NULL;
					--a;
				}

				--this->deleteQueueObjectCount;
			}
			else
			{
				this->deleteQueue[a]->DecrementDeathCounter();
			}
		}

		if (this->deleteQueue.size() > 1000 && this->deleteQueueObjectCount == 0)
		{
			this->deleteQueue.clear();
			this->deleteQueue.shrink_to_fit();
		}
	}

	void World::SerializeObjects(std::vector<dl>& resultObjectList, const unsigned int maxLength)
	{
		for (const Object const * const object : this->objectList)
		{
			if (object == nullptr)
				continue;

			char* const serializedData = new char[maxLength];
			int dataLength;
			object->GetSerializedForm(serializedData, dataLength, maxLength);
			resultObjectList.push_back({ dataLength, serializedData });
		}
	}

	World::~World()
	{
		for(int a = 0; a < this->GetObjectCount(); ++a)
		{
			if (this->objectList[a] != nullptr)
				delete this->objectList[a];
		}
	}
}