#include "pch.h"
#include "MildlyDifficultOpponent.h"
#include "PlatformerMissile.h"

using namespace SimulationEngine;
using namespace DirectX;

MildlyDifficultOpponent::MildlyDifficultOpponent(BlockUniverse* parentBlockUniverse, int owningCharacter, int owningPlayer, const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters, PlatformerCharacter* player) :
	PlatformerCharacter(parentBlockUniverse, false, owningCharacter, owningPlayer, objectParameters, moveableParameters),
	player(player),
	playerID(player->GetGlobalID()),
	coolDown(0)
{
	this->objectType = ObjectTypes::PlatformerCharacter;

	Thruster* leftThruster = new Thruster();
	Thruster* rightThruster = new Thruster();
	Thruster* bottomThruster = new Thruster;
	leftThruster->direction = XMVectorSet(1, 0, 0, 0);
	rightThruster->direction = XMVectorSet(-1, 0, 0, 0);
	bottomThruster->direction = XMVectorSet(0, 1, 0, 0);

	leftThruster->requireResting = false;
	rightThruster->requireResting = false;
	bottomThruster->requireResting = false;

	this->AddThruster(leftThruster);
	this->AddThruster(rightThruster);
	this->AddThruster(bottomThruster);

	const float speed = .25f;
	this->thrusters[0]->currentMagnitude = speed * this->mass;
	this->thrusters[1]->currentMagnitude = speed * this->mass;
	this->thrusters[2]->currentMagnitude = speed * this->mass;

	this->blockList.clear();
	this->blockList.shrink_to_fit();

	this->loadBlockModel("MildlyDifficultOpponent.m");
}