#pragma once

namespace SimulationEngine
{
	class TestSkeletonArmSegment : public Actor
	{
	public:
		const float length = 50;

	public:
		TestSkeletonArmSegment(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());
	};
}