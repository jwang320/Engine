#pragma once

#include "pch.h"
#include "GameApplication.h"
#include "Common\StepTimer.h"

namespace App1
{
	// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
	ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
	{
	public:
		App();

		// IFrameworkView Methods.
		virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
		virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
		virtual void Load(Platform::String^ entryPoint);
		virtual void Run();
		virtual void Uninitialize();

	protected:
		// Application lifecycle event handlers.
		void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
		void OnResuming(Platform::Object^ sender, Platform::Object^ args);

		// Window event handlers.
		void OnWindowSizeChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ args);
		void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
		void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);

		// DisplayInformation event handlers.
		void OnDpiChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnOrientationChanged(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);
		void OnDisplayContentsInvalidated(Windows::Graphics::Display::DisplayInformation^ sender, Platform::Object^ args);

		void OnPointerPressed(_In_ Windows::UI::Core::CoreWindow^ sender,
			_In_ Windows::UI::Core::PointerEventArgs^ args);

		void OnPointerMoved(
			_In_ Windows::UI::Core::CoreWindow^ sender,
			_In_ Windows::UI::Core::PointerEventArgs^ args);

		void OnPointerReleased(
			_In_ Windows::UI::Core::CoreWindow^,
			_In_ Windows::UI::Core::PointerEventArgs^ args);

		void OnKeyPressed(
			_In_ Windows::UI::Core::CoreWindow^,
			_In_ Windows::UI::Core::KeyEventArgs^ args);

		void OnKeyReleased(
			_In_ Windows::UI::Core::CoreWindow^,
			_In_ Windows::UI::Core::KeyEventArgs^ args);
	internal:
		void OnMouseMoved(_In_ Windows::Devices::Input::MouseDevice^ mouseDevice, _In_ Windows::Devices::Input::MouseEventArgs^ args);

	private:
		GameApplication* john;
		bool m_windowClosed;
		bool m_windowVisible;
		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};
