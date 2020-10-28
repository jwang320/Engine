#include "pch.h"
#include "UIButton.h"

namespace SimpleUI
{
	UIButton::UIButton(Window* parentWindow, OnUseCallback onUseCallback) :
		UIElement(parentWindow, onUseCallback, true, true, true)
	{

	}

	/*bool UIButton::OnPointerPress(unsigned char whichButton, float x, float y)
	{
		if(this->onUseCallback != NULL)
		{
			this->Use(2);
		}

		return true;
	}*/

	bool UIButton::OnKeyPressed(unsigned short whichKey, bool shiftDown)
	{
		if(this->IsEditable())
		{
			if(whichKey == 8)
			{
				if(this->textLabels[0].text.size() > this->editableStartIndex)
					this->textLabels[0].text.resize(this->textLabels[0].text.size() - 1);
			}
			else
			{
				if (!shiftDown)
				{
					//whichKey += 32;
				}

				this->textLabels[0].text.append(1, static_cast<char>(whichKey));
			}
			
			return true;
		}

		return false;
	}

	UIButton::~UIButton()
	{

	}
}