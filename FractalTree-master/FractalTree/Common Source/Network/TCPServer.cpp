#include "pch.h"
#include "TCPServer.h"
#include "NetworkInitializationException.h"
#include <iostream>
#include "../../WebServerVexal/WebServerVexal/src/WebApp/WebPageApp/WebPageApp.h"
#include "ServerPacketHolder.h"
#include "Client.h"

using namespace Network;

#define MAX_REQUEST_SIZE 16284
#define MAX_IDLE_FRAMES 10000

TCPServer::TCPServer(UserDAO* const userDAO, ServerPacketHolder* const serverPacketHolder) :
	NetworkServerBase(userDAO, serverPacketHolder)
{
	std::unordered_map<std::string, WebApp*> controllers = std::unordered_map<std::string, WebApp*>();
	controllers["web"] = new WebPageApp(&this->httpServer);
	this->httpServer.InitializeControllers(controllers);
}

void TCPServer::receiveNewPackets()
{
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(this->serverSocket, &readSet);
	TIMEVAL t = timeval();
	t.tv_usec = 0;
	const int readyCount = select(1, &readSet, nullptr, nullptr, &t);
	if (readyCount > 0)
	{
		sockaddr clientAddress;
		socklen_t clientAddressLength = sizeof(clientAddress);
		const SOCKET clientSocket = accept(this->serverSocket, &clientAddress, &clientAddressLength);
		const sockaddr_in* const add2 = reinterpret_cast<sockaddr_in*>(&clientAddress);
		const std::string clientAddressString(inet_ntoa(add2->sin_addr));
		this->netClients[clientSocket] = new NetworkClient{ clientSocket, clientAddress, clientAddressString };
	}

	std::vector<NetworkPacket*> newPackets = std::vector<NetworkPacket*>();
	for (auto client : this->netClients)
	{
		if (!client.second->active)
		{
			continue;
		}

		fd_set clientReadSet;
		FD_ZERO(&clientReadSet);
		FD_SET(client.second->socket, &clientReadSet);
		TIMEVAL clientT = timeval();
		clientT.tv_usec = 0;
		const int readyCount = select(1, &clientReadSet, nullptr, nullptr, &clientT);
		if (readyCount == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			std::cout << "Error select on socket " << client.second->socket << " error: " << err << std::endl;
		}
		if (readyCount <= 0)
		{
			if (client.second->framesSinceUpdate > MAX_IDLE_FRAMES)
			{
				std::cout << "closing idle socket " << client.second->socket << std::endl;
				client.second->keepOpen = false;
				closeClientSocket(client.second->socket, client.second->clientAddressString);
				ClientMessage msg;
				NetworkPacket* const initPacket = new NetworkPacket;
				initPacket->dataLength = 2;
				initPacket->networkClient = client.second;
				initPacket->data[0] = static_cast<char>(NetworkMessageType::CLIENT_ACTION);
				initPacket->data[1] = static_cast<char>(ClientActionType::LEAVE);
				msg.packet = initPacket;
				msg.clientId = this->getClient(std::to_string(client.first))->GetGlobalID();
				this->serverPacketHolder->receivePacket(msg);
				this->clientsToClose.push_back(client.second);
			}
			else
			{
				++client.second->framesSinceUpdate;
			}
			continue;
		}
		client.second->framesSinceUpdate = 0;
		//TODO don't do this
		NetworkPacket* const receivedPacket = new NetworkPacket;
		const int receivedBytes = recv(client.second->socket, receivedPacket->data, PACKET_SIZE - 2, 0);
		if (receivedBytes == SOCKET_ERROR)
		{
			const int err = WSAGetLastError();
			std::cout << "Received socket error recv " << client.second->socket << " error: " << err <<  std::endl;
		}
		if (receivedBytes > 0)
		{
			receivedPacket->data[receivedBytes] = 0;
			receivedPacket->dataLength = receivedBytes;
			receivedPacket->networkClient = client.second;
			newPackets.push_back(receivedPacket);
		}
		else if (receivedBytes == 0 && !client.second->keepOpen)
		{
			std::cout << "Shutting down socket for good " << std::endl << std::endl;
			this->closeClient(client.second->socket);
			delete receivedPacket;
			break;
		}
		else
		{
			delete receivedPacket;
		}
	}

	if (!newPackets.empty())
	{
		this->lockPacketReceiveQueue();
		for (NetworkPacket* const packet : newPackets)
		{
			this->receivedPackets.push(packet);
		}
		this->unlockPacketReceiveQueue();
	}
}

