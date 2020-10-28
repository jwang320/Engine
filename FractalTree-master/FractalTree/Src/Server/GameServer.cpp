#include "pch.h"
#include "GameServer.h"
#include "FractalTreeSimulation.h"
#include "UDPServer.h"
#include <iostream>
#include "Client.h"
#include "Humanoid.h"

using namespace SimulationEngine;
using namespace Network;
extern double performanceFrequency;
extern int networkTickRate;
extern bool UseTerrain;

namespace Server
{
	GameServer::GameServer(UDPServer* networkServer) :
		networkServer(networkServer),
		lastSentEvent(0)
	{

	}

	void GameServer::Update()
	{
		using namespace DirectX;

		bool newFrame = true;
		bool newNetworkFrame = true;
		_LARGE_INTEGER frameStart;
		frameStart.QuadPart = 0LL;

		_LARGE_INTEGER networkFrameStart;
		networkFrameStart.QuadPart = 0LL;

		_LARGE_INTEGER perfFreq;
		QueryPerformanceFrequency(&perfFreq);
		performanceFrequency = perfFreq.QuadPart / 1000.0;

		while(true)
		{
			if(newFrame)
			{
				newFrame = false;
				QueryPerformanceCounter(&frameStart);
			}

			if(newNetworkFrame)
			{
				newNetworkFrame = false;
				QueryPerformanceCounter(&networkFrameStart);
			}
			LARGE_INTEGER currentTick;
			QueryPerformanceCounter(&currentTick);
			const double elapsedTime = (currentTick.QuadPart - frameStart.QuadPart) / performanceFrequency;
			const double elapsedNetworkTime = (currentTick.QuadPart - networkFrameStart.QuadPart) / performanceFrequency;
			if(elapsedTime >= 16)
			{
				Packet receivedPacket;
				while(this->networkServer->ReceivePacket(receivedPacket) >= 0)
				{
					PacketType packetType = UDPNetworkBase::GetPacketType(receivedPacket);
					int currentIndex = sizeof(packetType);

					switch (packetType)
					{
					case PacketType::Ack:
						break;
					case PacketType::ConnectionRequest:
						{
							Client* newClient = this->networkServer->CreateClient(receivedPacket);
							if(newClient != NULL)
							{
								this->simulation->CreatePlayer(newClient->GetGlobalID());
							}
							
							break;
						}
					case PacketType::ProgramMessage:
						{
							this->handleProgramMessage(receivedPacket);
						}
						break;
					default:
						break;
					}
				}

				this->simulation->Update(1);
				newFrame = true;

				if(elapsedNetworkTime >= networkTickRate)
				{
					this->sendEventData();
					this->sendObjectData();
					newNetworkFrame = true;
				}
			}
			else
			{
				Sleep(1);
			}

			//std::cout << "Elapsed Time: " << elapsedTime << std::endl;
		}
	}

	void GameServer::Initialize()
	{
		this->simulation = new FractalTreeSimulation();
		this->simulation->Initialize();
		this->simulation->InitializeWorld(UseTerrain);
	}

