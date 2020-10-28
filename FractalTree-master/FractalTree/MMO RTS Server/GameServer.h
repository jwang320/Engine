#pragma once
#include "../Common Source/Network/ServerPacketHolder.h"

class UserDAO;
class EntityStateLoader;
namespace SpaceGameCore { class SpaceGameSimulation; }
namespace Network { class UDPServer; class TCPServer; }
namespace RTSServer
{
	class GameServer
	{
	private:
		SpaceGameCore::SpaceGameSimulation* simulation;
		Network::UDPServer* networkServer;
		Network::TCPServer* tcpServer;
		UserDAO* userDAO;
		EntityStateLoader* objectIdProvider;
		Network::ServerPacketHolder packetHolder;

	public:
		GameServer();
		void Initialize();

		void Run();

	private:
		void initializeSimulation();
		void initializeNetwork();
		void updateClients();
		void handleClientMessages();
	};
}