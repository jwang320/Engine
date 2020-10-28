#pragma once
#include "FractalTreeWorld.h"
#include "..\Common Source\Simulation Engine\Physical Simulation\PhysicalWorldSimulation.h"

namespace SimulationEngine
{
	class BadGuy;
	class FractalTreeWorld;
	class Terrain;
	class TestChainLink;

	class FractalTreeSimulation : public PhysicalWorldSimulation
	{
	protected:
		int cellWidth;
		float cellGridWidth;
		Actor* floor;
		Actor* theShip;
		BadGuy* testBadGuy;
		int localPlayerID;
		Terrain* terrain = nullptr;

	public:
		Actor* theFirstChainLink;
		TestChainLink* firstChainLink2;
		FractalTreeSimulation();
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void ResetWorld() override;
		void InitializeWorld(bool generateTerrain = true, bool generateAnything = true);
		void InitializeLocalPlayer(int playerID = -1);
		void ClearWorld();
		void ApplyStupidForce();
		void TestFoward();
		void TestBump();
		void TestLeft();
		void EnterWorld(bool generateTerrain = true);
		FractalTreeWorld* GetWorld() const {return static_cast<FractalTreeWorld*>(this->world);}
		Terrain* GetTerrain() const { return this->terrain; }
	};
}