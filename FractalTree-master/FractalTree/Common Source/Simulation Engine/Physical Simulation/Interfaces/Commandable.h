#pragma once

namespace SimulationEngine
{
	enum CommandName
	{
		StrafeFoward,
		StrafeBackward,
		StrafeLeft,
		StrafeRight,
		Look,
		Jump,
		Shoot,
		Shoot2,
		Shoot3,
		Shoot4,
		RollLeft,
		RollRight
	};

	enum CommandFlags
	{
		ToggleOn = 1,
		ToggleOff = 2
	};

	struct Command
	{
		CommandName command;
		int flags;
		float arg1;
		float arg2;
		int objectId;
		Command(CommandName command, int flags, float arg1 = 0, float arg2 = 0, int objectId = -1) :
			command(command),
			flags(flags),
			arg1(arg1),
			arg2(arg2),
			objectId(objectId)
		{

		}

		Command() {};

		unsigned int GetSerializedForm(char* data) const
		{
			unsigned int dataLength = 0;
			memcpy(data, &this->command, sizeof(CommandName));
			dataLength += sizeof(CommandName);
			memcpy(data + dataLength, &this->flags, sizeof(this->flags));
			dataLength += sizeof(this->flags);
			memcpy(data + dataLength, &this->arg1, sizeof(this->arg1));
			dataLength += sizeof(this->arg1);
			memcpy(data + dataLength, &this->arg2, sizeof(this->arg2));
			dataLength += sizeof(this->arg2);
			memcpy(data + dataLength, &this->objectId, sizeof(this->objectId));
			dataLength += sizeof(this->objectId);
			return dataLength;
		}

		static Command deserialize(const char* const data)
		{
			Command results;
			memcpy(&results, data, sizeof(Command));
			return results;
		}
	};

	//Commandable is an interface class for objects who can receive commands
	class Commandable
	{
	public:
		virtual void ReceiveCommand(const Command& command);
	};
}