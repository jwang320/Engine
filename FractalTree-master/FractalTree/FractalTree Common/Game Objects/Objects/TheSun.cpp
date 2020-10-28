#include "pch.h"
#include "TheSun.h"
#include "..\Common Source\Rendering Engine\Rendering Engine Standard\StandardRendererResources.h"

using namespace DirectX;

namespace SimulationEngine
{
	TheSun::TheSun(const ObjectParameters& objectParameters, const MoveableParameters& moveableParameters) :
		Actor(objectParameters, moveableParameters),
		posAng(0)
	{
		this->scale = XMVectorSet(1500, 1500, 1500, 1);
		this->location = XMVectorSet(0, 40000, 0, 0);
		this->objectType = ObjectTypes::TheSun;
	}

	void TheSun::Act()
	{
		this->location = XMVector3Transform(XMVectorSet(0, 40000, 0, 0), XMMatrixRotationX(this->posAng * 3.14159265359f / 180));

		if(StandardRendererResources::rotateSun)
		{
			this->posAng+= .55f;
			if(this->posAng >= 360)
			{
				this->posAng = 0;
			}
		}
	}

}