//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
class Calculator;
namespace RPNCalcPlus
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		bool shiftDown;
		bool clearNext;
		Calculator* calculator;
		void Click1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Click2(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Click3(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Click4(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Number5_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Number6_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Number7_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Number8_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Number9_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void EqualsButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void PlusButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Number0_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void UpdateTextBox(double newInput);
		void Grid_KeyDown_1(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void AddDigit(double d);
		void AddString(Platform::String^ s);
		double GetTextNumber();
		void UpdateStackView();
		void SinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MinusButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MultiplicationButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DivisionButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DecimalButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void NegateButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SquareButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SquareRootButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void RootButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void PowerButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void CosButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void InverseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ASinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ACosButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ATanButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void UnaryClick(double(unaryFunc()));
		void BinaryClick(double(binaryFunc()));
		void EqualsButton_LostFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MainGrid_GotFocus(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ViewLandScape_PointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void ApplicationViewStates_CurrentStateChanged(Platform::Object^ sender, Windows::UI::Xaml::VisualStateChangedEventArgs^ e);
		void thePage_SizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void DeleteButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MainGrid_KeyUp(Platform::Object^ sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs^ e);
		void Exp10Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Exponential_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Log10Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void NaturalLogButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void piebutton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void EButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
