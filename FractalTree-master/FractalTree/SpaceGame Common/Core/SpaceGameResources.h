#pragma once
#include "ResourceManager.h"

namespace SpaceGameCore
{
	class SpaceGameRenderer;
	class SpaceGameResources : public ResourceManager
	{
	public:
		static void LoadResources(SpaceGameRenderer* renderer);
		static void InitializeProgramVariables();
	};
}