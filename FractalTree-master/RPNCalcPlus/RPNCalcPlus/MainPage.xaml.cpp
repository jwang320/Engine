//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Calculator.h"
#include <deque>

using namespace RPNCalcPlus;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage() : calculator(new Calculator), clearNext(false)
{
	InitializeComponent();
	StackPanel^ stackPanel = ref new StackPanel;
	this->StackViewWindow->Content = stackPanel;
	this->StackViewWindow->VerticalScrollBarVisibility = ScrollBarVisibility::Visible;
	this->StackViewWindow->FlowDirection = Windows::UI::Xaml::FlowDirection::RightToLeft;
	this->ErrorBox->Text = "";
	//VisualStateManager::Go
	
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void MainPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter

	
}

void RPNCalcPlus::MainPage::UpdateTextBox(double newInput)
{
	this->calculator->PushNumber(newInput);
	this->ErrorBox->Text = "";
}

void RPNCalcPlus::MainPage::Click1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(1);
}

void RPNCalcPlus::MainPage::Click2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(2);
}

void RPNCalcPlus::MainPage::Click3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(3);
}

void RPNCalcPlus::MainPage::Click4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(4);
}

void RPNCalcPlus::MainPage::Number5_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(5);
}

void RPNCalcPlus::MainPage::Number6_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(6);
}

void RPNCalcPlus::MainPage::Number7_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(7);
}

void RPNCalcPlus::MainPage::Number8_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(8);
}

void RPNCalcPlus::MainPage::Number9_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(9);
}

void RPNCalcPlus::MainPage::DecimalButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddString(".");
}

double RPNCalcPlus::MainPage::GetTextNumber()
{
	Platform::String^ outputText = this->CalculationOutput->Text;
	auto outW =  outputText->Data();
	double a = _wtof(outW);
	return a;
}

void RPNCalcPlus::MainPage::EqualsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (this->CalculationOutput->Text->Length() == 0)
	{
		return;
	}

	this->UpdateTextBox(this->GetTextNumber());
	this->UpdateStackView();
}

void RPNCalcPlus::MainPage::AddDigit(double d)
{
	if(this->clearNext)
	{
		this->clearNext = false;
		this->CalculationOutput->Text = "";
	}
	this->CalculationOutput->Text = this->CalculationOutput->Text + d.ToString();
}

void RPNCalcPlus::MainPage::AddString(Platform::String^ s)
{
	if(this->clearNext)
	{
		this->clearNext = false;
		this->CalculationOutput->Text = "";
	}
	this->CalculationOutput->Text = this->CalculationOutput->Text + s;
}

void RPNCalcPlus::MainPage::UpdateStackView()
{
	this->CalculationOutput->Text = "";

	std::deque<double> stackView = this->calculator->calcStack._Get_container();

	StackPanel^ stackPanel = ((StackPanel^)this->StackViewWindow->Content);
	
	stackPanel->Children->Clear();

	for(unsigned int a = 0; a < stackView.size(); ++a)
	{
		double val = stackView[a];
		TextBlock^ block = ref new TextBlock;

		if (val < 0)
		{
			block->Text = abs(val).ToString() + "-";
		}
		else
		{
			block->Text = val.ToString();
		}

		block->FontSize = 36;
		block->TextAlignment = TextAlignment::Left;
		stackPanel->Children->Append(block);
	}
	
	TextBlock^ block = ref new TextBlock;
	block->FontSize = 36;
	stackPanel->Children->Append(block);

	this->StackViewWindow->VerticalScrollBarVisibility = ScrollBarVisibility::Hidden;
	this->StackViewWindow->ScrollToVerticalOffset(this->StackViewWindow->ScrollableHeight);
	this->StackViewWindow->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
}

void RPNCalcPlus::MainPage::Number0_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->AddDigit(0);
}

