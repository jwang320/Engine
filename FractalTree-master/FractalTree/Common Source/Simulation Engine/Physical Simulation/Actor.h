#pragma once
#include "Interfaces/Moveable.h"
#include "Interfaces/Geometric.h"
#include "Interfaces/Commandable.h"
#include "Brain/Brain.h"

namespace SimulationEngine
{
	class Actor;
	class PhysicalWorld;

	struct ActorParameters
	{
		DirectX::XMVECTOR facing;
		ActorParameters(const DirectX::XMVECTOR& facing = DirectX::XMVectorSet(0, 0, 1, 0)) : facing(facing){}
	};

	struct CollisionData
	{
		Actor* other;
		DirectX::XMVECTOR collisionPoint;
		DirectX::XMVECTOR collisionNormal;
	};

	enum CollisionType
	{
		SPHERE,
		BOX,
		NONE
	};

	struct CollisionHull
	{
		std::vector<DirectX::XMVECTOR> vertices;
		std::vector<DirectX::XMVECTOR> planes;
		DirectX::XMVECTOR minBounds;
		DirectX::XMVECTOR maxBounds;
		DirectX::XMMATRIX transformationMatrix = DirectX::XMMatrixIdentity();
		bool useTransformation = false;
	};

	//Actor is the main game object from which other objects should extend.
	class Actor : public Object, public Moveable, public Geometric, public Commandable
	{
		friend class MotorCortex;
	protected:
		DirectX::XMVECTOR facing = DirectX::XMVectorSet(0, 0, 1, 0);
		bool dead = false;
		Brain brain = Brain(this);
		std::vector<CollisionHull*> collisionHulls;
		int owningPlayer = 0;
		DirectX::XMVECTOR previousAngularVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
		DirectX::XMVECTOR previousVelocity = DirectX::XMVectorSet(0, 0, 0, 0);
		PhysicalWorld* const world;

	public:
		bool collideMoveable = true;
		bool ignoreForceEffects = false;
		CollisionType collisionType = SPHERE;
		DirectX::XMVECTOR viewDirection = DirectX::XMVectorSet(0, 0, 0, 0);
		bool locked = false;
		Actor* owner = nullptr;
		bool axisAvailable[3] = { false }; //x,y,z
		bool ignorePhysics = false;
		int pauseCount = 0;
		DirectX::XMVECTOR ePoint;
		DirectX::XMVECTOR eDir;
		//loc, orient, vel, scale, rot vel, owning player
		static const int SerializedActorLength = Object::SerializedObjectLength + sizeof(DirectX::XMVECTOR) * 5 + sizeof(int);
		static const int SerializedActorLengthSmall = Object::SerializedObjectLengthSmall + sizeof(DirectX::XMVECTOR) * 5 + sizeof(int);

	public:
		Actor(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters(),
			const ActorParameters& actorParameters = ActorParameters(),
			int owningPlayer = 0);
		Actor(World* const parentWorld, const MoveableParameters& moveableParameters = MoveableParameters());

		//All existing Actors' (and sub-classes) Act() method is called once per frame.
		virtual void Act();
		virtual void OnCollision(const CollisionData& collisionData);
		virtual void TrackTarget(const int targetObjectId) {};
		virtual ~Actor();

		//GetSerializedForm returns the class as an array of bytes (usually for network usage).
		//This function should be overriden for sub-classes, but should still call the parent
		virtual void GetSerializedForm(char* data, int& dataLength, int maxLength) const override;
		virtual void GetSerializedFormSmall(char* data, int& dataLength, int maxLength) const override;
		const int GetSerializedLength() const;
		void SetFacing(const DirectX::XMVECTOR& facing) {this->facing = facing;}
		bool canPlayerInteract(const unsigned int playerId) const { return this->owningPlayer == playerId; }
	public:
		static void DeserializeObjectProperties(const char* data, DirectX::XMVECTOR& location, DirectX::XMVECTOR& scale, DirectX::XMVECTOR& pitchYawRoll, DirectX::XMVECTOR& facing, DirectX::XMVECTOR& velocity, int& owningPlayer);

		struct ObjectData
		{
			DirectX::XMVECTOR location;
			DirectX::XMVECTOR rollPitchYaw;
			DirectX::XMVECTOR scale;
			DirectX::XMVECTOR facing;
			DirectX::XMVECTOR viewDirection;
			const Actor* object;
			ObjectTypes objectType;
		};

		ObjectData GetObjectData() const { return { this->location, this->qOrientation, this->scale, this->facing, this->viewDirection, this, this->objectType }; }
		bool IsDead() const {return this->dead;}
		DirectX::XMVECTOR GetFacing() const
		{
			return DirectX::XMVector3Normalize(DirectX::XMVector3Rotate(this->facing, this->qOrientation));
		}
		const std::vector<CollisionHull*>& GetCollisionHulls() const {return this->collisionHulls;}
		bool IgnoreForceEffects() const {return this->ignoreForceEffects;}
		int GetOwningPlayer() const { return this->owningPlayer; }
		void setOwningPlayer(const unsigned int playerId) { this->owningPlayer = playerId; }
		void SetOwner(Actor* const owner) { this->owner = owner; }
	};
}