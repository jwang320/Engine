#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#endif

namespace Network
{
	class Client
	{
	private:
		int localID;
		int globalID;
		sockaddr_in address;
		std::string addressString;
		int socket;
		time_t lastConnectionTime;
		std::string accountName;

	public:
		Client(const sockaddr_in& address, const std::string& addressString, int socket);

		void SetLastConnectionTime(time_t lastConnectionTime) {this->lastConnectionTime = lastConnectionTime;}
		void SetGlobalID(int globalID) {this->globalID = globalID;}
		void SetLocalID(int localID) {this->localID = localID;}
		void SetAccountName(const std::string& accountName) {this->accountName = accountName;}

		sockaddr_in GetAddress() const {return this->address;}
		std::string GetAddressString() const {return this->addressString;}
		std::string GetAccountName() const {return this->accountName;}
		time_t GetLastConnectionTime() const {return this->lastConnectionTime;}
		int GetGlobalID() const {return this->globalID;}
	};
}