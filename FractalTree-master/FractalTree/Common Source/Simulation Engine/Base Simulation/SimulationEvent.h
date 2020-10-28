#pragma once

namespace SimulationEngine
{
	enum class WorldEventType : char
	{
		ObjectDeath
	};

	struct WorldEvent
	{
		WorldEventType eventType;
		int objectID;
		int eventID;

		static void GetSerializedWorldEvent(const WorldEvent& worldEvent, char* data);
		static void DeserializeWorldEvent(const char* data, WorldEvent& worldEvent);
	};

}