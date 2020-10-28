#include "pch.h"
#include "SimulationEvent.h"

namespace SimulationEngine
{
	void WorldEvent::GetSerializedWorldEvent(const WorldEvent& worldEvent, char* data)
	{
		int currentIndex = 0;
		
		memcpy(data, &worldEvent.eventType, sizeof(worldEvent.eventType));
		currentIndex+= sizeof(worldEvent.eventType);

		memcpy(data + currentIndex, &worldEvent.objectID, sizeof(worldEvent.objectID));
		currentIndex+= sizeof(worldEvent.objectID);

		memcpy(data + currentIndex, &worldEvent.eventID, sizeof(worldEvent.eventID));
	}

	void WorldEvent::DeserializeWorldEvent(const char* data, WorldEvent& worldEvent)
	{
		int currentIndex = 0;

		memcpy(&worldEvent.eventType, data, sizeof(worldEvent.eventType));
		currentIndex+= sizeof(worldEvent.eventType);

		memcpy(&worldEvent.objectID, data + currentIndex, sizeof(worldEvent.objectID));
		currentIndex+= sizeof(worldEvent.objectID);

		memcpy(&worldEvent.eventID, data + currentIndex, sizeof(worldEvent.eventID));
		currentIndex+= sizeof(worldEvent.eventID);
	}
}