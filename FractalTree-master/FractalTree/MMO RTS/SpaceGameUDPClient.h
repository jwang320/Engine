#pragma once
#include "..\Common Source\Network\UDPClient.h"

namespace SpaceGameCore
{
	class SpaceGameSimulation;

	class SpaceGameUDPClient : public Network::UDPClient
	{
	private:
		SpaceGameSimulation* simulation = nullptr;

	public:
		void SendCommand(Network::ClientActionType actionType, const char* const const const commandData, int dataLength);

	protected:
		virtual void handlePackets() override;

	public:
		void setSimulation(SpaceGameSimulation* simulation) { this->simulation = simulation; }
	};
}