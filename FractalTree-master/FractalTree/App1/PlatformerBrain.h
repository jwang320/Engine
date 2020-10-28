#pragma once
#include <DirectXMath.h>

class PlatformerCharacter;



class PlatformerBrain
{
public:
	enum State
	{
		IDLE,
		FOLLOW,
		SEARCH,
		ENGAGE
	};

	enum Action
	{
		RETURN,
		STAY,
		CHASE
	};

	struct AIBrainParameters
	{
		float engageMinDistance;
		float searchRadius;
		float searchHeight;
		float weaponFireMaxDistance;
		Action outOfRangeAction;
		AIBrainParameters(float engageMinDistance = 400, float searchRadius = 50, float searchHeight = 200, float weaponFireMaxDistance = 600, Action outOfRangeAction = STAY) :
			engageMinDistance(engageMinDistance), searchRadius(searchRadius), searchHeight(searchHeight), weaponFireMaxDistance(weaponFireMaxDistance), outOfRangeAction(outOfRangeAction)
		{
		}
	};
protected:
	//The body this brain is contained in.
	PlatformerCharacter* body;
	State currentState;
	int targetObjectID;
	DirectX::XMVECTOR targetLocation;
	DirectX::XMVECTOR targetLocationMinDistance;
	DirectX::XMVECTOR targetLocationMaxDistance;
	float engageMinDistance;
	float searchRadius;
	float searchHeight;
	float weaponFireMaxDistance;
	Action outOfRangeAction;

public:
	PlatformerBrain(PlatformerCharacter* body, const AIBrainParameters& parameters = AIBrainParameters());

	//The brain update code should go in here
	void Think();
	void SetCurrentState(State newState);

	void SetTargetLocation(const DirectX::XMVECTOR& targetLocation) { this->targetLocation = targetLocation; }
	void SetSearchRadius(float searchRadius) { this->searchRadius = searchRadius; }

private:
	void FollowTarget();
	void BeIdle();
	void ExecuteSearch();
	void ExecuteFollow();
	void ExecuteEngage();
};