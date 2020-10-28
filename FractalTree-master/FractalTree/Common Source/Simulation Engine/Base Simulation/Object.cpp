#include "pch.h"

namespace SimulationEngine
{
	Object::Object(const ObjectParameters& objectParameters) :
		parentWorld(objectParameters.parentWorld),
		localID(objectParameters.localID),
		globalID(objectParameters.globalID),
		persistentId(objectParameters.persistentId),
		isRemoved(false),
		componentFlags(0),
		threadHandleCount(0),
		deathCounter(100),
		objectType(ObjectTypes::NOT_AN_OBJECT)
	{
		if(this->parentWorld != NULL)
		{
			this->parentWorld->QueueAddObject(this);
		}
	}

	Object::Object(World* parentWorld, const char* data) :
		parentWorld(parentWorld)
	{
		memcpy(&this->globalID, data, sizeof(this->globalID));
	}

	void Object::Remove()
	{
		this->parentWorld->RemoveObject(this->globalID);
	}

	void Object::GetSerializedForm(char* data, int& dataLength, int maxLength) const
	{
		if(Object::SerializedObjectLength > maxLength)
		{
			return;
		}

		memcpy(data, &this->globalID, sizeof(int));
		memcpy(data + sizeof(int), &this->objectType, sizeof(this->objectType));
		dataLength = sizeof(this->globalID) + sizeof(this->objectType);
	}

	ObjectTypes Object::GetSerializedObjectType(const char* const data)
	{
		short t;
		memcpy(&t, data + sizeof(int), sizeof(short));
		return static_cast<ObjectTypes>(t);
	}

	int Object::GetSerializedObjectId(const char* const data)
	{
		int id;
		memcpy(&id, data, sizeof(id));
		return id;
	}

	void Object::GetSerializedFormSmall(char* data, int& dataLength, int maxLength) const
	{
		if(Object::SerializedObjectLengthSmall > maxLength)
		{
			return;
		}

		memcpy(data, &this->globalID, sizeof(int));
		dataLength = sizeof(this->globalID);
	}

	Object::~Object()
	{

	}
}
