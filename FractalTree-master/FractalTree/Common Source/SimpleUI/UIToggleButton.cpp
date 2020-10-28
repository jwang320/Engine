#include "pch.h"
#include "UIToggleButton.h"

using namespace DirectX;

namespace SimpleUI
{
	UIToggleButton::UIToggleButton(Window* parentWindow, OnUseCallback onUseCallback) :
		UIElement(parentWindow, onUseCallback, true, true, true),
		toggled(false), toggledColor(XMVectorSet(0, 1, 0, 1))
	{

	}

	void UIToggleButton::Use(int activationMethod)
	{
		this->SetToggled(!this->toggled);
		__super::Use(activationMethod);
	}

	void UIToggleButton::SetToggled(bool toggled)
	{
		this->toggled = toggled;
		if(this->toggled)
		{
			this->currentForegroundTint = this->toggledColor;
		}
		else
		{
			this->currentForegroundTint = this->foregroundTint;
		}
	}

	UIToggleButton::~UIToggleButton()
	{

	}
}