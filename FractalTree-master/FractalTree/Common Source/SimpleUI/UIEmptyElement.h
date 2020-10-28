#pragma once
#include "UIElement.h"

namespace SimpleUI
{
	//An empty class meant for holding other elements silently.
	class UIEmptyElement : public UIElement
	{
	public:
		UIEmptyElement(Window* parentWindow);
		~UIEmptyElement();
	};
}