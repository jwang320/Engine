#pragma once
#include "UIElement.h"

namespace SimpleUI
{
	enum LayoutDirection
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		DEFAULT
	};

	struct UIContainerLayout
	{
		DirectX::XMVECTOR basePosition;
		DirectX::XMVECTOR elementSize;
		DirectX::XMVECTOR elementColor;
		DirectX::XMVECTOR offsetDirection;
		float offsetMagnitude;
		UIContainerLayout(const DirectX::XMVECTOR& basePosition = DirectX::XMVectorSet(0, .4f, 0, 0), 
			const DirectX::XMVECTOR& elementSize = DirectX::XMVectorSet(.8f, .1f, 0, 0),
			const DirectX::XMVECTOR& elementColor = DirectX::XMVectorSet(1, 0, 0, 1),
			const DirectX::XMVECTOR& offsetDirection = DirectX::XMVectorSet(0, -1, 0, 0),
			float offsetMagnitude = .01f) 
			: basePosition(basePosition),
			elementSize(elementSize),
			elementColor(elementColor),
			offsetDirection(offsetDirection),
			offsetMagnitude(offsetMagnitude)
		{

		}

	};
	
	class UILayoutElement : public UIElement
	{
	protected:
		UIContainerLayout layout;

	public:
		UILayoutElement(Window* parentWindow, const UIContainerLayout& layout = UIContainerLayout(), 
			OnUseCallback onUseCallback = NULL,
			bool isFocusable = false,
			bool isClickable = false,
			bool isRenderable = true);

		unsigned int AddControlLayout(UIElement* control, LayoutDirection layoutDirection = LayoutDirection::DEFAULT);
		void SetLayout(const UIContainerLayout& layout) {this->layout = layout;}

	};
}