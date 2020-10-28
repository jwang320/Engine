#pragma once

namespace SimulationEngine
{
	class Player
	{
	private:
		int localID;
		int globalID;
		int focusObjectID;

	public:
		Player();
		Player(const int globalId) : globalID(globalId) {}
		virtual ~Player() {}
#pragma region Getters and Setters
		int GetGlobalID() const {return this->globalID;}
		int GetFocusObject() const {return this->focusObjectID;}

		void SetGlobalID(int globalID) {this->globalID = globalID;}
		void SetLocalID(int localID) {this->localID = localID;}
		void SetFocusObject(int focusObjectGlobalID) {this->focusObjectID = focusObjectGlobalID;}
#pragma endregion
	};
}