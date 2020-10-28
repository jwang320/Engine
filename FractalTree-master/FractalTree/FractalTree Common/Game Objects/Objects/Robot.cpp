#include "pch.h"
#include "Robot.h"

using namespace SimulationEngine;

Robot::Robot(const ObjectParameters & objectParameters, const MoveableParameters & moveableParameters) :
	Actor(objectParameters, moveableParameters)
{
}

void Robot::Act()
{
}
