#pragma once
#include <atomic>

namespace SimulationEngine
{
	class World;
	class ObjectComponent;

	struct ObjectParameters
	{
		World* parentWorld;
		int localID;
		int globalID;
		int persistentId;
		ObjectParameters(World* parentWorld = NULL, int localID = -1, int globalID = -1, int persistentId = -1) : parentWorld(parentWorld), localID(localID), globalID(globalID), persistentId(persistentId){}
	};

	//Interface denoting an object that can exist in a world.
	class Object
	{
		friend class World;
	private:
		//An object's local ID refers to its index in the world's list of objects
		int localID;

		//an object's global ID is unique and used to refer to objects from other systems (example: over the network)
		int globalID;

		int serializedLength;

		//the number of non-owner threads handling the object
		mutable std::atomic<int> threadHandleCount;
		int deathCounter;

	protected:
		World* parentWorld;
		bool isRemoved;
		unsigned int componentFlags;
		ObjectTypes objectType;

	public:
		static const int SerializedObjectLength = sizeof(int) + sizeof(ObjectTypes);
		static const int SerializedObjectLengthSmall = sizeof(int);

		int persistentId = -1;
		bool persist = true;
		_LARGE_INTEGER lastUpdated;
	public:
		void Remove();
		//called when removing this object.
		virtual void onRemove() {};
		virtual ~Object();

		//Override this function to convert the object into a
		//stream to send over a network or copy to disk.  Overrides
		//should be additive and not replace the parent class's call.
		virtual void GetSerializedForm(char* data, int& dataLength, int maxLength) const;
		virtual void GetSerializedFormSmall(char* data, int& dataLength, int maxLength) const;
		const int GetSerializedLength() const {return sizeof(int);}//return this->serializedLength;}
		void Release() const {--this->threadHandleCount;}
		void Lock() const {++this->threadHandleCount;}
		static ObjectTypes GetSerializedObjectType(const char* const data);
		static int GetSerializedObjectId(const char* const data);
	protected:
		Object(const ObjectParameters& objectParameters = ObjectParameters());

		//Serialized constructor
		Object(World* parentWorld, const char* data);

	public:
#pragma region Getters and Setters
		int GetLocalID() const {return this->localID;}
		int GetGlobalID() const {return this->globalID;}
		int GetThreadHandleCount() const {return this->threadHandleCount;}
		int GetDeathCounter() const { return this->deathCounter; }
		bool IsRemoved() const {return this->isRemoved;}
		unsigned int GetComponentFlags() const {return this->componentFlags;}
		ObjectTypes GetObjectType() const {return this->objectType;}
		World* GetParentWorld() const {return this->parentWorld;}

		void SetLocalID(int localID) {this->localID = localID;}
		void SetGlobalID(int globalID) {this->globalID = globalID;}
		void SetRemoved() {this->isRemoved = true;}
		void DecrementDeathCounter() { --this->deathCounter; }
#pragma endregion
	};
}