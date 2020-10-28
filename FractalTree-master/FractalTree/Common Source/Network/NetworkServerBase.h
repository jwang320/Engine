#pragma once

#include <unordered_map>
#include "NetworkBase.h"

class UserDAO;

namespace Network
{
	class Client;
	class ServerPacketHolder;

	class NetworkServerBase : public NetworkBase
	{
	private:
		std::unordered_map<unsigned int, Client*> clients;
		std::unordered_map<std::string, unsigned int> clientIds;

	protected:
		UserDAO* const userDAO;
		ServerPacketHolder* const serverPacketHolder;

	public:
		NetworkServerBase(UserDAO* const userDAO, ServerPacketHolder* const serverPacketHolder) : userDAO(userDAO), serverPacketHolder(serverPacketHolder) {}
		void SendObjectUpdate(const char* const data, const int dataLength);
	protected:

		void handleConnectionRequest(const NetworkPacket* const packet);
		virtual std::string getClientIdentifier(const NetworkPacket* const packet) = 0;
		virtual void disconnectClient(const Client* client) {}
		void handleGamePacket(const NetworkPacket* const packet);
		Client* getClient(const NetworkPacket* const networkPacket);
		Client* getClient(const std::string& clientIdentifier);
		Client* getClient(const unsigned int clientId);
		void removeClient(const Client* const client);
	private:
		int getClientId(const std::string& accountName, bool& created);
		std::string getConnectionAccountName(const char* const packet);
		std::string getConnectionPassword(const char* const packet);
		void addClient(Client* client, const std::string& clientIdentifier);
	};
}