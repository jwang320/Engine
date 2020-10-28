#include "PlatformerCharacter.h"

class PlatformerMissile : public PlatformerCharacter
{
protected:
	int shooterID;
	float explosionRadius;
	float damage;

public:
	PlatformerMissile(BlockUniverse* parentBlockUniverse, int shooterID, int owningCharacter = -1, int owningPlayer = 0, int projectileLevel = 0, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

	virtual void OnBlockCollision(const BlockDefinition& blockDefinition, const DirectX::XMVECTOR& collisionLocation) override;
	int GetShooterID() const { return this->shooterID; }
	virtual void Act() override;
private:
	void explode(const DirectX::XMVECTOR& collisionLocation, int count = 10);
};