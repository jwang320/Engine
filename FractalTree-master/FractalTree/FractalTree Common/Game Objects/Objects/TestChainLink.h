#pragma once
#include "..\Common Source\Simulation Engine\Physical Simulation\Actor.h"
#include "..\..\..\Common Source\Simulation Engine\Base Simulation\Object.h"

class InverseKinematics::SkeletonSegment;

namespace SimulationEngine
{
	class TestChainLink : public Actor
	{
	public:
		const float length = 50;
		const InverseKinematics::SkeletonSegment* attachedSegment = nullptr;

	public:
		TestChainLink(const ObjectParameters& objectParameters, const float length = 50,
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
	};
}