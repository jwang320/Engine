#pragma once
#include "Interfaces/Commandable.h"

namespace SimulationEngine
{
	class Actor;

	class Item : public Commandable 
	{
	private:
		int localId = -1;
		int globalId = -1;

	protected:
		Actor* owner;

	public:
		Item(Actor* owner = nullptr);
		virtual void use() {}
		int getGlobalId() { return this->globalId; }
		virtual ~Item();
	};
}