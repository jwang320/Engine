#include "pch.h"
#include "Server.h"
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

namespace Server
{
	UDPNetworkModule::UDPNetworkModule(bool bindSpecific, const std::string& port) :
		bindSpecific(bindSpecific),
		port(port)
	{

	}

	bool UDPNetworkModule::InitializeServer()
	{
		if(!this->initializeWSA())
		{
			return false;
		}
		if(!this->initializeServerSocket())
		{
			return false;
		}

		this->initialize();
		return true;
	}

	bool UDPNetworkModule::initializeWSA()
	{
#ifdef _WIN32
		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) 
		{
			std::cout << "WSA initialization failed: " << iResult << std::endl;
			return false;
		}

		std::cout << "WSA initialization successful." << std::endl;
#endif
		return true;
	}

	bool UDPNetworkModule::initializeServerSocket()
	{
		addrinfo *result = NULL, *ptr = NULL, hints;

		memset(&hints, 0, sizeof (hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		auto iResult = getaddrinfo(NULL, this->port.c_str(), &hints, &result);
		if (iResult != 0)
		{
			std::cout << "getaddrinfo failed: " << iResult << std::endl;
#ifdef _WIN32
			WSACleanup();
#endif
			return false;
		}

		this->serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (this->serverSocket == INVALID_SOCKET) 
		{
			std::cout << "Error at socket(): " << std::endl;
			freeaddrinfo(result);
#ifdef _WIN32
			WSACleanup();
#endif
			return false;
		}

		std::cout << "Socket creation successful." << std::endl;

		auto iResult2 = bind(this->serverSocket, result->ai_addr, result->ai_addrlen);

		if (iResult2 == SOCKET_ERROR)
		{
			std::cout << "bind() failed with error: " << std::endl;
			freeaddrinfo(result);
#ifdef _WIN32
			closesocket(this->serverSocket);
			WSACleanup();
#else
			close(this->serverSocket);
#endif
			return false;
		}

#ifdef _WIN32
		DWORD nonBlocking = 1;
		ioctlsocket(this->serverSocket, FIONBIO, &nonBlocking);
#else
		int nonBlocking = 1;
		ioctl(this->serverSocket, FIONBIO, &nonBlocking);
#endif
		return true;
	}

	UDPNetworkModule::~UDPNetworkModule()
	{
		
	}
}