#pragma once
#include "UILayoutElement.h"

namespace SimpleUI
{
	class UIControlContainer : public UILayoutElement
	{
	protected:
		int focusElementIndex;
		OnUseCallback onFocusChangeCallback;

	public:
		UIControlContainer(Window* parentWindow, const UIContainerLayout& layout = UIContainerLayout());
		virtual bool OnKeyPressed(unsigned short whichKey, bool shiftDown = false) override;
		void SetFocusElement(int focusElementIndex);
		bool IsElementFocused(UIElement* element);
		void UIControlContainer::DecrementFocus();
		void UIControlContainer::IncrementFocus();
		UIElement* GetFocusElement() const { return this->children[this->focusElementIndex]; }
		int GetFocusElementIndex() const { return this->focusElementIndex; }
		void SetOnFocusChangeCallback(OnUseCallback callBack) { this->onFocusChangeCallback = callBack; }
		~UIControlContainer();
	};
}