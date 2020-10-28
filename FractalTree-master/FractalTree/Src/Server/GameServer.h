#pragma once
#include "Network\UDPNetworkBase.h"

namespace SimulationEngine
{
	class FractalTreeSimulation;
}

namespace Network
{
	class UDPServer;
}

namespace Server
{

	class GameServer
	{
	protected:
		Network::UDPServer* networkServer;
		SimulationEngine::FractalTreeSimulation* simulation;
		int lastSentEvent;

	public:
		GameServer(Network::UDPServer* networkServer = NULL);
		void Update();

		void Initialize();
		void SetNetworkServer(Network::UDPServer* networkServer) {this->networkServer = networkServer;}

	private:
		void sendObjectData();
		void sendEventData();
		void handleProgramMessage(const Network::Packet& packet);
		void handleObjectRequest(const Network::Packet& packet);
	};
}