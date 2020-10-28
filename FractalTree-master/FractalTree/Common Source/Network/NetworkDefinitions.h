#pragma once

#define PACKET_SIZE 8000
#define MAX_ACCOUNT_LENGTH 8
#define ACCOUNT_LENGTH_POSITION 1
#define ACCOUNT_NAME_POSITION 2
#define ACCEPT_ID_POSITION 1
#define MESSAGE_TYPE_POSITION 0
#define MESSAGE_DATA_POSITION 1
#define MESSAGE_DATA_POSITION_SIZE 4

#define MESSAGE_HEADER_SIZE MESSAGE_DATA_POSITION + MESSAGE_DATA_POSITION_SIZE
//position of action type value (1 byte)
#define CLIENT_ACTION_TYPE_POSITION 1
//position of action data (? byte)
#define CLIENT_ACTION_DATA_POSITION CLIENT_ACTION_TYPE_POSITION + 1


namespace Network
{
	constexpr auto REQUEST_HEADER_SIZE = sizeof(unsigned char);
	enum class NetworkMessageType : unsigned char
	{
		NONE,
		CONNECTION_REQUEST,
		CONNECTION_ACCEPT,
		OBJECT_STATE,
		CLIENT_ACTION,
		GET_PLAYER_INFO,
		PLAYER_INFO_RESPONSE,
		DISCONNECT_REQUEST
	};

	enum class ClientActionType : unsigned char
	{
		NONE,
		SIMULATION_COMMAND,
		INITIALIZE,
		JOIN,
		LEAVE
	};

	struct AccountInfoRequest
	{
		unsigned int accountId;
		static AccountInfoRequest getAccountInfoRequest(const char* const data) {
			AccountInfoRequest result;
			memcpy(&result, data, sizeof(accountId));

			return result;
		}
	};

	struct AccountInfoResponse
	{
		unsigned int accountId;
		std::string accountName;

		unsigned int get(char* const data) const
		{
			unsigned int offset = 0;
			data[offset] = static_cast<char>(NetworkMessageType::PLAYER_INFO_RESPONSE); 
			offset += REQUEST_HEADER_SIZE;
			memcpy(data + offset, &this->accountId, sizeof(this->accountId));
			offset += sizeof(this->accountId);
			const int len = this->accountName.length();
			memcpy(data + offset, &len, sizeof(int));
			offset += sizeof(int);
			memcpy(data + offset, accountName.c_str(), accountName.length());
			offset += accountName.length();

			return offset;
		}
	};

	struct DisconnectRequest
	{
		unsigned int get(char* const data) const
		{
			unsigned int offset = 0;
			data[offset] = static_cast<char>(NetworkMessageType::DISCONNECT_REQUEST);
			offset += REQUEST_HEADER_SIZE;

			return offset;
		}
	};

	struct ConnectionAcceptResponse
	{
		unsigned int clientGlobalId;

		unsigned int get(char* const data) const
		{
			unsigned int offset = 0;
			data[offset] = static_cast<char>(NetworkMessageType::CONNECTION_ACCEPT);
			offset += REQUEST_HEADER_SIZE;
			memcpy(data + offset, &this->clientGlobalId, sizeof(this->clientGlobalId));
			offset += sizeof(this->clientGlobalId);

			return offset;
		}
	};

	struct ClientActionRequest
	{
		static ClientActionRequest get(const char* const data) {

		}
	};

	unsigned int GetPlayerIdFromAcceptMessage(const char* message);
	NetworkMessageType GetMessageType(const char* message);
	void SetPacketMessageType(char* message, NetworkMessageType messageType);
	void SetPacketMessageData(char* message, const char*const  data, unsigned int length);
	const char* GetMessageDataPointer(const char* const message);
	ClientActionType GetClientActionTypeFromMessage(const char* message);
	const char* GetClientActionDataPointer(const char* message);
}