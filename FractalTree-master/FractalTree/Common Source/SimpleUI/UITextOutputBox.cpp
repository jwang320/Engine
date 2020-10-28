#include "pch.h"
#include "UITextOutputBox.h"

using namespace DirectX;

namespace SimpleUI
{
	UITextOutputBox::UITextOutputBox(Window* parentWindow, int maxLineCount, float lineSeparation) :
		UIElement(parentWindow, nullptr, false, false, true),
		maxLineCount(maxLineCount),
		lineSeparation(lineSeparation)
	{

	}

	UITextOutputBox& UITextOutputBox::operator<<(const std::string& line)
	{
		if(this->textLabels.size() == 0)
		{
			this->PrintLine(line);
		}
		else
		{
			this->textLabels[this->textLabels.size() - 1].text+= line;
		}
		return (*this);
	}

	UITextOutputBox& UITextOutputBox::operator<<(ConsoleParameters parameters)
	{
// 		if(this->textLabels.size() == 0)
// 		{
// 			this->PrintLine(line);
// 		}
		switch (parameters)
		{
		case SimpleUI::endl:
			this->PrintLine("");
			break;
		default:
			break;
		}
		return (*this);
	}

	UITextOutputBox& UITextOutputBox::operator<<(int line)
	{
		return *this << std::to_string(line);
	}

	UITextOutputBox& UITextOutputBox::operator<<(float line)
	{
		return *this << std::to_string(line);
	}

	void UITextOutputBox::PrintLine(const std::string& line)
	{
		UILabel newLabel = {line, XMVectorSet(0, this->lineSeparation * this->textLabels.size(), 0, 1)};
		this->textLabels.push_back(newLabel);
		if(this->textLabels.size() > this->maxLineCount)
		{
			this->shiftTextDown();
		}
	}

	void UITextOutputBox::shiftTextDown()
	{
		for(unsigned int a = 1; a < this->textLabels.size(); ++a)
		{
			this->textLabels[a].offset = this->textLabels[a - 1].offset;
		}

		this->textLabels.erase(textLabels.begin());
	}
}