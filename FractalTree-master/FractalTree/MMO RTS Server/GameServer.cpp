#include "pch.h"
#include "GameServer.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"
#include "..\Common Source\DAO\MySqlUserDAO.h"
#include "..\Common Source\DAO\NullUserDAO.h"
#include "..\Common Source\Network\TCPServer.h"
#include "..\Common Source\Network\ServerPacketHolder.h"
#include "..\Common Source\Network\Client.h"
#include "..\Common Source\Network\UDPServer.h"
#include "..\Common Source\DAO\MySqlEntityStateLoader.h"


using namespace Network;
using namespace SpaceGameCore;
using namespace std;
using namespace SimulationEngine;

static double performanceFrequency;

static const long long frameInterval = 16;

namespace RTSServer
{
	GameServer::GameServer()
	{
		//performance frequency relates to elapsed time calculations for
		//timing.
		_LARGE_INTEGER perfFreq;
		QueryPerformanceFrequency(&perfFreq);
		performanceFrequency = perfFreq.QuadPart / 1000.0;
	}

	_LARGE_INTEGER startTimer() {
		_LARGE_INTEGER startTime; 
		QueryPerformanceCounter(&startTime);
		return startTime;
	}

	double elapsed(const _LARGE_INTEGER startTime) {
		_LARGE_INTEGER endTime; QueryPerformanceCounter(&endTime);
		return (endTime.QuadPart - startTime.QuadPart) / performanceFrequency;
	}

	void GameServer::Run()
	{
		_LARGE_INTEGER frameStart;
		frameStart.QuadPart = 0LL;
		bool newFrame = true;
		while (true)
		{
			if (newFrame)
			{
				newFrame = false;
				QueryPerformanceCounter(&frameStart);
			}

			_LARGE_INTEGER currentTick;
			QueryPerformanceCounter(&currentTick);
			const double elapsedTime = (currentTick.QuadPart - frameStart.QuadPart) / performanceFrequency;
			if (elapsedTime >= frameInterval)
			{
				newFrame = true;

				const _LARGE_INTEGER clientReceive = startTimer();
				this->handleClientMessages();
				const double clientMessageTime = elapsed(clientReceive);


				const _LARGE_INTEGER udpStart = startTimer();
				this->networkServer->Update();
				const double udpTime = elapsed(udpStart);

				const _LARGE_INTEGER tcpStart = startTimer();
				this->tcpServer->Update();
				const double tcpTime = elapsed(tcpStart);

				const _LARGE_INTEGER simulationStart = startTimer();
				this->simulation->Update();
				const double simulationTime = elapsed(simulationStart);


				const _LARGE_INTEGER clientUpdateStart = startTimer();
				this->updateClients();
				const double clientUpdateTime = elapsed(clientUpdateStart);

				const double totalTime = elapsed(clientReceive);

				if (rand() % 200 == 0)
				{
					cout << "Total: " << totalTime << " (client rcv: " << clientMessageTime << 
						", udp srv: " << udpTime << ", tcp srv: " << tcpTime << ", sim: " << simulationTime << ", client up: " << 
						clientUpdateTime << ")" << endl;
				}
			}
			else
			{
				Sleep(1);
			}
		}
	}

	void GameServer::Initialize()
	{
		const string addr = "tcp://127.0.0.1:3306";
		cout << "Initializing datasource: " << addr << endl;
		this->userDAO = new MySqlUserDAO(addr, "root", "root");
		if (this->userDAO->Initialized())
		{
			cout << "Datasource initialization successful." << endl;
		}
		else
		{
			cout << "ERROR: datasource initialization failed." << endl;
			delete this->userDAO;
			this->userDAO = new NullUserDAO();
		}

		this->objectIdProvider = new MySqlEntityStateLoader(addr, "root", "root");
		if (this->objectIdProvider->isInitialize())
		{
			cout << "Datasource object id provider initialization successful." << endl;
		}
		else
		{
			cout << "ERROR: datasource initialization failed." << endl;
			delete this->objectIdProvider;
			this->objectIdProvider = nullptr;
		}
		this->initializeSimulation();
		this->simulation->SetObjectIdProvider(this->objectIdProvider);
		this->initializeNetwork();
	}

	void GameServer::initializeNetwork()
	{
		this->networkServer = new UDPServer(this->userDAO, &this->packetHolder);
		this->networkServer->InitializeNetwork();

		this->tcpServer = new TCPServer(this->userDAO, &this->packetHolder);
		this->tcpServer->InitializeNetwork();
	}

	void GameServer::initializeSimulation()
	{
		this->simulation = new SpaceGameSimulation();
		this->simulation->Initialize();
		this->simulation->InitializeWorld();
	}

	void GameServer::updateClients()
	{
		vector<World::dl> serializedObjectData;
		this->simulation->GetWorld()->SerializeObjects(serializedObjectData, PACKET_SIZE);

		for (const World::dl serializedData : serializedObjectData)
		{
			this->networkServer->SendObjectUpdate(serializedData.data, serializedData.len);
			this->tcpServer->SendObjectUpdate(serializedData.data, serializedData.len);
			delete[] serializedData.data;
		}
	}

	void GameServer::handleClientMessages()
	{
		vector<ClientMessage> clientMessages;
		this->packetHolder.getAllPackets(clientMessages);

		for (ClientMessage& message : clientMessages)
		{
			const char* const data = message.data();
			const ClientActionType actionType = GetClientActionTypeFromMessage(data);

			switch (actionType)
			{
			case ClientActionType::SIMULATION_COMMAND:
			{
				this->simulation->ReceiveCommand(message.client->GetGlobalID(), GetClientActionDataPointer(data));
			}
			break;
			case ClientActionType::INITIALIZE:
			{
				this->simulation->InitializeNewPlayer(message.client->GetGlobalID());
			}
			break;
			case ClientActionType::JOIN:
			{
				this->simulation->joinPlayer(message.client->GetGlobalID());
			}
			break;
			case ClientActionType::LEAVE:
			{
				this->simulation->leavePlayer(message.clientId);
			}
			break;
			}

			message.clear();
		}
	}
}