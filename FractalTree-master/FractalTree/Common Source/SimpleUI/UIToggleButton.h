#pragma once
#include "UIElement.h"

namespace SimpleUI
{
	class UIToggleButton : public UIElement
	{
	protected:
		bool toggled;
		DirectX::XMVECTOR toggledColor;

	public:
		UIToggleButton(Window* parentWindow, OnUseCallback = nullptr);
		virtual void Use(int activationMethod) override;
		~UIToggleButton();

		void SetToggled(bool toggled);
		void SetToggledColor(const DirectX::XMVECTOR& toggledColor) {this->toggledColor = toggledColor;}
	};
}