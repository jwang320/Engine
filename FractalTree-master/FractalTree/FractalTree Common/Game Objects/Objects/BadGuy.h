#pragma once
#define DO_STUFFa
namespace SimulationEngine
{
	class DecentMissileLauncher;

	class BadGuy : public Actor
	{

	public:
		enum ThrusterNames
		{
			BOTTOM_CENTER,
			BACK_CENTER,
			BACK_LEFT,
			BACK_RIGHT,
			FRONT_LEFT,
			FRONT_RIGHT,
			TOP_FRONT,
			TOP_BACK,
			BOTTOM_FRONT,
			BOTTOM_BACK,
			TOP_LEFT,
			TOP_RIGHT,
			BOTTOM_LEFT,
			BOTTOM_RIGHT,
			FRONT_CENTER,
			LEFT,
			RIGHT
		};

	protected:
		bool gunActive = false;
		bool ownsTheText = false;
		DecentMissileLauncher* decentMissileLauncher;
		int moveState = 0;
		int targetObjectID = -1;
		int updateNow = 0;
		unsigned int frameCount = 0;

	public:
		BadGuy(const ObjectParameters& objectParameters = ObjectParameters(),
			const MoveableParameters& moveableParameters = MoveableParameters());

		virtual void Act() override;
		void Hover(int targetID);
		void Bump();
		void TurnLeft(float degrees, int frames = 2);
		void TurnRight(float degrees, int frames = 2);
		void ApplyForceUp(float amount);
		void TrackTarget(int targetID);

	private:
		void initializeCollisionHull();
	};
}