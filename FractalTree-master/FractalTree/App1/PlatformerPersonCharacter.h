#include "PlatformerCharacter.h"

class PlatformerPersonCharacter : public PlatformerCharacter
{
protected:
	float gunFacingAngle;
	float mouseX;
	float mouseY;

public:
	PlatformerPersonCharacter(BlockUniverse* parentBlockUniverse, int owningCharacter = -1, int owningPlayer = 0, const SimulationEngine::ObjectParameters& objectParameters = SimulationEngine::ObjectParameters(),
		const SimulationEngine::MoveableParameters& moveableParameters = SimulationEngine::MoveableParameters());

	virtual void ReceiveCommand(const SimulationEngine::Command& command) override;

	float GetGunFacingDirection() const { return this->gunFacingAngle; }
};