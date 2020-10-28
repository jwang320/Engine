#pragma once
#include <queue>
#include "Network\UDPNetworkBase.h"


namespace Network
{
	class Client;

	class UDPServer : public UDPNetworkBase
	{
	private:
		std::map<std::string, Client*> clients;

	protected:
		std::string port;

	public:
		UDPServer(const std::string& port);
		void Update();
		int SendPacketToClients(const char* data, int dataLength) const;
		Client* CreateClient(const Packet& receivedPacket);
		//returns NULL if no client exists with the address
		Client* getClient(const std::string& clientIPAddress);
	protected:
		virtual void initialize() override;
		virtual bool initializeServerSocket() override;

		virtual bool handlePacket(const Packet& packet) override;

	private:
		void AddClient(Client* client);
	};
}