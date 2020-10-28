#pragma once
#include <string>

namespace Network
{
	class NetworkInitializionException
	{
		const std::string message;
	public:
		NetworkInitializionException(const std::string& message) : message(message) {}
	};
}