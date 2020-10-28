#include "pch.h"
#include "UILayoutElement.h"

using namespace DirectX;

namespace SimpleUI
{
	UILayoutElement::UILayoutElement(Window* parentWindow, const UIContainerLayout& layout,
		OnUseCallback onUseCallback,
		bool isFocusable,
		bool isClickable,
		bool isRenderable) : 
		UIElement(parentWindow, onUseCallback, isFocusable, isClickable, isRenderable),
		layout(layout)
	{

	}

	unsigned int UILayoutElement::AddControlLayout(UIElement* control, LayoutDirection layoutDirection)
	{
		XMVECTOR offsetDir = this->layout.offsetDirection;

		switch(layoutDirection)
		{
		case LayoutDirection::LEFT:
			offsetDir = XMVectorSet(-1, 0, 0, 0);
			break;
		case LayoutDirection::RIGHT:
			offsetDir = XMVectorSet(1, 0, 0, 0);
			break;
		case LayoutDirection::UP:
			offsetDir = XMVectorSet(0, 1, 0, 0);
			break;
		case LayoutDirection::DOWN:
			offsetDir = XMVectorSet(0, -1, 0, 0);
			break;
		}

		XMVECTOR newPosition = this->layout.basePosition;
		newPosition = XMVectorAdd(newPosition, XMVectorMultiply(offsetDir, XMVectorAbs(XMVectorMultiply(XMVector3Dot(this->layout.elementSize, this->layout.offsetDirection), XMVectorReplicate(static_cast<float>(this->children.size()))))));
		newPosition = XMVectorAdd(newPosition, XMVectorMultiply(offsetDir, XMVectorReplicate(abs(this->layout.offsetMagnitude * this->children.size()))));

		control->SetPosition(newPosition);
		control->SetBounds(this->layout.elementSize);
		control->SetForegroundTint(this->layout.elementColor);

		return this->AddChild(control);
	}
}