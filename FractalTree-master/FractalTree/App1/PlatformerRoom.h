#pragma once
#include "GameResources.h"

class PlatformerRoom
{
public:
	struct ExitDefinition
	{
		enum Direction
		{
			UP,
			DOWN,
			LEFT,
			RIGHT
		};

		Direction direction;
		int exitStart;
		int exitEnd;
	};

private:
	std::vector<ExitDefinition> exits;
	std::vector<BlockCoordinate> blockList;
	int width;
	int height;

public:
	PlatformerRoom(const RoomModel& roomModel);
	const std::vector<BlockCoordinate>& GetBlockList();

private:
	void initialize(const RoomModel& roomModel);
};