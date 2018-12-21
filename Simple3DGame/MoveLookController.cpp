//********************************************************* 
// 
// Copyright (c) Microsoft. All rights reserved. 
// This code is licensed under the MIT License (MIT). 
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY 
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR 
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT. 
// 
//*********************************************************

#include "pch.h"
#include "MoveLookController.h"
#include "DirectXSample.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI;
using namespace Windows::Foundation;
using namespace Windows::Devices::Input;
using namespace Windows::Gaming::Input;
//using namespace Windows::Phone::UI::Input;
using namespace Windows::System;
#define TRIGGER_DEADZONE 0.1f
// Analog control deadzone definitions. Tune these values to adjust the size of the deadzone.
// Thumbstick range in each dimension is defined as [-1, 1].
#define THUMBSTICK_DEADZONE 0.25f

// Trigger range is defined as [0, 1].


// If a CoreDispatcher is provided, this indicates the game is running on a separate thread
// from the Xaml UI thread, and this separate thread does NOT have access to
// the CoreWindow.  The dispatcher can be used to marshal execution back to
// the Xaml UI thread which does have a CoreWindow.  The Dispatcher is cached
// to enable execution of code on the UI thread to turn on and off the cursor glyph.
MoveLookController::MoveLookController(_In_ CoreWindow^ window, _In_ CoreDispatcher^ dispatcher,
	Simple3DGame::IGameUIControl^ UIControl) :
	m_dispatcher(dispatcher),
	m_activeGamepad(nullptr),
	m_autoFire(true),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_state(MoveLookControllerState::None),
	m_gamepadStartButtonInUse(false),
	m_gamepadTriggerInUse(false),
	m_gamepadsChanged(true),
	pUIControl(UIControl)
{
	m_window = window;
	InitWindow(window);
}

MoveLookController::MoveLookController(_In_ CoreWindow^ window) :
	m_dispatcher(nullptr),
	m_activeGamepad(nullptr),
	m_autoFire(true),
	m_pitch(0.0f),
	m_yaw(0.0f),
	m_state(MoveLookControllerState::None),
	m_gamepadStartButtonInUse(false),
	m_gamepadTriggerInUse(false),
	m_gamepadsChanged(true)
{
	m_window = window;
	InitWindow(window);
}

void MoveLookController::InitWindow(_In_ CoreWindow^ window)
{
	ResetState();

	//window->

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerMoved);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerReleased);

	window->PointerExited +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerExited);

	window->KeyDown +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &MoveLookController::OnKeyDown);

	window->KeyUp +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &MoveLookController::OnKeyUp);

	// There is a separate handler for mouse only relative mouse movement events.
	MouseDevice::GetForCurrentView()->MouseMoved +=
		ref new TypedEventHandler<MouseDevice^, MouseEventArgs^>(this, &MoveLookController::OnMouseMoved);

	// Hardware back button is only available on some device families such as Phone.
	if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Phone.UI.Input.HardwareButtons"))
	{
		//HardwareButtons::BackPressed +=
		//	ref new EventHandler<BackPressedEventArgs^>(this, &MoveLookController::OnHardwareBackButtonPressed);
	}

	mk_yaw=0;
	mk_pitch=0;
	mk_roll=0;
	mk_reset = 0;
	mk_height = 0;

	m_freeze = false;

	//Gamepad::
	// Detect gamepad connection and disconnection events.
	Gamepad::GamepadAdded +=
		ref new EventHandler<Gamepad^>(this, &MoveLookController::OnGamepadAdded);

	Gamepad::GamepadRemoved +=
		ref new EventHandler<Gamepad^>(this, &MoveLookController::OnGamepadRemoved);

	
}

//----------------------------------------------------------------------

bool MoveLookController::IsPauseRequested()
{
	if (m_pausePressed == true)
	{
		m_pausePressed = false;
		return true;
	}
	else
	{
		return false;
	}
	/*
	switch (m_state)
	{
	case MoveLookControllerState::Active:
	UpdatePollingDevices();
	if (m_pausePressed)
	{
	#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"IsPauseRequested == true\n");
	#endif
	m_pausePressed = false;
	return true;
	}
	else
	{
	return false;
	}
	}
	return false;
	*/
}

//----------------------------------------------------------------------

