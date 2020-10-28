#include "pch.h"
#include "SpaceGameResources.h"
#ifndef _NO_GRAPHICS
#include "../MMO RTS/SpaceGameRenderer.h"
#endif
namespace SpaceGameCore
{
	void SpaceGameResources::InitializeProgramVariables()
	{
		__super::InitializeProgramVariables();
	}

	void SpaceGameResources::LoadResources(SpaceGameRenderer* renderer)
	{
		//BaseResourceDirectory = "../";
		SpaceGameResources::InitializeProgramVariables();
		//RTSResources::LoadProgramVariables("../Resources/ProgramVariables.txt");
#ifndef _NO_GRAPHICS
		ResourceManager::LoadResourceList(renderer, "Resources/ResourceList.rl");
#endif
	}
}