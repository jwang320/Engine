#include "pch.h"
#include "GameServer.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"

using namespace Network;
using namespace SpaceGameCore;
using namespace RTSServer;
GameServer gameServer;
int main()
{
	gameServer.Initialize();
	gameServer.Run();
	return 0;
}