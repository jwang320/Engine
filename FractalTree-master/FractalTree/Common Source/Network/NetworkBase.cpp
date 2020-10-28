#include "pch.h"
#include "NetworkBase.h"
#include "NetworkInitializationException.h"
#include <iostream>
#include <WinSock2.h>

using namespace std;

bool initialized = false;
namespace Network
{
	void NetworkBase::InitializeNetwork()
	{
		if (!initialized) {
			this->initializeWSA();
			initialized = true;
		}
		cout << "WSA initialization successful." << endl;

		this->initializeSocket();
		cout << "Socket initialization successful" << endl;;
	}

	void NetworkBase::receiveNewPackets()
	{
		int receivedBytes = 0;
		do 
		{
			//TODO don't do this
			NetworkPacket* const receivedPacket = new NetworkPacket;
			receivedBytes = recvfrom(this->serverSocket, (char*)receivedPacket->data, PACKET_SIZE, 0, (sockaddr*)&receivedPacket->sender, &receivedPacket->fromLength);

			if (receivedBytes > 0)
			{
				receivedPacket->dataLength = receivedBytes;
				this->lockPacketReceiveQueue();
				this->receivedPackets.push(receivedPacket);
				this->unlockPacketReceiveQueue();
			}
			else
			{
				delete receivedPacket;
			}
		} while (receivedBytes > 0);
	}

	void NetworkBase::sendNewPackets()
	{
		while (!this->networkSendQueue.empty())
		{
			this->lockPacketSendQueue();
			const NetworkPacket* const packet = this->networkSendQueue.front();
			this->networkSendQueue.pop();
			this->unlockPacketSendQueue();
			const auto ret = sendto(this->serverSocket, packet->data, packet->dataLength, 0, (sockaddr*)&packet->sender, packet->fromLength);

			delete packet;
		}
	}

	void NetworkBase::queueNewPacket(NetworkPacket* packet)
	{
		this->lockPacketSendQueue();
		this->networkSendQueue.push(packet);
		this->unlockPacketReceiveQueue();
	}

	void NetworkBase::Update()
	{
		this->receiveNewPackets();
		this->handlePackets();
		this->sendNewPackets();
	}

	void NetworkBase::getReceivedPacketList(vector<const NetworkPacket*>& receivedPacketList)
	{
		this->lockPacketReceiveQueue();
		while (!this->receivedPackets.empty())
		{
			receivedPacketList.push_back(this->receivedPackets.front());
			this->receivedPackets.pop();
		}

		this->unlockPacketReceiveQueue();
	}

	void NetworkBase::beDoneWithPacket(const NetworkPacket* const packet)
	{
		delete packet;
	}

	void NetworkBase::beDoneWithPackets(const std::vector<const NetworkPacket*>& packets)
	{
		for (const NetworkPacket* const packet : packets)
		{
			delete packet;
		}
	}

	void NetworkBase::initializeWSA()
	{
		WSADATA wsaData;

		// Initialize Winsock
		const int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			throw new NetworkInitializionException("WSA initialization failed");
		}
	}
}
