#pragma once

namespace SimulationEngine
{
	class TheSun : public Actor
	{
	private:
		float posAng;
	public:
		TheSun(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
	};
}