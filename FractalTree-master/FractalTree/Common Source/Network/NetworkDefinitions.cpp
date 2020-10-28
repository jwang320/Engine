#include "pch.h"
#include "NetworkDefinitions.h"

namespace Network
{
	unsigned int GetPlayerIdFromAcceptMessage(const char* message)
	{
		unsigned int id;
		memcpy(&id, message + 1, sizeof(unsigned int));
		return id;
	}

	NetworkMessageType GetMessageType(const char* message)
	{
		return static_cast<NetworkMessageType>(message[0]);
	}

	void SetPacketMessageType(char* message, NetworkMessageType messageType)
	{
		message[MESSAGE_TYPE_POSITION] = static_cast<char>(messageType);
	}

	void SetPacketMessageData(char* message, const char* const data, unsigned int length)
	{
		memcpy(message + MESSAGE_DATA_POSITION, data, length);
	}

	const char* GetMessageDataPointer(const char* const message)
	{
		return message + MESSAGE_DATA_POSITION;
	}

	ClientActionType GetClientActionTypeFromMessage(const char* message)
	{
		return static_cast<ClientActionType>(message[CLIENT_ACTION_TYPE_POSITION]);
	}

	const char* GetClientActionDataPointer(const char* message)
	{
		return message + CLIENT_ACTION_DATA_POSITION;
	}
}
