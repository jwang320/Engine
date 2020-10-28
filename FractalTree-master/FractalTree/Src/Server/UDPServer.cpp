#include "pch.h"
#include "UDPServer.h"
#include <iostream>
#include "Client.h"

using namespace std;

namespace Network
{
	UDPServer::UDPServer(const std::string& port) :
		UDPNetworkBase(),
		port(port)
	{

	}

	void UDPServer::initialize()
	{

	}

	void UDPServer::Update()
	{
		this->ReceiveAllPackets();
		this->analyzePackets();
	}

	bool UDPServer::handlePacket(const Packet& packet)
	{
		PacketType packetType = UDPNetworkBase::GetPacketType(packet);

		switch (packetType)
		{
		case Network::PacketType::Ack:
			return true;
		case Network::PacketType::ConnectionRequest:
			{
				
				std::string clientAddressString(inet_ntoa(packet.sender.sin_addr));

				cout << packet.data << endl;

				Client* client = this->getClient(clientAddressString);

				//New connection
				if(client == NULL)
				{
					client = new Client(packet.sender, clientAddressString, this->serverSocket);
					client->SetGlobalID(static_cast<int>(this->clients.size()));
					this->AddClient(client);
					int globalID = client->GetGlobalID();
					char acceptStr[sizeof(globalID)];
					memcpy(acceptStr, &globalID, sizeof(globalID));
					int s = this->SendMessageD(PacketType::ConnectionAccept, packet.sender, acceptStr, sizeof(globalID));
					std::cout << "Created new client object with ID: " << globalID << std::endl;
				}
				else //Existing connection
				{
					client->SetLastConnectionTime(time(NULL));
					std::cout << "Received connection request from existing client with ID: " << client->GetGlobalID() << std::endl;
				}

				return true;
			}
		default:
			return false;
		}
	}

	Client* UDPServer::CreateClient(const Packet& receivedPacket)
	{
		const std::string clientAddressString(inet_ntoa(receivedPacket.sender.sin_addr));

		std::cout << receivedPacket.data << std::endl;

		Client* client = this->getClient(clientAddressString);

		//New connection
		if(client == NULL)
		{
			client = new Client(receivedPacket.sender, clientAddressString, this->serverSocket);
			client->SetGlobalID(static_cast<int>(this->clients.size()));
			this->AddClient(client);
			int globalID = client->GetGlobalID();
			char acceptStr[sizeof(globalID)];
			memcpy(acceptStr, &globalID, sizeof(globalID));
			int s = this->SendMessageD(PacketType::ConnectionAccept, receivedPacket.sender, acceptStr, sizeof(globalID));
			std::cout << "Created new client object with ID: " << globalID << std::endl;

			return client;
		}
		else //Existing connection
		{
			int globalID = client->GetGlobalID();
			char acceptStr[sizeof(globalID)];
			memcpy(acceptStr, &globalID, sizeof(globalID));
			int s = this->SendMessageD(PacketType::ConnectionAccept, receivedPacket.sender, acceptStr, sizeof(globalID));
			client->SetLastConnectionTime(time(NULL));
			std::cout << "Received connection request from existing client with ID: " << client->GetGlobalID() << std::endl;
			return NULL;
		}
	}
	int UDPServer::SendPacketToClients(const char* data, int dataLength) const
	{
		//for(unsigned int a = 0; a < this->clients.size(); ++a)
		int packetCount = 0;
		for(auto it = this->clients.begin(); it != this->clients.end(); ++it)
		{
			//this->SendPacket()
			const sockaddr_in& addr = it->second->GetAddress();
			sendto(this->serverSocket, data, dataLength, 0, (sockaddr*)&(addr), sizeof(addr));
			++packetCount;
		}

		return packetCount;
	}

	Client* UDPServer::getClient(const string& ipAddress)
	{
		auto potentialClient =  this->clients.find(ipAddress);
		if(potentialClient != this->clients.end())
		{
			return potentialClient->second;
		}

		return NULL;
	}

	void UDPServer::AddClient(Client* client)
	{
		this->clients[client->GetAddressString()] = client;
	}

	bool UDPServer::initializeServerSocket()
	{
		addrinfo *result = NULL, *ptr = NULL, hints;

		memset(&hints, 0, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;
		auto iResult = getaddrinfo(NULL, this->port.c_str(), &hints, &result);
		if (iResult != 0)
		{
			std::cout << "getaddrinfo failed: " << iResult << std::endl;
#ifdef _WIN32
			WSACleanup();
#endif
			return false;
		}

		this->serverSocket = static_cast<int>(socket(result->ai_family, result->ai_socktype, result->ai_protocol));
		if (this->serverSocket == INVALID_SOCKET) 
		{
			std::cout << "Error at socket(): " << std::endl;
			freeaddrinfo(result);
#ifdef _WIN32
			WSACleanup();
#endif
			return false;
		}

		auto iResult2 = bind(this->serverSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));

		if (iResult2 == SOCKET_ERROR)
		{
			std::cout << "bind() failed with error: " << std::endl;
			freeaddrinfo(result);
#ifdef _WIN32
			closesocket(this->serverSocket);
			WSACleanup();
#else
			close(this->serverSocket);
#endif
			return false;
		}

		DWORD nonBlocking = 1;
		int sss = ioctlsocket(this->serverSocket, FIONBIO, &nonBlocking);

		return true;
	}
}