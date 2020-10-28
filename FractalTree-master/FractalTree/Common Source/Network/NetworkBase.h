#pragma once
#include "NetworkDefinitions.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <queue>

namespace Network
{
	enum NetworkType
	{
		HTTP = 1,
		WEBSOCKET = 2,
		GAME = 3
	};

	struct NetworkClient
	{
		SOCKET socket;
		sockaddr clientAddress;
		std::string clientAddressString;
		bool active = true;
		bool keepOpen = false;
		NetworkType networkType = HTTP;
		unsigned long long framesSinceUpdate = 0;
	};

	struct NetworkPacket
	{
		char data[PACKET_SIZE];
		socklen_t fromLength = sizeof(sockaddr_in);
		sockaddr_in sender;
		int dataLength;
		NetworkClient* networkClient = nullptr;
	};

	class NetworkBase
	{
	protected:
		std::queue<NetworkPacket*> receivedPackets;
		std::queue<NetworkPacket*> networkSendQueue;

	protected:
		SOCKET serverSocket;

	public:
		void InitializeNetwork();
		void Update();
		virtual ~NetworkBase() {}
	protected:
		virtual void receiveNewPackets();
		virtual void sendNewPackets();
		void getReceivedPacketList(std::vector<const NetworkPacket*>& receivedPacketList);
		void queueNewPacket(NetworkPacket* packet);
		virtual void handlePackets() = 0;
		virtual void initializeSocket() = 0;
		static void beDoneWithPacket(const NetworkPacket* const packet); //should be called to clean up packet memory
		static void beDoneWithPackets(const std::vector<const NetworkPacket*>& packets);
	protected:
		void lockPacketReceiveQueue() {}
		void unlockPacketReceiveQueue() {}
		void lockPacketSendQueue() {}
		void unlockPacketSendQueue() {}
		void initializeWSA();
	};
}