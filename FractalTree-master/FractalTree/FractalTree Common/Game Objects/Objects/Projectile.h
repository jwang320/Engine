#pragma once

namespace SimulationEngine
{
	class Projectile : public Actor
	{
	protected:
		int lifeTime;
		const DirectX::XMVECTOR constraintPosition;
		const DirectX::XMVECTOR originalConstraintDistance;

	public:
		Projectile(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
	};
}