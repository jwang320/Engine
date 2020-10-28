#include "pch.h"
#include "SpaceGameUDPClient.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"

using namespace std;
using namespace Network;

namespace SpaceGameCore
{
	void SpaceGameUDPClient::handlePackets()
	{
		vector<const NetworkPacket*> receivedPackets;
		this->getReceivedPacketList(receivedPackets);

		for (const NetworkPacket* const packet : receivedPackets)
		{
			const NetworkMessageType messageType = GetMessageType(packet->data);
			switch (messageType)
			{
			case NetworkMessageType::OBJECT_STATE:
			{
				if (this->simulation != nullptr)
				{
					const char* const objectData = GetMessageDataPointer(packet->data);
					this->simulation->UpdateObjectFromSerializedData(objectData);
				}
			}
			break;
			}
		}

		this->beDoneWithPackets(receivedPackets);
	}

	void SpaceGameUDPClient::SendCommand(ClientActionType actionType, const char * const const const commandData, int dataLength)
	{
		NetworkPacket* packet = new NetworkPacket;
		packet->sender = this->serverAddress;
		SetPacketMessageType(packet->data, NetworkMessageType::CLIENT_ACTION);
		memcpy(packet->data + CLIENT_ACTION_TYPE_POSITION, &actionType, sizeof(actionType));
		memcpy(packet->data + CLIENT_ACTION_DATA_POSITION, commandData, dataLength);

		packet->dataLength = CLIENT_ACTION_DATA_POSITION + dataLength;

		this->queueNewPacket(packet);
	}
}
