#pragma once

namespace SimulationEngine
{
	class DecentMissile : public Actor
	{
	protected:
		int remainingFrames;
		bool dying;
		const bool killImmediately;
		const float force;

	public:
		DecentMissile(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters(), const bool killImmediately = false, const float force = 50000);

		virtual void Act() override;
		virtual void OnCollision(const CollisionData& collisionData) override;
	};
}