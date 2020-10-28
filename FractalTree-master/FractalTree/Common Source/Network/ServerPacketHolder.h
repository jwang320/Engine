#pragma once
#include <queue>
#include "NetworkBase.h"

namespace Network
{
	class Client;

	class ClientMessage
	{
	public:
		Client* client = nullptr;
		const NetworkPacket* packet;
		unsigned int dataOffset = 0;
		int clientId = -1;
	public:
		void clear() {
			delete this->packet;
			this->packet = nullptr;
		}

		const char* data() const {
			return this->packet->data + this->dataOffset;
		}

		int dataLength() const {
			return this->packet->dataLength;
		}
	};

	class ServerPacketHolder
	{
	private:
		std::queue<ClientMessage> receivedPackets;

	public:
		void receivePacket(const ClientMessage& clientMessage) {
			this->receivedPackets.push(clientMessage);
		}

		void getAllPackets(std::vector<ClientMessage>& results) {
			results.reserve(this->receivedPackets.size());
			while (!this->receivedPackets.empty())
			{
				results.push_back(this->receivedPackets.front());
				this->receivedPackets.pop();
			}
		}
	};
}