#include "pch.h"
#include "RTSPlayer.h"
#include "SpaceGameSimulation.h"
#include "RTSUnit.h"

namespace RTSCore 
{
	RTSPlayer::RTSPlayer(SpaceGameSimulation* simulation, const unsigned int newPlayerId) :
		Player(newPlayerId),
		rtsSimulation(simulation)
	{

	}
	
	void RTSPlayer::SelectUnit(int globalUnitId)
	{
		RTSUnit* const unit = this->rtsSimulation->GetUnit(globalUnitId);
		if (unit == nullptr)
		{
			return;
		}

		unit->SetSelected(!unit->Selected());

		if (!unit->Selected())
		{
			this->selectedUnitsIds.erase(globalUnitId);
		}
		else
		{
			this->selectedUnitsIds.insert(globalUnitId);
		}
	}

	void RTSPlayer::ReverseSelection(int globalUnitId)
	{
		RTSUnit* const unit = this->rtsSimulation->GetUnit(globalUnitId);
		if (unit == nullptr)
		{
			return;
		}

		if (this->selectedUnitsIds.find(globalUnitId) != this->selectedUnitsIds.end())
		{
			this->selectedUnitsIds.erase(globalUnitId);
			unit->SetSelected(false);
		}
		else
		{
			this->selectedUnitsIds.insert(globalUnitId);
			unit->SetSelected(true);
		}
	}
}
