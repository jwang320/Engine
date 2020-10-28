#pragma once

#include "UIElement.h"

namespace SimpleUI
{
	class UIButton : public UIElement
	{
	public:
		UIButton(Window* parentWindow, OnUseCallback = nullptr);
		//virtual bool OnPointerPress(unsigned char whichButton, float x, float y) override;
		virtual bool OnKeyPressed(unsigned short whichKey, bool shiftDown = false) override;
		~UIButton();
	};
}