#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

namespace Network
{
	struct NetworkClient;
	class Client
	{
	private:
		int localID;
		const unsigned int globalID;
		sockaddr_in address;
		std::string addressString;
		const SOCKET socket;
		time_t lastConnectionTime = time(nullptr);
		std::string accountName = "";
		NetworkClient* networkClient = nullptr;
		std::string clientIdentifier;

	public:
		Client(const sockaddr_in& address, const std::string& addressString, SOCKET socket, const unsigned int globalId);

		void SetLastConnectionTime(time_t lastConnectionTime) { this->lastConnectionTime = lastConnectionTime; }
		void SetLocalID(int localID) { this->localID = localID; }
		void SetAccountName(const std::string& accountName) { this->accountName = accountName; }
		void SetAddress(const sockaddr_in& address) { this->address = address; }
		void SetAddressString(const std::string& addressString) { this->addressString = addressString; }
		void setNetworkClient(NetworkClient* const n) { this->networkClient = n; }
		void setClientIdentifier(const std::string& clientIdentifier) { this->clientIdentifier = clientIdentifier; }
		sockaddr_in GetAddress() const { return this->address; }
		std::string GetAddressString() const { return this->addressString; }
		std::string GetAccountName() const { return this->accountName; }
		time_t GetLastConnectionTime() const { return this->lastConnectionTime; }
		unsigned int GetGlobalID() const { return this->globalID; }
		NetworkClient* getNetworkClient() const { return this->networkClient; }
		std::string getClientIdentifier() const { return this->clientIdentifier; }
	};
}