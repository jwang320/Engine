#include "pch.h"
#include "App.h"

#include <ppltasks.h>
#include "GameApplication.h"

using namespace App1;

using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
extern double performanceFrequency;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	//Object* testObj = new Object(ObjectParameters(NULL));
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new App();
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

//This function is the main program loop handler.
void App::Run()
{
	while (!m_windowClosed)
	{
		this->john->Update();
		m_timer.Tick([&]()
		{

		});
		CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
	}
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView^ applicationView)
{
	_LARGE_INTEGER perfFreq;
	QueryPerformanceFrequency(&perfFreq);
	performanceFrequency = perfFreq.QuadPart / 1000.0;

	this->john = new GameApplication();
	this->john->Initialize();
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow^ window)
{
	window->SizeChanged += 
		ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

	DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation->DpiChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

	currentDisplayInformation->OrientationChanged +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

	DisplayInformation::DisplayContentsInvalidated +=
		ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

	// Disable all pointer visual feedback for better performance when touching.
	auto pointerVisualizationSettings = PointerVisualizationSettings::GetForCurrentView();
	pointerVisualizationSettings->IsContactFeedbackEnabled = false; 
	pointerVisualizationSettings->IsBarrelButtonFeedbackEnabled = false;

	window->PointerPressed +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(
		this, &App::OnPointerPressed);
	window->PointerReleased +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(
		this, &App::OnPointerReleased);

	window->KeyDown +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(
		this, &App::OnKeyPressed);

	window->KeyUp +=
		ref new Windows::Foundation::TypedEventHandler<
		Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(
		this, &App::OnKeyReleased);
	Windows::Devices::Input::MouseDevice::GetForCurrentView()->MouseMoved +=
		ref new TypedEventHandler<Windows::Devices::Input::MouseDevice^, Windows::Devices::Input::MouseEventArgs^>(this, &App::OnMouseMoved);

	this->john->InitializeRenderer(CoreWindow::GetForCurrentThread());
}

float previousX2 = 0;
float previousY2 = 0;
void App::OnPointerPressed(
	_In_ Windows::UI::Core::CoreWindow^,
	_In_ Windows::UI::Core::PointerEventArgs^ args)
{
	previousX2 = args->CurrentPoint->Position.X;
	previousY2 = args->CurrentPoint->Position.Y;
	unsigned int pointerId = args->CurrentPoint->PointerId;



	//SystemParameters::Activated = true;
	this->john->MouseDown(1, args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
}

void App::OnPointerMoved(
	_In_ Windows::UI::Core::CoreWindow^ sender,
	_In_ Windows::UI::Core::PointerEventArgs^ args)
{
	unsigned int pointerId = args->CurrentPoint->PointerId;

	previousX2 = args->CurrentPoint->Position.X;
	previousY2 = args->CurrentPoint->Position.Y;
	Windows::Foundation::Collections::IVector<Windows::UI::Input::PointerPoint^>^ pointerPoints = Windows::UI::Input::PointerPoint::GetIntermediatePoints(
		pointerId);

}
void App::OnMouseMoved(
	_In_ Windows::Devices::Input::MouseDevice^ mouseDevice,
	_In_ Windows::Devices::Input::MouseEventArgs^ args
	)
{
	this->john->MouseMoved(args->MouseDelta.X * -.01f, args->MouseDelta.Y * .01f);
}
void App::OnPointerReleased(
	_In_ Windows::UI::Core::CoreWindow^,
	_In_ Windows::UI::Core::PointerEventArgs^ args)
{
	unsigned int pointerId = args->CurrentPoint->PointerId;
	Windows::UI::Input::PointerPoint^ pointerPoint = Windows::UI::Input::PointerPoint::GetCurrentPoint(
		pointerId);
	//SystemParameters::Activated = false;
}

void App::OnKeyPressed(_In_ Windows::UI::Core::CoreWindow^, _In_ Windows::UI::Core::KeyEventArgs^ args)
{
	this->john->KeyPressed(static_cast<short>(args->VirtualKey));
}

void App::OnKeyReleased(_In_ Windows::UI::Core::CoreWindow^, _In_ Windows::UI::Core::KeyEventArgs^ args)
{
	this->john->KeyReleased(static_cast<short>(args->VirtualKey));
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(Platform::String^ entryPoint)
{

}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	// Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread()->Activate();
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

	create_task([this, deferral]()
	{

		// Insert your code here.

		deferral->Complete();
	});
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// Insert your code here.
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
{

}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	m_windowClosed = true;
}

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{

}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{

}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{

}