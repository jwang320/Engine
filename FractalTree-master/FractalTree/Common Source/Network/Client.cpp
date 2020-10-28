#include "pch.h"
#include "Client.h"

using namespace std;

namespace Network
{
	Client::Client(const sockaddr_in& address, const string& addressString, SOCKET socket, const unsigned int globalID) :
		address(address),
		addressString(addressString),
		socket(socket),
		globalID(globalID)
	{
	}
}