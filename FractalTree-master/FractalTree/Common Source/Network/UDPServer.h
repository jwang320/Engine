#pragma once
#include "NetworkServerBase.h"

namespace Network
{
	class UDPServer : public NetworkServerBase
	{
	private:
		const std::string port = "6112";

	protected:
		virtual void handlePackets() override;
		virtual void initializeSocket() override;

	public:
		using NetworkServerBase::NetworkServerBase;

	private:

		// Inherited via NetworkServerBase
		virtual std::string getClientIdentifier(const NetworkPacket* const packet) override;
	};
}