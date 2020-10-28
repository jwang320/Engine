#pragma once

namespace SimulationEngine
{
	class PhysicalWorld;

	struct EffectParameters
	{
		PhysicalWorld* parentWorld;
		bool instant;

		EffectParameters(PhysicalWorld* parentWorld, bool instant = false) :
			parentWorld(parentWorld), instant(instant)
		{}
	};

	class Effect
	{
	protected:
		PhysicalWorld* parentWorld;
		bool instant;

	private:
		int localEffectID;
		int globalEffectID;

	public:
		Effect(const EffectParameters& effectParameters);
		virtual void Apply() = 0;

		virtual ~Effect();

		void SetLocalID(int localID) {this->localEffectID = localID;}
		void SetGlobalID(int globalID) {this->globalEffectID = globalID;}
		void SetInstant(bool instant) {this->instant = instant;}
		int GetLocalID() const {return this->localEffectID;}
		int GetGlobalID() const {return this->globalEffectID;}
		bool IsInstant() const {return this->instant;}
	};
}