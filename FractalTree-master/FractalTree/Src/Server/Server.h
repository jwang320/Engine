#pragma once

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#endif

namespace Server
{
	class UDPNetworkModule
	{
	protected:
		int serverSocket;
		std::string port;
		bool bindSpecific;

	public:
		//InitializeServer() must always be called before using a server object
		bool InitializeServer();

		virtual ~UDPNetworkModule();

	protected:
		UDPNetworkModule(bool bindSpecific, const std::string& port = "");

		//Override initialize() with server-specific initialization
		virtual void initialize() = 0;
		virtual bool initializeServerSocket();

	private:
		bool initializeWSA();
	};
}