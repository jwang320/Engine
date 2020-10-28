#include "pch.h"
#include "UDPServer.h"
#include <iostream>
#include "NetworkInitializationException.h"

using namespace std;
bool printEverything = false;
bool printThreading = false;
bool printError = false;

namespace Network
{
	void UDPServer::handlePackets()
	{
		vector<const NetworkPacket*> receivedPackets;
		this->getReceivedPacketList(receivedPackets);

		for (const NetworkPacket* const packet : receivedPackets)
		{
			this->handleGamePacket(packet);
		}
	}

	void UDPServer::initializeSocket()
	{
		addrinfo* result = NULL, * ptr = NULL, hints;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;
		auto iResult = getaddrinfo(NULL, this->port.c_str(), &hints, &result);
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

		DWORD nonBlocking = 1;
		int sss = ioctlsocket(this->serverSocket, FIONBIO, &nonBlocking);
	}

	std::string UDPServer::getClientIdentifier(const NetworkPacket* const packet)
	{
		const std::string clientAddressString(inet_ntoa(packet->sender.sin_addr));
		return clientAddressString + ":" + std::to_string(packet->sender.sin_port);
	}
}
