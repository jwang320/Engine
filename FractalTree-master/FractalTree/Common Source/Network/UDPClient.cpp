#include "pch.h"
#include "UDPClient.h"
#include "NetworkInitializationException.h"

using namespace std;

namespace Network
{
	void UDPClient::initializeSocket()
	{
		if ((this->serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		{
			const auto er = WSAGetLastError();
			throw new NetworkInitializionException("Socket initialization failed " + to_string(er));
		}


		DWORD nonBlocking = 1;
		sockaddr_in ad;
		ad.sin_family = AF_INET;
		ad.sin_addr.s_addr = inet_addr("127.0.0.1");
		ad.sin_port = htons(8890);
		//int aaa = bind(this->serverSocket, (sockaddr*)&ad, sizeof(sockaddr_in));
		//int er = WSAGetLastError();
		auto sss = ioctlsocket(this->serverSocket, FIONBIO, &nonBlocking);
		if (sss == SOCKET_ERROR)
		{
			throw new NetworkInitializionException("Failed to create socket ioctlsocket " + to_string(sss));
		}
	}

	unsigned int UDPClient::ConnectToServer(const string& accountName, const string& password, const string& serverIPAddress, const short serverPort)
	{
		this->serverIPAddress = serverIPAddress;
		this->serverPort = serverPort;

		this->serverAddress.sin_family = AF_INET;
		this->serverAddress.sin_addr.s_addr = inet_addr(this->serverIPAddress.c_str());
		this->serverAddress.sin_port = htons(this->serverPort);
		const std::string messageSend = "Connection Request: " + accountName;

		int received_bytes = -1;
		char resp[1024];
		int myId = -1;
		int attempts = 0;
		while (myId < 0 && attempts < 10)
		{
			string dataSend = string();
			dataSend.resize(50);
			dataSend[0] = (char)NetworkMessageType::CONNECTION_REQUEST;
			dataSend[1] = strlen(accountName.c_str());
			memcpy(&dataSend[2], accountName.c_str(), dataSend[1]);
			dataSend[2 + dataSend[1]] = strlen(password.c_str());
			memcpy(&dataSend[2] + dataSend[1] + 1, password.c_str(), strlen(password.c_str()));
			//dataSend.append((char)NetworkMessageType::CONNECTION_REQUEST);

			++attempts;
			const char* data = dataSend.c_str();
			auto ret = sendto(this->serverSocket, data, strlen(data), 0, (sockaddr*)&this->serverAddress, sizeof(sockaddr_in));
			if (ret < 0)
			{
				throw new NetworkInitializionException("Could not connect to server");
			}
			Sleep(500);

			socklen_t fromLength = sizeof(sockaddr_in);
			sockaddr_in sender;
			int dataLength;
			received_bytes = recvfrom(this->serverSocket, (char*)resp, 1024, 0, (sockaddr*)&sender, &fromLength);

			if (received_bytes > 0)
			{
				const NetworkMessageType messageType = GetMessageType(resp);

				switch (messageType)
				{
				case NetworkMessageType::CONNECTION_ACCEPT:
					myId = GetPlayerIdFromAcceptMessage(resp);
					break;
				}
			}
		}

		this->clientId = myId;
		return myId;
	}
	void UDPClient::disconnect()
	{
		char data[1] = { static_cast<char>(NetworkMessageType::DISCONNECT_REQUEST)};
		auto ret = sendto(this->serverSocket, data, strlen(data), 0, (sockaddr*)& this->serverAddress, sizeof(sockaddr_in));
	}
}