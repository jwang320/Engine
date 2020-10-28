#pragma once
#include "..\Common Source\SimpleUI\UIElement.h"

namespace SimpleUI
{
	class UIImage : public UIElement
	{
	private:
		int imageWidth;
		int imageHeight;

	public:
		UIImage(Window* parentWindow);
		void Init(int imageWidth, int imageHeight);
		void RefreshImage(unsigned int* image);
		~UIImage();
	};
}