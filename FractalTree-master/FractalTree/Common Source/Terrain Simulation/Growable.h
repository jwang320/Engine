#pragma once

namespace WorldGrowthSimulation
{
	class GrowthSimulation;

	//interface for object growable in growth simulation
	class Growable
	{
	private:
		unsigned int localGrowableId;
		bool removed = false;

	protected:
		GrowthSimulation* const growthSimulation;

	public:
		Growable(GrowthSimulation* const growthSimulation);
		virtual bool Grow() = 0;

	protected:
		void removeThisGrowable();

	public:
		void SetLocalGrowableId(const unsigned int localId) { this->localGrowableId = localId; }
		unsigned int GetLocalGrowableId() const { return this->localGrowableId; }
	};
}