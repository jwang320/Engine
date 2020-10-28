#pragma once
#include <vector>
#include "NetworkServerBase.h"
#include "../../WebServerVexal/WebServerVexal/src/HttpServer/HttpServer.h"

namespace Network
{
	class TCPServer : public NetworkServerBase
	{
	private:
		std::vector<const NetworkClient*> clientsToClose;

	public:
		HttpServer httpServer;
		Logger log = Logger("TCPServer");
		TCPServer(UserDAO* const userDAO, ServerPacketHolder* const serverPacketHolder);
	protected:
		std::unordered_map<SOCKET, NetworkClient*> netClients;
		virtual void initializeSocket() override;
		virtual void handlePackets() override;
		virtual void receiveNewPackets() override;
		virtual void sendNewPackets() override;
		virtual std::string getClientIdentifier(const NetworkPacket* const packet) override;
		virtual void disconnectClient(const Client* client) override;

	private:
		void closeClientSocket(const SOCKET clientSocket, const std::string& clientAddressString);
		void closeClient(const SOCKET socket);
	};
}

