// FractalTreeServer.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "Server/UDPServer.h"
#include "Server/GameServer.h"
#include <thread>
#include <iostream>

using namespace Server;
using namespace Network;
using namespace std;

int networkTickRate = 64;
int worldType = 0;
bool UseTerrain = true;

void ServerThread()
{
	UDPServer* server = new UDPServer("8890");
	server->InitializeNetwork();
	GameServer* gameServer = new GameServer(server);
	gameServer->Initialize();

	gameServer->Update();
}

int main(int argc, char* argv[])
{	
	thread serverThread;
	while(true)
	{
		string input;
		cin >> input;
		
		if(input.compare("world_type") == 0)
		{
			cin >> worldType;
			if(worldType > 0)
			{
				UseTerrain = false;
			}
		}
		else if(input.compare("start") == 0)
		{
			serverThread = thread(&ServerThread);
			serverThread.detach();
		}		
		else if(input.compare("octave") == 0)
		{
			int octave;
			cin >> octave;
			--octave;
			if(octave < 8 && octave >= 0)
			{
				if(!FractalTreeResources::octaveEnabled[octave])
				{
					cout << "Octave Enabled" << octave << endl;
					if(octave == 7)
					{
						cout << "Mountain Enabled" << endl;
					}

					FractalTreeResources::octaveEnabled[octave] = true;
				}
				else
				{
					cout << "Octave Disabled" << octave << endl;
					FractalTreeResources::octaveEnabled[octave] = false;
				}

				
			}
		}
		if(input.compare("network_tick") == 0)
		{
			cin >> networkTickRate;
// 			if(worldType > 0)
// 			{
// 				UseTerrain = false;
// 			}
		}
	}

	return 0;
}

