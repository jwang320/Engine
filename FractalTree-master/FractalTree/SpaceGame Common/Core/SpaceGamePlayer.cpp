#include "pch.h"
#include "SpaceGamePlayer.h"
#include "SpaceGameSimulation.h"

namespace SpaceGameCore 
{
	SpaceGamePlayer::SpaceGamePlayer(SpaceGameSimulation* simulation, const unsigned int newPlayerId) :
		Player(newPlayerId),
		rtsSimulation(simulation)
	{

	}
}
