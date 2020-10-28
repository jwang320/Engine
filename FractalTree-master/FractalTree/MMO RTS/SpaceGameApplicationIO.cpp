#include "pch.h"
#include "SpaceGameApplication.h"
#include "SpaceGameWindow.h"
#include "..\SpaceGame Common\Core\SpaceGameSimulation.h"
#include "SpaceGameRenderer.h"

using namespace SpaceGameCore;
using namespace SpaceGameClient;

void SpaceGameApplication::KeyPressed(short key, bool shiftDown)
{
	if (!this->window->KeyDown(key))
	{
		if (this->window->SetKeyDown(key, shiftDown) || this->simulation == nullptr)
		{
			return;
		}

		switch (key)
		{
		case '1':
			break;

		case '2':
			break;

		case '3':
			break;

		case '4':
			break;

		case '5':
			break;

		case '6':
			break;

		case '7':
			break;

		case '8':
			break;

		case '0':
			break;

		case '9':
			break;

		case 'A':
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeLeft, SimulationEngine::ToggleOn, 0, 0, this->getFocusObject()));
			break;

		case 'B':
		{
		}
		break;

		case 'C':

		{
		}
		break;

		case 'D':
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeRight, SimulationEngine::ToggleOn, 0, 0, this->getFocusObject()));
			break;

		case 'E':
			break;

		case 'F':
			break;

		case 'G':
			break;

		case 'H':
			break;

		case 'I':
			break;

		case 'J':
			break;

		case 'K':
			break;

		case 'L':
			break;

		case 'M':
			break;

		case 'N':
			break;

		case 'O':
			break;

		case 'Q':
			break;

		case 'R':
			this->renderer->ToggleWireFrame();
			break;

		case 'S':
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeBackward, SimulationEngine::ToggleOn, 0, 0, this->getFocusObject()));
			break;

		case 'T':
			break;

		case 'U':
			break;

		case 'V':
			break;

		case 'W':
		{
			if (this->simulation == NULL)
				break;
			const int focusObjectId = this->getFocusObject();
			//this->simulation->SendCommandPlayer(this->playerID, SimulationEngine::Command(SimulationEngine::StrafeFoward, SimulationEngine::ToggleOn));
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeFoward, SimulationEngine::ToggleOn, 0, 0, focusObjectId));

		}
		break;

		case 'X':
			break;

		case 'Y':
			break;

		case 'Z':
			break;

		case 27:
		{
			if (this->simulation == nullptr)
				break;
			this->sendCommand(SimulationEngine::Command(SimulationEngine::Jump, SimulationEngine::ToggleOff, 0, 0, this->getFocusObject()));
		}
			break;
		}
	}
}

void SpaceGameApplication::KeyReleased(short key)
{
	if (this->window->KeyDown(key) && this->simulation != nullptr)
	{
		switch (key)
		{
		case 'W':
		{
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeFoward, SimulationEngine::ToggleOff, 0, 0, this->getFocusObject()));
		}
		break;
		case 'S':
		{
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeBackward, SimulationEngine::ToggleOff, 0, 0, this->getFocusObject()));
			{
			}
		}
		break;
		case 'A':
		{
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeLeft, SimulationEngine::ToggleOff, 0, 0, this->getFocusObject()));
			if (this->window->KeyDown('D'))
			{
			}
		}
		break;
		case 'D':
		{
			this->sendCommand(SimulationEngine::Command(SimulationEngine::StrafeRight, SimulationEngine::ToggleOff, 0, 0, this->getFocusObject()));
			if (this->window->KeyDown('A'))
			{
			}
		}
		break;
		case 'E':
			break;
		case 'G':
			break;
		case 'Q':
			break;
		case 27:
		{
		}
		break;
		}
	}

	this->window->SetKeyUp(key);
}

void SpaceGameApplication::MouseMoved(float dx, float dy)
{
	using namespace SimulationEngine;
	if (this->simulation == nullptr)
	{
		this->window->mouseMove(dx, dy);
		return;
	}
	if (!this->window->mouseMove(dx, dy))
		this->sendCommand(Command(Look, 0, dx, dy, this->getFocusObject()), true);
}

void SpaceGameApplication::MouseDown(short button, float x, float y)
{
	switch (button)
	{
	case 1:
	{
		
	}
	break;
	case 0:
	{
	}
	break;
	case 2:
	{
		
	}
	break;
	}
}

void SpaceGameApplication::MouseUp(short button)
{
	switch (button)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		break;
	}
}

