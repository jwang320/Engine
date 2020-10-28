#pragma once
#include <atomic>
#include "..\..\..\Common Source\Terrain Simulation\Growable.h"
class SpatialLSystem;

namespace WorldGrowthSimulation { class GrowthSimulation; }

namespace SimulationEngine
{
	class TreeActor : public Actor, WorldGrowthSimulation::Growable
	{
	protected:
		SpatialLSystem* const treeSystem;
		volatile unsigned long long updateVersion = 0;

	public:
		TreeActor(WorldGrowthSimulation::GrowthSimulation* const growthSimulation, const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters(),
			const ActorParameters& actorParameters = ActorParameters());

		virtual bool Grow() override;
		virtual void onRemove() override;
		~TreeActor();

		SpatialLSystem* GetTreeSystem() const {return this->treeSystem;}
		volatile unsigned long long GetUpdateVersion() const { return this->updateVersion; }
	};
}