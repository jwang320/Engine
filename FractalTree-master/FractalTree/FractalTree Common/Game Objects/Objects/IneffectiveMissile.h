#pragma once

namespace SimulationEngine
{
	class IneffectiveMissile : public Actor
	{
	private:
		int age;

	public:
		IneffectiveMissile(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
		virtual void OnCollision(const CollisionData& collisionData) override;
	};
}