	void GameServer::handleProgramMessage(const Packet& receivedPacket)
	{
		int currentIndex = sizeof(PacketType);

		ProgramMessageType programMessageType;
		memcpy(&programMessageType, receivedPacket.data + currentIndex, sizeof(programMessageType));
		currentIndex+= sizeof(programMessageType);

		switch (programMessageType)
		{
		case ProgramMessageType::AssignPlayerObject:
			{
				int playerID;
				memcpy(&playerID, receivedPacket.data + sizeof(char)*2, sizeof(playerID));

				int currentFocusObject = this->simulation->GetPlayer(playerID)->GetFocusObject();
				if(this->simulation->DoesObjectExist(currentFocusObject))
				{
				}
				else
				{
					Humanoid* testHumanoid = new Humanoid(ObjectParameters(this->simulation->GetSimulationWorld()), MoveableParameters(DirectX::XMVectorSet(-20, 0, -20, 0)));
					currentFocusObject = testHumanoid->GetGlobalID();
					this->simulation->GetPlayer(playerID)->SetFocusObject(currentFocusObject);
				}

				char data[256];
				const char messageType = static_cast<const char>(Network::PacketType::ProgramMessage);
				const char updateType = static_cast<const char>(ProgramMessageType::AssignPlayerObject);
				memcpy(data, &messageType, sizeof(messageType));
				memcpy(data + sizeof(messageType), &updateType, sizeof(updateType));
				const int headerSize = sizeof(messageType) + sizeof(updateType);
				memcpy(data + headerSize, &currentFocusObject, sizeof(currentFocusObject));

				this->networkServer->SendPacketToClients(data, headerSize + sizeof(int));							
			}
			break;

		case ProgramMessageType::UserInput:
			{
				int playerID;
				CommandName commandName;
				int commandFlags;
				float arg1;
				float arg2;

				memcpy(&playerID, receivedPacket.data + currentIndex, sizeof(playerID));
				currentIndex+= sizeof(playerID);

				memcpy(&commandName, receivedPacket.data + currentIndex, sizeof(commandName));
				currentIndex+= sizeof(commandName);

				memcpy(&commandFlags, receivedPacket.data + currentIndex, sizeof(commandFlags));
				currentIndex+= sizeof(commandFlags);

				memcpy(&arg1, receivedPacket.data + currentIndex, sizeof(arg1));
				currentIndex+= sizeof(arg1);

				memcpy(&arg2, receivedPacket.data + currentIndex, sizeof(arg2));
				currentIndex+= sizeof(arg2);

				this->simulation->SendCommandPlayer(playerID, Command(commandName, commandFlags, arg1, arg2));
			}
			break;

		case ProgramMessageType::RequestObject:
			{
				int playerID;
				int objectID;

				memcpy(&playerID, receivedPacket.data + currentIndex, sizeof(playerID));
				currentIndex+= sizeof(playerID);

				memcpy(&objectID, receivedPacket.data + currentIndex, sizeof(objectID));
				currentIndex+= sizeof(objectID);

				char data[PACKET_SIZE];

				const char messageType = static_cast<const char>(PacketType::ProgramMessage);
				const char updateType = static_cast<const char>(ProgramMessageType::CreateObject);

				int bytesUsed = 0;
				memcpy(data, &messageType, sizeof(messageType));
				bytesUsed+= sizeof(messageType);

				memcpy(data + bytesUsed, &updateType, sizeof(updateType));
				bytesUsed+= sizeof(updateType);

				const Object* object = this->simulation->GetWorldObjectByGlobalID(objectID);
				int dataLength = 0;
				object->GetSerializedForm(data + bytesUsed + sizeof(dataLength), dataLength, PACKET_SIZE);
				memcpy(data + bytesUsed, &dataLength, sizeof(dataLength));
				bytesUsed+= dataLength + sizeof(dataLength);
				this->networkServer->SendPacketToClients(data, bytesUsed);
			}
			break;
		default:
			break;
		}
	}

	void GameServer::handleObjectRequest(const Packet& packet)
	{

	}

	void GameServer::sendObjectData()
	{
		const int objectCount = this->simulation->GetObjectCount();

		int a = 0;

		while(a < objectCount)
		{
			char data[PACKET_SIZE];

			const char messageType = static_cast<const char>(PacketType::ProgramMessage);
			const char updateType = static_cast<const char>(ProgramMessageType::UpdateObject);

			int bytesUsed = 0;
			memcpy(data, &messageType, sizeof(messageType));
			bytesUsed+= sizeof(messageType);

			memcpy(data + bytesUsed, &updateType, sizeof(updateType));
			bytesUsed+= sizeof(updateType);

			while(a < objectCount)
			{
				const int globalObjectID = this->simulation->GetObjectGlobalID(a);
				const Object* object = this->simulation->GetWorldObjectByGlobalID(globalObjectID);
				int dataLength = 0;
				object->GetSerializedFormSmall(data + bytesUsed + sizeof(dataLength), dataLength, PACKET_SIZE - bytesUsed - sizeof(dataLength));

				if(dataLength <= 0)
				{
					break;
				}

				memcpy(data + bytesUsed, &dataLength, sizeof(dataLength));
				bytesUsed+= dataLength + sizeof(dataLength);
				++a;
			}
			
			this->networkServer->SendPacketToClients(data, bytesUsed);
		}
	}

	void GameServer::sendEventData()
	{
		const int eventCount = static_cast<int>(this->simulation->GetSimulationWorld()->GetWorldEventList().size());

		while(this->lastSentEvent < eventCount)
		{
			char data[PACKET_SIZE];

			const char messageType = static_cast<const char>(PacketType::ProgramMessage);
			const char updateType = static_cast<const char>(ProgramMessageType::WorldEvent);

			int bytesUsed = 0;
			memcpy(data, &messageType, sizeof(messageType));
			bytesUsed+= sizeof(messageType);

			memcpy(data + bytesUsed, &updateType, sizeof(updateType));
			bytesUsed+= sizeof(updateType);

			while(this->lastSentEvent < eventCount && bytesUsed + sizeof(WorldEvent) < PACKET_SIZE)
			{
				const WorldEvent& nextEvent = this->simulation->GetSimulationWorld()->GetWorldEventList()[this->lastSentEvent];
				WorldEvent::GetSerializedWorldEvent(nextEvent, data + bytesUsed);
				bytesUsed+= sizeof(nextEvent);
				++this->lastSentEvent;			
			}

			this->networkServer->SendPacketToClients(data, bytesUsed);
		}
	}
}