#pragma once
#include "UIElement.h"

namespace SimpleUI
{
	enum ConsoleParameters
	{
		endl
	};

	class UITextOutputBox : public UIElement
	{
	protected:
		int maxLineCount;
		float lineSeparation;

	public:
		UITextOutputBox(Window* parentWindow, int maxLineCount = 5, float lineSeparation = -.09f);

		void PrintLine(const std::string& line);
		void SetMaxLineCount(int maxLineCount) {this->maxLineCount = maxLineCount;}

		UITextOutputBox& operator<<(const std::string& line);
		UITextOutputBox& operator<<(float line);
		UITextOutputBox& operator<<(int line);
		UITextOutputBox& operator<<(ConsoleParameters parameters);
	private:
		void shiftTextDown();
	};
}