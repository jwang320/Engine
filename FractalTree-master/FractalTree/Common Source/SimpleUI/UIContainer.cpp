#include "pch.h"
#include "UIContainer.h"

using namespace DirectX;

namespace SimpleUI
{
	UIControlContainer::UIControlContainer(Window* parentWindow, const UIContainerLayout& layout) : 
		UILayoutElement(parentWindow, layout, NULL, false, false, true),
		focusElementIndex(-1),
		onFocusChangeCallback(NULL)
	{

	}

	bool UIControlContainer::IsElementFocused(UIElement* element)
	{
		return element == this->children[focusElementIndex];
	}

	void UIControlContainer::DecrementFocus()
	{
		for(int newFocusIndex = this->focusElementIndex - 1; newFocusIndex != this->focusElementIndex; --newFocusIndex)
		{
			if(newFocusIndex < 0)
			{
				newFocusIndex = static_cast<int>(this->children.size()) - 1;
			}

			if(this->children[newFocusIndex]->IsFocusable())
			{
				this->SetFocusElement(newFocusIndex);
				break;
			}
		}
	}

	void UIControlContainer::IncrementFocus()
	{
		for(int newFocusIndex = this->focusElementIndex + 1; newFocusIndex != this->focusElementIndex; ++newFocusIndex)
		{
			if(newFocusIndex > this->children.size() - 1)
			{
				newFocusIndex = 0;
			}
			
			if(newFocusIndex != this->focusElementIndex && this->children[newFocusIndex]->IsFocusable())
			{
				this->SetFocusElement(newFocusIndex);
				break;
			}
		}
	}

	bool UIControlContainer::OnKeyPressed(unsigned short whichKey, bool shiftDown)
	{
		bool handled = __super::OnKeyPressed(whichKey, shiftDown);
		if (handled)
			return handled;

		if (this->passThroughInputElement != NULL)
		{
			return this->passThroughInputElement->OnKeyPressed(whichKey, shiftDown);
		}

		switch(whichKey)
		{
		case KEY_ENTER:
			{
				if(this->focusElementIndex > -1 && this->focusElementIndex < this->children.size())
				{
					UIElement* controlElement = this->children[this->focusElementIndex];
					if(controlElement->GetOnUseCallback() != NULL)
					{
						controlElement->Use(0);
					}
				}

				handled = true;
			}
			break;
		case KEY_DOWNARROW:
			{
				if(this->focusElementIndex > -1)
				{
					this->IncrementFocus();
				}

				handled = true;
			}
			break;
		case KEY_UPARROW:
			{
				if(this->focusElementIndex > -1)
				{
					this->DecrementFocus();
				}

				handled = true;
			}
			break;
		default:
			if (this->focusElementIndex >= 0)
				handled = this->children[this->focusElementIndex]->OnKeyPressed(whichKey, shiftDown);
			break;
		}

		return handled;
	}

	void UIControlContainer::SetFocusElement(int focusElementIndex)
	{
		if(this->focusElementIndex > -1)
		{
			this->children[this->focusElementIndex]->SetFocused(false);
		}

		this->focusElementIndex = focusElementIndex;
		if (focusElementIndex > -1) 
			this->children[this->focusElementIndex]->SetFocused(true);
		if (this->onFocusChangeCallback != NULL)
		{
			this->onFocusChangeCallback(this, 0, this->parentWindow);
		}
	}

	UIControlContainer::~UIControlContainer()
	{

	}
}
