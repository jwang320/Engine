#pragma once
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorld.h"

namespace SimulationEngine
{
	class Terrain;
	class Actor;

	class FractalTreeWorld : public PhysicalWorld
	{
	protected:
		Terrain* terrain;

	public:
		FractalTreeWorld();
		virtual void Update() override;

	public:
		float GetHeight(float x, float z) const;
		void SetTerrain(Terrain* terrain) {this->terrain = terrain;}
		Terrain* GetTerrain() {return this->terrain;}

	private:
		void ApplyPathing();
		bool CheckCollision(CollisionResult& collisionResult, const Actor* const objectA, const Actor* const objectB) const;
	};
}