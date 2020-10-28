#include "pch.h"
#include "NetworkServerBase.h"

#include <iostream>
#include <string>
#include "NetworkInitializationException.h"
#include "Client.h"
#include "..\DAO\UserDAO.h"
#include "ServerPacketHolder.h"

using namespace std;

namespace Network
{
	void NetworkServerBase::handleConnectionRequest(const NetworkPacket* const packet)
	{
		cout << "Received connection request ";
		const string clientAddressString(inet_ntoa(packet->sender.sin_addr));


		const string accountName = this->getConnectionAccountName(packet->data);
		const string password = this->getConnectionPassword(packet->data);
		cout << "from account '" << accountName << "' password '" << password << "'" << endl;
		bool createdAccount = false;
		const int clientGlobalId = this->getClientId(accountName, createdAccount);
		if (clientGlobalId < 0)
		{
			return;
		}

		const bool validAccount = this->userDAO->validateAccount(accountName, password);
		if (!validAccount)
		{
			cout << "Connection request for account '" << accountName << "' password invalid." << endl;
			return;
		}
		Client* client = this->getClient(clientGlobalId);

		//New connection
		if (client == nullptr)
		{
			client = new Client(packet->sender, clientAddressString, this->serverSocket, clientGlobalId);
			client->SetAccountName(accountName);
			this->addClient(client, this->getClientIdentifier(packet));
			client->setNetworkClient(packet->networkClient);

			cout << "Created new client object with ID: " << clientGlobalId << endl;

			NetworkPacket* const respPacket = new NetworkPacket;
			respPacket->sender = packet->sender;
			respPacket->dataLength = 2;
			respPacket->networkClient = packet->networkClient;
			respPacket->data[0] = static_cast<char>(NetworkMessageType::CLIENT_ACTION);
			respPacket->data[1] = static_cast<char>(ClientActionType::JOIN);
			if (createdAccount)
			{
				NetworkPacket* const initPacket = new NetworkPacket;
				initPacket->sender = packet->sender;
				initPacket->dataLength = 2;
				initPacket->networkClient = packet->networkClient;
				initPacket->data[0] = static_cast<char>(NetworkMessageType::CLIENT_ACTION);
				initPacket->data[1] = static_cast<char>(ClientActionType::INITIALIZE);
				this->serverPacketHolder->receivePacket({ client, initPacket });
			}

			this->serverPacketHolder->receivePacket({ client, respPacket });
		}
		else //Existing connection
		{
			return;
		}

		NetworkPacket* const respPacket = new NetworkPacket;
		const ConnectionAcceptResponse response = { clientGlobalId };
		respPacket->sender = packet->sender;
		respPacket->dataLength = response.get(respPacket->data);
		respPacket->networkClient = packet->networkClient;
		this->queueNewPacket(respPacket);
	}

	void NetworkServerBase::addClient(Client* client, const std::string& clientIdentifier)
	{
		this->clients[client->GetGlobalID()] = client;
		this->clientIds[clientIdentifier] = client->GetGlobalID();
		client->setClientIdentifier(clientIdentifier);
	}

	string NetworkServerBase::getConnectionAccountName(const char* const packet)
	{
		const unsigned char accountNameLength = packet[ACCOUNT_LENGTH_POSITION];
		char* const accountName = new char[accountNameLength + 1];
		memcpy(accountName, packet + ACCOUNT_NAME_POSITION, accountNameLength);
		accountName[accountNameLength] = 0;
		const string nameString(accountName);
		delete[] accountName;

		return nameString;
	}

	string NetworkServerBase::getConnectionPassword(const char* const packet)
	{
		const unsigned char accountNameLength = packet[ACCOUNT_LENGTH_POSITION];
		const unsigned char passwordLength = packet[ACCOUNT_NAME_POSITION + accountNameLength];
		char* const password = new char[passwordLength + 1];
		memcpy(password, packet + ACCOUNT_NAME_POSITION + accountNameLength + 1, passwordLength);
		password[passwordLength] = 0;
		const string nameString(password);
		delete[] password;

		return nameString;
	}

	Client* NetworkServerBase::getClient(const std::string& clientIdentifier)
	{
		auto potentialClient = this->clientIds.find(clientIdentifier);
		if (potentialClient != this->clientIds.end())
		{
			return this->clients[potentialClient->second];
		}

		return nullptr;
	}

	Client* NetworkServerBase::getClient(const unsigned int id)
	{
		auto potentialClient = this->clients.find(id);
		if (potentialClient != this->clients.end())
		{
			return potentialClient->second;
		}

		return nullptr;
	}

