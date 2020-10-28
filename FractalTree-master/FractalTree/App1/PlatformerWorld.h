#include "../Simulation Engine Physical/PhysicalWorld.h"

class BlockUniverse;
class PlatformerCharacter;

//The World class is a container for all existing objects.  The World controls the natural
//interaction between them.
class PlatformerWorld : public SimulationEngine::PhysicalWorld
{
protected:
	BlockUniverse* blockUniverse; //Handles the pathing and stuff for platforming.

public:
	PlatformerWorld();
	//Update is called once per frame.
	virtual void Update() override;
	void SetBlockUniverse(BlockUniverse* blockUniverse) { this->blockUniverse = blockUniverse; }
	bool CheckPlatformerCollision(PlatformerCharacter* platformerCharacter);
	
private:
	void applyPathing();
	void writeObjectBlocks();
	void writeObjectBlock(const PlatformerCharacter* platformerObject);
};