bool MoveLookController::IsFiring()
{
	if (m_state == MoveLookControllerState::Active)
	{
		if (m_autoFire)
		{
			return (m_fireInUse || (m_mouseInUse && m_mouseLeftInUse) || PollingFireInUse());
		}
		else
		{
			if (m_firePressed)
			{
				m_firePressed = false;
				return true;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------

bool MoveLookController::IsPressComplete()
{
	switch (m_state)
	{
	case MoveLookControllerState::WaitForInput:
		UpdatePollingDevices();
		if (m_buttonPressed)
		{
#ifdef MOVELOOKCONTROLLER_TRACE
			DebugTrace(L"IsPressComplete == true\n");
#endif
			m_buttonPressed = false;
			return true;
		}
		else
		{
			return false;
		}
		break;
	}

	return false;
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerPressed(
	_In_ CoreWindow^ /* sender */,
	_In_ PointerEventArgs^ args
	)
{
	//return;

	bool bfound = false;

	if (last_down<20.0f)
	{
		if (last_jump <= 1.0f)
		{
			//bJump = true;
			last_jump = 50.0f;

		}
	}
	last_down = 0.0f;

	if (jump_delay == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			if (touch_points[i].bActive == true)
			{
				bJump = true;
				jump_delay = 10;
			}
		}
	}

	for (int i = 0; i<10; i++)
	{
		if (bfound == false)
		{
			if (touch_points[i].bActive == false)
			{
				bfound = true;
				touch_points[i].bActive = true;
				touch_points[i].id = args->CurrentPoint->PointerId;
				touch_points[i].original_position = DX::PointToTouch(args->CurrentPoint->Position, outputSize);
				touch_points[i].current_position = touch_points[i].original_position;
				touch_points[i].pixel_position = XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
				touch_points[i].touch_number = touches_down++;
				touch_points[i].hit_num = 0;

				point_before.setX(touch_points[i].current_position.x);
				point_before.setY(0.0f);
				point_before.setZ(touch_points[i].current_position.y);

				break;
			}
		}
	}
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerMoved(
	_In_ CoreWindow^ /* sender */,
	_In_ PointerEventArgs^ args
	)
{
	m_pointer_pos.X = args->CurrentPoint->Position.X;
	m_pointer_pos.Y = args->CurrentPoint->Position.Y;
	for (int i = 0; i<10; i++)
	{
		if (touch_points[i].id == args->CurrentPoint->PointerId && touch_points[i].bActive == true)
		{
			touch_points[i].current_position = DX::PointToTouch(args->CurrentPoint->Position, outputSize);
			touch_points[i].pixel_position = XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
			//char str[40];
			//sprintf_s(str , "%f  %f\n" , touch_points[i].current_position.x , touch_points[i].current_position.y );

			//OutputDebugStringA(str);

		}
	}
}

//----------------------------------------------------------------------

void MoveLookController::OnMouseMoved(
	_In_ MouseDevice^ /* mouseDevice */,
	_In_ MouseEventArgs^ args
	)
{
	// Handle Mouse Input via dedicated relative movement handler.

	switch (m_state)
	{
	case MoveLookControllerState::Active:
		XMFLOAT2 mouseDelta;
		mouseDelta.x = static_cast<float>(args->MouseDelta.X);
		mouseDelta.y = static_cast<float>(args->MouseDelta.Y);


		XMFLOAT2 rotationDelta;
		rotationDelta.x = mouseDelta.x * MoveLookConstants::RotationGain;   // Scale for control sensitivity.
		rotationDelta.y = mouseDelta.y * MoveLookConstants::RotationGain;

		// Update our orientation based on the command.
		m_pitch -= rotationDelta.y;
		m_yaw += rotationDelta.x;

		// Limit pitch to straight up or straight down.
		float limit = XM_PI / 2.0f - 0.01f;
		m_pitch = __max(-limit, m_pitch);
		m_pitch = __min(+limit, m_pitch);

		// Keep longitude in sane range by wrapping.
		if (m_yaw >  XM_PI)
		{
			m_yaw -= XM_PI * 2.0f;
		}
		else if (m_yaw < -XM_PI)
		{
			m_yaw += XM_PI * 2.0f;
		}
		break;
	}
}

//----------------------------------------------------------------------

void MoveLookController::OnPointerReleased(
	_In_ CoreWindow^ /* sender */,
	_In_ PointerEventArgs^ args
	)
{
	for (int i = 0; i<10; i++)
	{
		if (touch_points[i].id == args->CurrentPoint->PointerId && touch_points[i].bActive == true)
		{
			touch_points[i].bActive = false;

			if (true) // touch_points[i].current_position.x==0.0f && touch_points[i].current_position.y==0.0f )
			{
				bReleased = true;

				hitDirection.x = touch_points[i].current_position.x - touch_points[i].original_position.x;
				hitDirection.y = touch_points[i].current_position.y - touch_points[i].original_position.y;
			}
		}
	}

}

//----------------------------------------------------------------------

void MoveLookController::OnPointerExited(
	_In_ CoreWindow^ /* sender */,
	_In_ PointerEventArgs^ args
	)
{
	PointerPoint^ point = args->CurrentPoint;
	uint32 pointerID = point->PointerId;
	Point pointerPosition = point->Position;
	PointerPointProperties^ pointProperties = point->Properties;

	XMFLOAT2 position = XMFLOAT2(pointerPosition.X, pointerPosition.Y);

#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"%-7s (%d) at (%4.0f, %4.0f)\n", L"Exit", pointerID, position.x, position.y);
#endif

	switch (m_state)
	{
	case MoveLookControllerState::WaitForInput:
		if (m_buttonInUse && (pointerID == m_buttonPointerID))
		{
			m_buttonInUse = false;
			m_buttonPressed = false;
#ifdef MOVELOOKCONTROLLER_TRACE
			DebugTrace(L"\tWaitForPress: ButtonPressed = false -- Got Exit Event\n");
#endif
		}
		break;

	case MoveLookControllerState::Active:
		if (pointerID == m_movePointerID)
		{
			m_velocity = XMFLOAT3(0, 0, 0);      // Stop on release.
			m_moveInUse = false;
			m_movePointerID = 0;
		}
		else if (pointerID == m_lookPointerID)
		{
			m_lookInUse = false;
			m_lookPointerID = 0;
		}
		else if (pointerID == m_firePointerID)
		{
			m_fireInUse = false;
			m_firePointerID = 0;
		}
		else if (pointerID == m_mousePointerID)
		{
			m_mouseInUse = false;
			m_mousePointerID = 0;
			m_mouseLeftInUse = false;
			m_mouseRightInUse = false;
		}
		break;
	}
}

//----------------------------------------------------------------------

void MoveLookController::OnKeyDown(
	_In_ CoreWindow^ /* sender */,
	_In_ KeyEventArgs^ args
	)
{
	Windows::System::VirtualKey Key;
	Key = args->VirtualKey;

	// ca_z_pos, ca_x_pos,ca_y_pos,ca_yaw,ca_pitch,ca_roll

	// Figure out the command from the keyboard.

	if (Key == VirtualKey::NumberPad8)
		ca_z_pos = 1;
	if (Key == VirtualKey::NumberPad2)
		ca_z_pos = -1;
	if (Key == VirtualKey::NumberPad4)
		ca_x_pos = -1;
	if (Key == VirtualKey::NumberPad6)
		ca_x_pos = 1;
	if (Key == VirtualKey::NumberPad9)
		ca_y_pos = 1;
	if (Key == VirtualKey::NumberPad3)
		ca_y_pos = -1;

	if (Key == VirtualKey::Up)
		ca_pitch = 1;
	if (Key == VirtualKey::Down)
		ca_pitch = -1;
	if (Key == VirtualKey::Left)
		ca_yaw = -1;
	if (Key == VirtualKey::Right)
		ca_yaw = 1;
/*	if (Key == VirtualKey::Delete)
		ca_yaw = -1;
	if (Key == VirtualKey::PageDown)
		ca_yaw = 1;
		*/

	if (Key == VirtualKey::W)
	{
		m_forward = true;
		m_forward_fast = true;
		//pUIControl->HideHUD();
		pUIControl->SetScreenControls(0);
		//m_window->HideHUD();
	}
	if (Key == VirtualKey::S)
		m_back = true;
	if (Key == VirtualKey::A)
		m_left = true;
	if (Key == VirtualKey::D)
		m_right = true;

	if (Key == VirtualKey::Q)
		m_freeze = true;

	if (Key == VirtualKey::K)
		mk_yaw = 1;
	if (Key == VirtualKey::H)
		mk_yaw = -1;
	if (Key == VirtualKey::U)
		mk_pitch = 1;
	if (Key == VirtualKey::J)
		mk_pitch = -1;
	if (Key == VirtualKey::Y)
		mk_roll = 1;
	if (Key == VirtualKey::I)
		mk_roll = -1;

	if (Key == VirtualKey::O)
		mk_height = 1;
	if (Key == VirtualKey::L)
		mk_height = -1;

	if (Key == VirtualKey::P)
		mk_reset = 1;

	if (Key == VirtualKey::Control)
		ca_fast = 1;


	if (Key == VirtualKey::R)
	{
		if (jump_delay == 0)
		{
			bJump = true;
			jump_delay = 10;
		}
	}
	if (Key == VirtualKey::Space)
	{
		m_shooting = 1;
	}
	if (Key == VirtualKey::Shift)
	{
		m_forward_fast = true;
	}
	//m_up = true;
	if (Key == VirtualKey::X)
		m_down = true;
	if (Key == VirtualKey::P)
		m_pause = true;
}

//----------------------------------------------------------------------

void MoveLookController::OnKeyUp(
	_In_ CoreWindow^ /* sender */,
	_In_ KeyEventArgs^ args
	)
{
	Windows::System::VirtualKey Key;
	Key = args->VirtualKey;




	if (Key == VirtualKey::NumberPad8)
		ca_z_pos = 0;
	if (Key == VirtualKey::NumberPad2)
		ca_z_pos = 0;
	if (Key == VirtualKey::NumberPad4)
		ca_x_pos = 0;
	if (Key == VirtualKey::NumberPad6)
		ca_x_pos = 0;
	if (Key == VirtualKey::NumberPad9)
		ca_y_pos = 0;
	if (Key == VirtualKey::NumberPad3)
		ca_y_pos = 0;

	if (Key == VirtualKey::Up)
		ca_pitch = 0;
	if (Key == VirtualKey::Down)
		ca_pitch = 0;
	if (Key == VirtualKey::Left)
		ca_yaw = 0;
	if (Key == VirtualKey::Right)
		ca_yaw = 0;
	/*
	if (Key == VirtualKey::Delete)
		ca_yaw = 0;
	if (Key == VirtualKey::PageDown)
		ca_yaw = 0;
		*/
	if (Key == VirtualKey::Control)
		ca_fast = 0;

	// Figure out the command from the keyboard.
	if (Key == VirtualKey::W)
	{
		m_forward = false;
		m_forward_fast = false;
	}
	if (Key == VirtualKey::S)
		m_back = false;
	if (Key == VirtualKey::A)
		m_left = false;
	if (Key == VirtualKey::D)
		m_right = false;

	if (Key == VirtualKey::Q)
		m_freeze = false;

	if (Key == VirtualKey::K)
		mk_yaw = 0;
	if (Key == VirtualKey::H)
		mk_yaw = 0;
	if (Key == VirtualKey::U)
		mk_pitch = 0;
	if (Key == VirtualKey::J)
		mk_pitch = 0;
	if (Key == VirtualKey::Y)
		mk_roll = 0;
	if (Key == VirtualKey::I)
		mk_roll = 0;

	if (Key == VirtualKey::O)
		mk_height = 0;
	if (Key == VirtualKey::L)
		mk_height = 0;

	if (Key == VirtualKey::P)
		mk_reset = 0;

	if (Key == VirtualKey::Space)
	{
		m_shooting = -1;
	}
	if (Key == VirtualKey::Shift)
	{
		m_forward_fast = false;
	}
	if (Key == VirtualKey::X)
		m_down = false;
	if (Key == VirtualKey::P)
	{
		if (m_pause)
		{
			// Trigger pause only once on button release.
			m_pausePressed = true;
			m_pause = false;
		}
	}
}

//----------------------------------------------------------------------

void MoveLookController::ResetState()
{
	// Reset the state of the MoveLookController.
	// Disable any active pointer IDs to stop all interaction.
	m_buttonPressed = false;
	m_pausePressed = false;
	m_buttonInUse = false;
	m_moveInUse = false;
	m_lookInUse = false;
	m_fireInUse = false;
	m_mouseInUse = false;
	m_mouseLeftInUse = false;
	m_mouseRightInUse = false;
	m_movePointerID = 0;
	m_lookPointerID = 0;
	m_firePointerID = 0;
	m_mousePointerID = 0;
	m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_moveCommand = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_forward = false;
	m_back = false;
	m_left = false;
	m_right = false;
	m_up = false;
	m_down = false;
	m_pause = false;
}

//----------------------------------------------------------------------

void MoveLookController::UpdatePollingDevices()
{
	if (m_gamepadsChanged)
	{
		m_gamepadsChanged = false;
		unsigned int index = 0;

		if (Gamepad::Gamepads->Size == 0)
		{
			m_activeGamepad = nullptr;
		}
		// Check if the cached gamepad is still connected.
		else if (!Gamepad::Gamepads->IndexOf(m_activeGamepad, &index))
		{
			// MoveLookController is intended to handle input for a single player, so it
			// defaults to the first active gamepad.
			m_activeGamepad = Gamepad::Gamepads->GetAt(0);
		}
	}

	if (m_activeGamepad == nullptr)
	{
		return;
	}

	GamepadReading reading = m_activeGamepad->GetCurrentReading();

	switch (m_state)
	{
	case MoveLookControllerState::WaitForInput:
		if ((reading.Buttons & GamepadButtons::Menu) == GamepadButtons::Menu)
		{
			m_gamepadStartButtonInUse = true;
		}
		else if (m_gamepadStartButtonInUse)
		{
			// Trigger once only on button release.
			m_gamepadStartButtonInUse = false;
			m_buttonPressed = true;
		}
		break;

	case MoveLookControllerState::Active:
		if ((reading.Buttons & GamepadButtons::Menu) == GamepadButtons::Menu)
		{
			m_gamepadStartButtonInUse = true;
		}
		else if (m_gamepadStartButtonInUse)
		{
			// Trigger once only on button release.
			m_gamepadStartButtonInUse = false;
			m_pausePressed = true;
		}
		// Use the left thumbstick on the game controller to control
		// the eye point position control. Thumbstick input is defined from [-1, 1].
		// We use a deadzone in the middle range to avoid drift.
		if (reading.LeftThumbstickX > THUMBSTICK_DEADZONE ||
			reading.LeftThumbstickX < -THUMBSTICK_DEADZONE)
		{
			float x = static_cast<float>(reading.LeftThumbstickX);
			m_moveCommand.x -= (x > 0) ? 1 : -1;
		}

		if (reading.LeftThumbstickY > THUMBSTICK_DEADZONE ||
			reading.LeftThumbstickY < -THUMBSTICK_DEADZONE)
		{
			float y = static_cast<float>(reading.LeftThumbstickY);
			m_moveCommand.y += (y > 0) ? 1 : -1;
		}

		// Use the right thumbstick on the game controller to control
		// the look at control. Thumbstick input is defined from [-1, 1].
		// We use a deadzone in the middle range to avoid drift.
		XMFLOAT2 pointerDelta;
		if (reading.RightThumbstickX > THUMBSTICK_DEADZONE ||
			reading.RightThumbstickX < -THUMBSTICK_DEADZONE)
		{
			float x = static_cast<float>(reading.RightThumbstickX);
			pointerDelta.x = x * x * x;
		}
		else
		{
			pointerDelta.x = 0.0f;
		}

		if (reading.RightThumbstickY > THUMBSTICK_DEADZONE ||
			reading.RightThumbstickY < -THUMBSTICK_DEADZONE)
		{
			float y = static_cast<float>(reading.RightThumbstickY);
			pointerDelta.y = y * y * y;
		}
		else
		{
			pointerDelta.y = 0.0f;
		}

		XMFLOAT2 rotationDelta;
		rotationDelta.x = pointerDelta.x *  0.08f;       // Scale for control sensitivity.
		rotationDelta.y = pointerDelta.y *  0.08f;

		// Update our orientation based on the command.
		m_pitch += rotationDelta.y;
		m_yaw += rotationDelta.x;

		// Limit pitch to straight up or straight down.
		m_pitch = __max(-XM_PI / 2.0f, m_pitch);
		m_pitch = __min(+XM_PI / 2.0f, m_pitch);

		// Check the state of the Right Trigger button.  This is used to indicate fire control.

		if (reading.RightTrigger > TRIGGER_DEADZONE)
		{
			if (!m_autoFire && !m_gamepadTriggerInUse)
			{
				m_firePressed = true;
			}

			m_gamepadTriggerInUse = true;
		}
		else
		{
			m_gamepadTriggerInUse = false;
		}
		break;
	}
}

//----------------------------------------------------------------------
// Turn on mouse cursor, which also disables relative mouse movement tracking.
void MoveLookController::ShowCursor()
{
	//CoreWindow::PointerPosition
	//CoreWindow::PointerPosition
	if (m_dispatcher != nullptr)
	{
		// Use the CoreDispatcher to ensure that the pointer is updated on the UI thread.
		// This is needed for XAML updates in a XAML app.
		m_dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this]()
		{
			CoreWindow::GetForCurrentThread()->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
		})
			);
	}
	else
	{
		auto window = CoreWindow::GetForCurrentThread();
		if (window)
		{
			// Protect case where there isn't a window associated with the current thread.
			// This happens on initialization or when being called from a background thread.
			window->PointerCursor = ref new CoreCursor(CoreCursorType::Arrow, 0);
		}
	}
}

//----------------------------------------------------------------------

// Turn mouse cursor off (hidden), which also enables relative mouse movement tracking.
void MoveLookController::HideCursor()
{
	if (m_dispatcher != nullptr)
	{
		// Use the CoreDispatcher to ensure that the pointer is updated on the UI thread.
		// This is needed for XAML updates in a XAML app.
		m_dispatcher->RunAsync(
			CoreDispatcherPriority::Normal,
			ref new DispatchedHandler([this]()
		{
			CoreWindow::GetForCurrentThread()->PointerCursor = nullptr;
		})
			);
	}
	else
	{
		auto window = CoreWindow::GetForCurrentThread();
		if (window)
		{
			// Protect case where there isn't a window associated with the current thread.
			// This happens on initialization or when being called from a background thread.
			window->PointerCursor = nullptr;
		}
	}
}

//----------------------------------------------------------------------

void MoveLookController::SetMoveRect(
	_In_ XMFLOAT2 upperLeft,
	_In_ XMFLOAT2 lowerRight
	)
{
#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"MoveRect (%d, %d) to (%d, %d)", upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y);
#endif
	m_moveUpperLeft = upperLeft;
	m_moveLowerRight = lowerRight;
}

//----------------------------------------------------------------------

void MoveLookController::SetFireRect(
	_In_ XMFLOAT2 upperLeft,
	_In_ XMFLOAT2 lowerRight
	)
{
#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"FireRect (%d, %d) to (%d, %d)", upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y);
#endif
	m_fireUpperLeft = upperLeft;
	m_fireLowerRight = lowerRight;
}

//----------------------------------------------------------------------

void MoveLookController::WaitForPress(
	_In_ XMFLOAT2 upperLeft,
	_In_ XMFLOAT2 lowerRight
	)
{
#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"WaitForPress (%.1f, %.1f) to (%.1f, %.1f)\n", upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y);
#endif

	ResetState();
	m_state = MoveLookControllerState::WaitForInput;
	m_buttonUpperLeft = upperLeft;
	m_buttonLowerRight = lowerRight;
	ShowCursor();
}

//----------------------------------------------------------------------

void MoveLookController::WaitForPress()
{
#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"WaitForPress (null rect)\n");
#endif

	ResetState();
	m_state = MoveLookControllerState::WaitForInput;
	m_buttonUpperLeft.x = 0.0f;
	m_buttonUpperLeft.y = 0.0f;
	m_buttonLowerRight.x = 0.0f;
	m_buttonLowerRight.y = 0.0f;
	ShowCursor();
}

//----------------------------------------------------------------------

XMFLOAT3 MoveLookController::Velocity()
{
	return m_velocity;
}

//----------------------------------------------------------------------

XMFLOAT3 MoveLookController::LookDirection()
{
	XMFLOAT3 lookDirection;

	float r = cosf(m_pitch);              // In the plane.
	lookDirection.y = sinf(m_pitch);      // Vertical.
	lookDirection.z = r * cosf(m_yaw);    // Fwd-back.
	lookDirection.x = r * sinf(m_yaw);    // Left-right.

	return lookDirection;
}

//----------------------------------------------------------------------

float MoveLookController::Pitch()
{
	return m_pitch;
}

//----------------------------------------------------------------------

void MoveLookController::Pitch(_In_ float pitch)
{
	m_pitch = pitch;
}

//----------------------------------------------------------------------

float MoveLookController::Yaw()
{
	return m_yaw;
}

//----------------------------------------------------------------------

void MoveLookController::Yaw(_In_ float yaw)
{
	m_yaw = yaw;
}

//----------------------------------------------------------------------

void MoveLookController::Active(_In_ bool active)
{
	ResetState();
#ifdef MOVELOOKCONTROLLER_TRACE
	DebugTrace(L"Active = %s\n", active ? L"true" : L"false");
#endif

	if (active)
	{
		m_state = MoveLookControllerState::Active;
		HideCursor();

	}
	else
	{
		m_state = MoveLookControllerState::None;
		ShowCursor();
	}
}

//----------------------------------------------------------------------

bool MoveLookController::Active()
{
	if (m_state == MoveLookControllerState::Active)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//----------------------------------------------------------------------

void MoveLookController::AutoFire(_In_ bool autoFire)
{
	m_autoFire = autoFire;
}

//----------------------------------------------------------------------

bool MoveLookController::AutoFire()
{
	return m_autoFire;
}

//----------------------------------------------------------------------
DirectX::XMFLOAT2* MoveLookController::GetDifference(int id)
{
	static DirectX::XMFLOAT2 new_position;
	new_position.x = touch_points[id].current_position.x - touch_points[id].original_position.x;
	new_position.y = touch_points[id].current_position.y - touch_points[id].original_position.y;

	return &new_position;
}

DirectX::XMFLOAT2* MoveLookController::GetFirstDifference()
{
	bool bFound = false;
	static DirectX::XMFLOAT2 zero_move = DirectX::XMFLOAT2(0.0f, 0.0f);

	for (int i = 0; i < 10; i++)
	{
		if (bFound == false)
		{
			if (touch_points[i].bActive == true)
			{
				bFound = true;

				zero_move.x = touch_points[i].current_position.x - touch_points[i].original_position.x;
				zero_move.y = touch_points[i].current_position.y - touch_points[i].original_position.y;
			}
		}
	}
	return &zero_move;
}



DirectX::XMFLOAT2* MoveLookController::GetSwipeVector()
{
	int i;
	int lowest = 999;
	int low_index = -1;
	static DirectX::XMFLOAT2 ret_vec;

	for (i = 0; i<10; i++)
	{
		if (touch_points[i].bActive == true)
		{
			if (touch_points[i].touch_number<lowest)
			{
				low_index = i;
				lowest = touch_points[i].touch_number;
			}
		}
	}

	if (low_index != -1)
	{
		ret_vec.x = touch_points[low_index].current_position.x - touch_points[low_index].original_position.x;
		ret_vec.y = touch_points[low_index].current_position.y - touch_points[low_index].original_position.y;

		btVector3 current(touch_points[low_index].current_position.x, touch_points[low_index].current_position.y, 0.0f);
		btVector3 original(touch_points[low_index].original_position.x, touch_points[low_index].original_position.y, 0.0f);
		btVector3 new_current = original - current;
		new_current.normalize();
		new_current *= 0.08f;

		btScalar dist = current.distance(original);

		if (dist > 0.08f)
		{
			current = current + new_current;
			touch_points[low_index].original_position.x = current.getX();
			touch_points[low_index].original_position.y = current.getY();
			ret_vec.x = touch_points[low_index].current_position.x - touch_points[low_index].original_position.x;
			ret_vec.y = touch_points[low_index].current_position.y - touch_points[low_index].original_position.y;
		}

	}
	else
	{
		ret_vec.x = 0.0f;
		ret_vec.y = 0.0f;
	}

	return &ret_vec;
}

void MoveLookController::Update(float timeDelta)
{
	last_down += 1.0f;
	if (last_jump>0.0f)
		last_jump -= 1.0f;

	{	btVector3 p1, p2;
	float dist_to;

	if (jump_delay > 0)
		jump_delay--;
	/*
	POINT p;
	if (GetCursorPos(&p))
	{
		//cursor position now in p.x and p.y
	}
	*/
	//m_pointer_pos = m_window->PointerPosition;

	
	for (int i = 0; i<10; i++)

		if (touch_points[i].bActive == true)
		{
			p1 = btVector3(touch_points[i].current_position.x, 0.0f, touch_points[i].current_position.y);
			p2 = btVector3(touch_points[i].original_position.x, 0.0f, touch_points[i].original_position.y);

			distance_vector = point_before - p1;

			point_before += (p1 - point_before)*(timeDelta*1.5);


			if (touch_points[i].hit_num > 0)
				dist_to = 0.002f;
			else
				dist_to = 0.0007f;


			if (p1.distance(p2) > dist_to)
			{
				//bReleased = true;

				hitDirection.x = touch_points[i].current_position.x - touch_points[i].original_position.x;
				hitDirection.y = touch_points[i].current_position.y - touch_points[i].original_position.y;

				touch_points[i].original_position.x = touch_points[i].current_position.x;
				touch_points[i].original_position.y = touch_points[i].current_position.y;
				touch_points[i].hit_num++;

				//touch_points[i].bActive = false;
			}

			if (true) // touch_points[i].current_position.x==0.0f && touch_points[i].current_position.y==0.0f )
			{

			}
		}
	}

}

void MoveLookController::Update()
{
	UpdatePollingDevices();

	if (m_moveInUse)
	{
		// Move control.
		XMFLOAT2 pointerDelta;

		pointerDelta.x = m_movePointerPosition.x - m_moveFirstDown.x;
		pointerDelta.y = m_movePointerPosition.y - m_moveFirstDown.y;

		// Figure out the command from the virtual joystick.
		XMFLOAT3 commandDirection = XMFLOAT3(0.0f, 0.0f, 0.0f);
		if (fabsf(pointerDelta.x) > 16.0f)         // Leave 32 pixel-wide dead spot for being still.
			m_moveCommand.x -= pointerDelta.x / fabsf(pointerDelta.x);

		if (fabsf(pointerDelta.y) > 16.0f)
			m_moveCommand.y -= pointerDelta.y / fabsf(pointerDelta.y);
	}

	// Poll our state bits set by the keyboard input events.
	if (m_forward)
	{
		m_moveCommand.y += 1.0f;
	}
	if (m_back)
	{
		m_moveCommand.y -= 1.0f;
	}
	if (m_left)
	{
		m_moveCommand.x += 1.0f;
	}
	if (m_right)
	{
		m_moveCommand.x -= 1.0f;
	}
	if (m_up)
	{
		m_moveCommand.z += 1.0f;
	}
	if (m_down)
	{
		m_moveCommand.z -= 1.0f;
	}

	// Make sure that 45deg cases are not faster.
	if (fabsf(m_moveCommand.x) > 0.1f ||
		fabsf(m_moveCommand.y) > 0.1f ||
		fabsf(m_moveCommand.z) > 0.1f)
	{
		XMStoreFloat3(&m_moveCommand, XMVector3Normalize(XMLoadFloat3(&m_moveCommand)));
	}

	// Rotate command to align with our direction (world coordinates).
	XMFLOAT3 wCommand;
	wCommand.x = m_moveCommand.x * cosf(m_yaw) - m_moveCommand.y * sinf(m_yaw);
	wCommand.y = m_moveCommand.x * sinf(m_yaw) + m_moveCommand.y * cosf(m_yaw);
	wCommand.z = m_moveCommand.z;

	// Scale for sensitivity adjustment.
	// Our velocity is based on the command. Y is up.
	m_velocity.x = -wCommand.x * MoveLookConstants::MovementGain;
	m_velocity.z = wCommand.y * MoveLookConstants::MovementGain;
	m_velocity.y = wCommand.z * MoveLookConstants::MovementGain;

	// Clear movement input accumulator for use during next frame.
	m_moveCommand = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

//----------------------------------------------------------------------
/*
void MoveLookController::OnHardwareBackButtonPressed(
	_In_ Platform::Object^ sender,
	_In_ BackPressedEventArgs^ args
	)
{
	if (true)//m_state == MoveLookControllerState::Active)
	{
		//exit(0);
		// The game is currently in active play mode, so hitting the hardware back button
		// will cause the game to pause.
		m_pausePressed = true;
		args->Handled = true;
	}
	else
	{
		// The game is not currently in active play mode, so take the default behavior
		// for the hardware back button.
		args->Handled = false;
	}
}
*/
//----------------------------------------------------------------------

void MoveLookController::OnGamepadAdded(_In_ Object^ sender, _In_ Gamepad^ gamepad)
{
	// OnGamepadAdded and OnGamepadRemoved can be called from a worker thread. For simplicity,
	// defer updating the active gamepad until Update().
	m_gamepadsChanged = true;
}

//----------------------------------------------------------------------

void MoveLookController::OnGamepadRemoved(_In_ Object^ sender, _In_ Gamepad^ gamepad)
{
	// OnGamepadAdded and OnGamepadRemoved can be called from a worker thread. For simplicity,
	// defer updating the active gamepad until Update().
	m_gamepadsChanged = true;
}

#ifdef MOVELOOKCONTROLLER_TRACE
void MoveLookController::DebugTrace(const wchar_t *format, ...)
{
	// Generate the message string.
	va_list args;
	va_start(args, format);
	wchar_t message[1024];
	vswprintf_s(message, 1024, format, args);
	OutputDebugStringW(message);
}
#endif

//----------------------------------------------------------------------
