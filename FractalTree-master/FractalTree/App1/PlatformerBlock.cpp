#include "pch.h"
#include "PlatformerBlock.h"
#include "BlockUniverse.h"

using namespace SimulationEngine;
using namespace DirectX;

PlatformerBlock::PlatformerBlock(BlockUniverse* parentBlockUniverse, int owningCharacter, int owningPlayer, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
	PlatformerCharacter(parentBlockUniverse, true, owningCharacter, owningPlayer, objectParameters, moveableParameters)
{
	this->objectType = ObjectTypes::PlatformerBlock;
	this->blockType = 3;
}