#include "pch.h"
#include "Client.h"

namespace Network
{
	Client::Client(const sockaddr_in& address, const std::string& addressString, int socket) :
		address(address),
		addressString(addressString),
		socket(socket),
		accountName("")
	{
		this->lastConnectionTime = time(NULL);
	}
}