	void NetworkServerBase::removeClient(const Client* const client)
	{
		this->clients.erase(client->GetGlobalID());
		this->clientIds.erase(client->getClientIdentifier());

		delete client;
	}

	void NetworkServerBase::handleGamePacket(const NetworkPacket* const packet)
	{
		const NetworkMessageType messageType = GetMessageType(packet->data);
		switch (messageType)
		{
		case NetworkMessageType::CONNECTION_REQUEST:
		{
			this->handleConnectionRequest(packet);
		}
		break;
		case NetworkMessageType::DISCONNECT_REQUEST:
		{
			Client* const client = this->getClient(packet);
			cout << "Disconnecting " << client->GetAccountName() << endl;
			NetworkPacket* const initPacket = new NetworkPacket;
			initPacket->sender = packet->sender;
			initPacket->dataLength = 2;
			initPacket->networkClient = packet->networkClient;
			initPacket->data[0] = static_cast<char>(NetworkMessageType::CLIENT_ACTION);
			initPacket->data[1] = static_cast<char>(ClientActionType::LEAVE);
			ClientMessage msg;
			msg.packet = initPacket;
			msg.clientId = client->GetGlobalID();
			this->serverPacketHolder->receivePacket(msg);
			

			this->disconnectClient(client);
			this->removeClient(client);
		}
		break;
		case NetworkMessageType::CLIENT_ACTION:
		{
			Client* const client = this->getClient(packet);
			if (client == nullptr)
			{
				cout << "Received client action from unknown client." << endl;
				break;
			}

			this->serverPacketHolder->receivePacket({ client, packet });
		}
		break;
		case NetworkMessageType::GET_PLAYER_INFO:
		{
			const AccountInfoRequest request = AccountInfoRequest::getAccountInfoRequest(packet->data + REQUEST_HEADER_SIZE);
			cout << request.accountId << endl;
			if (request.accountId == 0)
			{
				const AccountInfoResponse response = { request.accountId, string("AI") };
				NetworkPacket* const respPacket = new NetworkPacket;
				respPacket->dataLength = response.get(respPacket->data);
				respPacket->sender = packet->sender;
				respPacket->networkClient = packet->networkClient;
				this->queueNewPacket(respPacket);
				return;
			}
			else
			{
				const string accountName = this->userDAO->getAccountName(request.accountId);
				const AccountInfoResponse response = { request.accountId, accountName };
				NetworkPacket* const respPacket = new NetworkPacket;
				respPacket->dataLength = response.get(respPacket->data);
				respPacket->sender = packet->sender;
				respPacket->networkClient = packet->networkClient;
				this->queueNewPacket(respPacket);
				return;
			}
			/*Client* const client = this->getClient(request.accountId);
			if (client != nullptr)
			{
				const AccountInfoResponse response = { request.accountId, client->GetAccountName() };
				NetworkPacket* const respPacket = new NetworkPacket;
				respPacket->dataLength = response.get(respPacket->data);
				respPacket->sender = packet->sender;
				respPacket->networkClient = packet->networkClient;
				this->queueNewPacket(respPacket);
			}
			else if (request.accountId == 0)
			{
				const AccountInfoResponse response = { request.accountId, string("AI") };
				NetworkPacket* const respPacket = new NetworkPacket;
				respPacket->dataLength = response.get(respPacket->data);
				respPacket->sender = packet->sender;
				respPacket->networkClient = packet->networkClient;
				this->queueNewPacket(respPacket);
				return;
			}*/
		}
		break;
		default:
			cout << "Received unknown message type: " << static_cast<unsigned int>(messageType);
		}
	}

	Client* NetworkServerBase::getClient(const NetworkPacket* const networkPacket)
	{
		const std::string identifier = this->getClientIdentifier(networkPacket);
		return this->getClient(identifier);
	}

	int NetworkServerBase::getClientId(const string& accountName, bool& created)
	{
		return this->userDAO->getOrCreateAccountId(accountName, "", created);
	}

	void NetworkServerBase::SendObjectUpdate(const char* const data, const int dataLength)
	{
		//const Client* const client = this->getClient(clientId);
		for (auto i : this->clients)
		{
			const Client* const client = i.second;
			NetworkPacket* const packet = new NetworkPacket();
			packet->dataLength = MESSAGE_HEADER_SIZE + dataLength;
			packet->sender = client->GetAddress();
			SetPacketMessageType(packet->data, NetworkMessageType::OBJECT_STATE);
			SetPacketMessageData(packet->data, data, dataLength);
			packet->networkClient = client->getNetworkClient();
			this->queueNewPacket(packet);
		}
	}

	
}
