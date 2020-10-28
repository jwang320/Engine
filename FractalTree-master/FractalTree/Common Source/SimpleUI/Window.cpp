#include "pch.h"
#include "Window.h"
#include <DirectXMath.h>
#include "UIElement.h"

using namespace DirectX;

namespace SimpleUI
{
	Window::Window(UIElement* foregroundUIContainer) : 
		foregroundUIContainer(foregroundUIContainer)
	{
		for(unsigned int a = 0; a < 256; ++a)
		{
			this->keyDown[a] = false;
		}
	}

	void Window::SetForegroundUIElement(UIElement* foregroundUIContainer)
	{
		this->showMouse = foregroundUIContainer->IsUsingMouse();
		this->foregroundUIContainer = foregroundUIContainer;
	}

	bool Window::SetKeyDown(unsigned short whichKey, bool shiftDown)
	{
		bool handled = false;
		this->keyDown[whichKey] = true;

		//Check to see if the foreground UI element is capable of handling input.
		//If it is, give the foreground element priority to handle the input.
		//UIInputElement* inputElement = dynamic_cast<UIInputElement*>(this->foregroundUIContainer);

		if(this->foregroundUIContainer != NULL)
		{
			handled = this->foregroundUIContainer->OnKeyPressed(whichKey, shiftDown);
		}

		if(!handled)
		{
			switch (whichKey)
			{
			case 27:
				break;
			default:
				break;
			}
		}

		return handled;
	}

	bool Window::SetPointerDown(unsigned char whichButton, float x, float y)
	{
		if (!this->showMouse)
			return false;

		bool handled = false;
		this->mouseDown[whichButton] = true;

		//Transform to normalized coordinates
		float newX = (this->mouseX + 1) * this->width * .5f;// / (this->width * .5f) - 1.f;
		float newY = (this->mouseY + 1) * this->height * .5f;// / (this->height * .5f) - 1.f;

		if(this->foregroundUIContainer != NULL && this->foregroundUIContainer->IsEnabled())
		{
			const float normalizedX = (this->mouseX ) * .5f;
			const float normalizedY = (this->mouseY ) * .5f;
			XMFLOAT4 resultPoint;
			UIElement* clickedElement = this->foregroundUIContainer->CheckPointerIntersection(XMVectorSet(normalizedX, normalizedY, 0, 1), XMMatrixIdentity(), resultPoint);
			if(clickedElement != NULL)
			{
				XMFLOAT4 pos;
				//XMStoreFloat4(&pos, clickedElement->GetRelativePosition());
				XMStoreFloat4(&pos, XMVector3Transform(XMVectorSet(normalizedX, normalizedY, 0, 1), XMMatrixInverse(nullptr, clickedElement->GetTransform())));
//				float xx = this->mouseX - pos.x;
	//			const float yy = this->mouseY - pos.y;
				handled = clickedElement->OnPointerPress(whichButton, resultPoint.x, resultPoint.y);
			}
		}

		return handled;
	}

	bool Window::mouseMove(float x, float y)
	{
		if (this->showMouse)
		{
			this->mouseX += x*.5;
			this->mouseY += -y*.5;
			this->mouseX = Geometry::clamp(this->mouseX, -1, 1);
			this->mouseY = Geometry::clamp(this->mouseY, -1, 1);
		}

		return this->showMouse;
	}

	Window::~Window()
	{
		
	}

	bool Window::SetKeyUp(unsigned short whichKey)
	{
		this->keyDown[whichKey] = false;

		return true;
	}

}