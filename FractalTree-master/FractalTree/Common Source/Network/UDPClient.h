#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <string>
#include "NetworkBase.h"

namespace Network
{

	class UDPClient : public NetworkBase
	{
	private:
		const std::string port = "6112";
		short serverPort;
		std::string serverIPAddress;

	protected:
		sockaddr_in serverAddress;
		int clientId = -1;

	public:
		unsigned int ConnectToServer(const std::string& accountName, const std::string& password, const std::string& serverIPAddress, const short serverPort);
		void disconnect();
	protected:
		virtual void initializeSocket() override;
	};
}