void RPNCalcPlus::MainPage::Grid_KeyDown_1(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e)
{
	switch (e->Key)
	{
	case Windows::System::VirtualKey::Number1:
		{
			this->Click1(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number2:
		{
			this->Click2(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number3:
		{
			this->Click3(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number4:
		{
			this->Click4(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number5:
		{
			this->Number5_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number6:
		{
			this->Number6_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number7:
		{
			this->Number7_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number8:
		{
			this->Number8_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number9:
		{
			this->Number9_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Number0:
		{
			this->Number0_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Enter:
		{
			this->EqualsButton_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Decimal:
		{
			this->DecimalButton_Click(this->Number1, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Divide:
		{
			this->DivisionButton_Click(nullptr, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Add:
		{
			this->PlusButton_Click(nullptr, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Multiply:
		{
			this->MultiplicationButton_Click(nullptr, nullptr);
		}
		break;
	case Windows::System::VirtualKey::Subtract:
		{
			this->MinusButton_Click(nullptr, nullptr);
		}
		break;
	default:
		break;
	}
	e->Handled = true;
}

void RPNCalcPlus::MainPage::PlusButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendPlus);
}

void RPNCalcPlus::MainPage::MinusButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendMinus);
}

void RPNCalcPlus::MainPage::MultiplicationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendMult);
}

void RPNCalcPlus::MainPage::DivisionButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendDiv);
}

void RPNCalcPlus::MainPage::NegateButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendNegate);
}

void RPNCalcPlus::MainPage::InverseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendInverse);
}

void RPNCalcPlus::MainPage::SquareButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendSquare);
}

void RPNCalcPlus::MainPage::SquareRootButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendSqrt);
}

void RPNCalcPlus::MainPage::RootButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendRoot);
}

void RPNCalcPlus::MainPage::PowerButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendPow);
}

void RPNCalcPlus::MainPage::SinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendSine);
}

void RPNCalcPlus::MainPage::CosButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendCosine);
}

void RPNCalcPlus::MainPage::TanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendTangent);
}

void RPNCalcPlus::MainPage::ASinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendArcsine);
}

void RPNCalcPlus::MainPage::ACosButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendArccosine);
}

void RPNCalcPlus::MainPage::ATanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	BinaryClick(this->calculator->SendArctangent);
}

void RPNCalcPlus::MainPage::BinaryClick(double(binaryFunc()))
{
	if(this->CalculationOutput->Text->Length())
	{
		this->UpdateTextBox(this->GetTextNumber());
	}

	try
	{
		double newFront = binaryFunc();
		this->UpdateStackView();
	}
	catch(const InvalidOperation& e)
	{
		std::wstring strEr;
		strEr.assign(e.issue, e.issue + strlen(e.issue));
		this->ErrorBox->Text = ref new Platform::String(strEr.c_str());

		if (this->CalculationOutput->Text->Length() != 0)
		{
			this->calculator->PopNumber();
		}
	}
}



void RPNCalcPlus::MainPage::ViewLandScape_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
{
	this->EqualsButton->Focus(Windows::UI::Xaml::FocusState::Programmatic);
	e->Handled = true;
}


void RPNCalcPlus::MainPage::ApplicationViewStates_CurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::VisualStateChangedEventArgs^ e)
{
	//this->ApplicationViewStates->
	if(e->NewState == Snapped)
	{
		//Application::Current->Current->
		int x = 0;
		++x;
	}
}


void RPNCalcPlus::MainPage::thePage_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
{
	Windows::UI::ViewManagement::ApplicationViewState currentState = Windows::UI::ViewManagement::ApplicationView::Value;
	switch (currentState)
	{
	case Windows::UI::ViewManagement::ApplicationViewState::FullScreenLandscape:
		if(this->ApplicationViewStates->CurrentState != FullScreenLandscape)
		{
			Windows::UI::Xaml::VisualStateManager::GoToState(this, "FullScreenLandscape", false);
		}
		break;
	case Windows::UI::ViewManagement::ApplicationViewState::Filled:
		if(this->ApplicationViewStates->CurrentState != Filled)
		{
			Windows::UI::Xaml::VisualStateManager::GoToState(this, "Filled", false);
		}
		break;
	case Windows::UI::ViewManagement::ApplicationViewState::Snapped:
		if(this->ApplicationViewStates->CurrentState != Snapped)
		{
			Windows::UI::Xaml::VisualStateManager::GoToState(this, "Snapped", false);
		}
		break;
	case Windows::UI::ViewManagement::ApplicationViewState::FullScreenPortrait:
		if(this->ApplicationViewStates->CurrentState != FullScreenPortrait)
		{
			Windows::UI::Xaml::VisualStateManager::GoToState(this, "FullScreenPortrait", false);
		}
		break;
	default:
		break;
	}

	this->EqualsButton->Focus(Windows::UI::Xaml::FocusState::Programmatic);
}