void Network::TCPServer::handlePackets()
{
	std::vector<const NetworkPacket*> newPackets;
	this->getReceivedPacketList(newPackets);

	for (const NetworkPacket* const receivedPacket : newPackets)
	{
		NetworkClient* const client = receivedPacket->networkClient;
		if (client->networkType == WEBSOCKET)
		{
			const std::string websocketsMessage = this->httpServer.decodeWebSocketMessage(receivedPacket->data, receivedPacket->dataLength);
			//std::cout << "Received websockets message: " << websocketsMessage << std::endl;
			if (websocketsMessage[0] == 'Q' && websocketsMessage[1] == 'Q')
			{
				NetworkPacket* const transformedPacket = new NetworkPacket;
				memcpy(transformedPacket->data, websocketsMessage.c_str() + 2, websocketsMessage.length());
				transformedPacket->dataLength = websocketsMessage.length();
				transformedPacket->networkClient = client;
				transformedPacket->sender = receivedPacket->sender;
				transformedPacket->fromLength = receivedPacket->fromLength;
				this->handleGamePacket(transformedPacket);
			}
		}
		else if (client->networkType == GAME)
		{

		}
		else
		{
			const HandleResult res = this->httpServer.HandleClientRequest(receivedPacket->data, client->socket, client->clientAddressString);
			if (res != HandleResult::NOT_HTTP)
			{
				client->networkType = HTTP;
				if (res == HandleResult::KEEP_OPEN)
				{
					client->keepOpen = true;
				}
				else if (res == HandleResult::WEBSOCKET)
				{
					client->keepOpen = true;
					client->networkType = WEBSOCKET;
				}
				else
				{
					closeClientSocket(client->socket, client->clientAddressString);
				}
			}
		}
	}

	this->beDoneWithPackets(newPackets);

	for (const NetworkClient* const client : this->clientsToClose)
	{
		this->netClients.erase(client->socket);
		const std::string identifier = std::to_string(client->socket);
		const Client* gameClient = this->getClient(identifier);
		if (gameClient != nullptr)
		{
			this->removeClient(gameClient);
		}
		delete client;
	}

	this->clientsToClose.clear();
}

void Network::TCPServer::sendNewPackets()
{
	while (!this->networkSendQueue.empty())
	{
		this->lockPacketSendQueue();
		const NetworkPacket* const packet = this->networkSendQueue.front();
		this->networkSendQueue.pop();
		this->unlockPacketSendQueue();
		if (packet->networkClient->networkType == WEBSOCKET)
		{
			this->httpServer.sendWebSocketMessageShort(packet->networkClient->socket, packet->data, packet->dataLength, true);
		}
		else
		{
			const auto ret = sendto(this->serverSocket, packet->data, packet->dataLength, 0, (sockaddr*)& packet->sender, packet->fromLength);
		}
		delete packet;
	}
}
void Network::TCPServer::closeClientSocket(const SOCKET clientSocket, const std::string& clientAddressString)
{
	using namespace LoggingUtil;
	const auto ret = shutdown(clientSocket, SD_SEND);
	if (ret == SOCKET_ERROR)
	{
		this->log << "failed to shutdown from " << clientAddressString << " " << WSAGetLastError() << lendl();
		this->closeClient(clientSocket);
		return;
	}
}

void Network::TCPServer::closeClient(const SOCKET socket)
{
	if (socket < 0)
	{
		return;
	}

	auto c = this->netClients.find(socket);
	if (c != netClients.end())
	{
		this->netClients[socket]->active = false;
	}
	closesocket(socket);
}

std::string Network::TCPServer::getClientIdentifier(const NetworkPacket* const packet)
{
	return std::to_string(packet->networkClient->socket);
}

void TCPServer::initializeSocket()
{
	addrinfo* result = NULL, * ptr = NULL, hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	auto iResult = getaddrinfo(NULL, "80", &hints, &result);
	if (iResult != 0)
	{
		WSACleanup();
		throw NetworkInitializionException("getaddrinfo failed " + std::to_string(iResult));
	}

	this->serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (this->serverSocket == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << std::endl;
		freeaddrinfo(result);
		WSACleanup();
	}



	const int iResult2 = ::bind(this->serverSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));

	if (iResult2 == SOCKET_ERROR)
	{
		freeaddrinfo(result);
		closesocket(this->serverSocket);
		WSACleanup();
		throw  NetworkInitializionException("bind() failed with error: " + std::to_string(iResult2));
	}

	iResult = listen(this->serverSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(this->serverSocket);
		WSACleanup();
	}

	DWORD nonBlocking = 1;
	int sss = ioctlsocket(this->serverSocket, FIONBIO, &nonBlocking);
}

void TCPServer::disconnectClient(const Network::Client* client) {
	this->clientsToClose.push_back(client->getNetworkClient());
}

