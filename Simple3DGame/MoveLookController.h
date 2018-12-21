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

#pragma once

// MoveLookController:
// This is the class that handles input events and turns it into player
// intent for movement ("move") camera ("look") and other actions.
//
// This class aggregates, mouse, keyboard, touch and game controller input
// (like the XBox controller) into a unified single controller.
// The controller has three modes of operation:
//     None - the controller is not active and all input is ignored.
//     WaitForInput - the controller is waiting for the player to acknowledge
//         a message from the app.  Only a left mouse click or touch event
//         in the rectangle specified in the WaitForPress method or the
//         'Menu' button being pressed on the game controller will
//         result in IsPressComplete returning true.
//     Active - the controller is in active game play mode.
//
// This class provides input handlers that can be associated directly with a CoreWindow
// object.  As an abstract class the association is left to the derived class.
// Input handlers are available for the following events:
//     PointerPressed
//     PointerMoved
//     PointerReleased
//     PointerExited
//     KeyDown
//     KeyUp
//     MouseMoved
//
// During 'Active' mode, the controller will put the mouse into relative mouse
// mode by turning off the pointer cursor.  In this mode the controller separates
// mouse movement from pointer movement and uses the relative mouse movements to
// update the controller for the Look Control.
//
// The touch portion of the MoveLookController divides the window area of the app
// into three distinct regions:
//     MoveRect - the rectangle of the screen where touch input is converted into
//         a virtual joystick where the vertical motion will be translated into
//         forward/backward position motion and horizontal motion will be translated
//         into left/right position motion.
//     FireRect - the rectangle of the screen where touch input will be converted
//         into a fire button.  A touch sequence of PointerPressed will result in
//         IsFiring method to return true in AutoFire mode, otherwise a PointPressed
//         followed by a PointerReleased event in the FireRect will result in the
//         IsFiring method returning true once.
//     Outside MoveRect & FireRect - this area of the screen controls the Look control
//         and updates the LookDirection.
//
// The keyboard/mouse are used as a pair to provide a typical PC gaming input model in Active
// mode:
//     Mouse - is mapped to the look control.  Left mouse is mapped to IsFiring or 'Fire' button
//     W key - is mapped to the 'forward' motion of the move control
//     S key - is mapped to the 'back' motion of the move control
//     A key - is mapped to the 'left' motion of the move control
//     D key - is mapped to the 'right' motion of the move control
//     <space> key - is mapped to the 'up' motion of the move control
//     X key - is mapped to the 'down' motion of the move control
//     P key - is mapped to IsPauseRequested
//
// This class enables game controllers (e.g. Xbox controller) which follow a polling model:
//      UpdatePollingDevices - will be called at the beginning of update
//          and as a part of checking for pause requests
//      PollingFireInUse - can use used to indicate a fire button is 
//          currently depressed.
//      ResetState - can be augumented to reset an stored state associated
//          with the polling devices
//
// The following controls are mapped from the game controller using Windows::Gaming::Input:
//     GamepadButtons::Menu - is mapped to IsPressComplete in the WaitForInput state and
//         IsPauseRequested in the Active state.
//     Left Thumb stick - is mapped to the move control in Active mode
//     Right Thumb stick - is mapped to the look control in Active mode
//     Right Trigger - is mapped to IsFiring in Active mode or "Fire" button.
//
// Because the MoveLookController can be use in a variety of environments
// such as on a separate thread, it has the option to be initialized with a CoreDispatcher.
// CoreDispatcher is used to marshall commands such as showing and hiding the cursor
// (ShowCursor, HideCursor) onto the UI thread. This is necessary when
// MoveLookController is used in a XAML app, and is not necessary when it is
// used in a CoreWindow/DirectX app.
//
// The MoveLookController collects input from each of the input events.
// On the Update method it calls UpdatePollingDevices to update state.
// All the inputs are coalesced and merged to generate a velocity vector
// and update the Pitch and Yaw values.

// Uncomment the next line to print debug tracing information.
// #define MOVELOOKCONTROLLER_TRACE 1

enum class MoveLookControllerState
{
	None,
	WaitForInput,
	Active,
};

namespace MoveLookConstants
{
	static const float RotationGain = 0.008f;       // Sensitivity adjustment for look controller.
	static const float MovementGain = 2.0f;         // Sensitivity adjustment for move controller.
};


typedef struct
{
	bool bActive;
	int id;

	int touch_number;
	int hit_num; // number of hits on one touch

	DirectX::XMFLOAT2 original_position;
	DirectX::XMFLOAT2 current_position;
	DirectX::XMFLOAT2 pixel_position;
} touch_point;

ref class MoveLookController
{
internal:
	// A CoreDispatcher is only needed when running in a XAML app.
	MoveLookController(
		_In_ Windows::UI::Core::CoreWindow^ window,
		_In_ Windows::UI::Core::CoreDispatcher^ dispatcher,
		Simple3DGame::IGameUIControl^ UIControl
		);
	MoveLookController(_In_ Windows::UI::Core::CoreWindow^ window);

	void InitWindow(_In_ Windows::UI::Core::CoreWindow^ window);

	void SetMoveRect(
		_In_ DirectX::XMFLOAT2 upperLeft,
		_In_ DirectX::XMFLOAT2 lowerRight
		);
	void SetFireRect(
		_In_ DirectX::XMFLOAT2 upperLeft,
		_In_ DirectX::XMFLOAT2 lowerRight
		);
	void WaitForPress(
		_In_ DirectX::XMFLOAT2 UpperLeft,
		_In_ DirectX::XMFLOAT2 LowerRight
		);
	void WaitForPress();

	void Update();

	bool IsFiring();
	bool IsPressComplete();
	bool IsPauseRequested();

	DirectX::XMFLOAT3 Velocity();
	DirectX::XMFLOAT3 LookDirection();
	float Pitch();
	void  Pitch(_In_ float pitch);
	float Yaw();
	void  Yaw(_In_ float yaw);
	bool  Active();
	void  Active(_In_ bool active);

	int KeyYaw() { return mk_yaw; };
	int KeyPitch() { return mk_pitch; };
	int KeyRoll() { return mk_roll; };

	int KeyHeight() { return mk_height; };
	int KeyReset() { return mk_reset; };

	int CamMoveY() { return ca_y_pos; };
	int CamMoveX() { return ca_x_pos; };
	int CamMoveZ() { return ca_z_pos; };

	int CamYaw() { return ca_yaw; };
	int CamPitch() { return ca_pitch; };
	int CamRoll() { return ca_roll; };
	int CamFast() { return ca_fast; };
	// ca_z_pos, ca_x_pos,ca_y_pos,ca_yaw,ca_pitch,ca_roll;

	bool AutoFire();
	void AutoFire(_In_ bool AutoFire);

	int jump_delay;

	DirectX::XMFLOAT2* GetSwipeVector();
	DirectX::XMFLOAT2* GetDifference(int id);
	DirectX::XMFLOAT2* GetFirstDifference();
	btVector3 point_before;
	btVector3 distance_vector;


	void Update(float timeDelta);
	bool IsReleased()
	{
		if (bReleased == true)
		{
			bReleased = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsKeyDown()
	{
		if (m_forward_fast != false || m_forward != false || m_back != false || m_left != false || m_right != false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int IsShooting()
	{
		int t = m_shooting;
		m_shooting = 0;

		return t;

	}

	DirectX::XMFLOAT2* GetKeyDirection()
	{
		DirectX::XMFLOAT2 hit_dir_b;

		hit_dir_b.x = 0.0f;
		hit_dir_b.y = 0.0f;
		if (m_forward_fast == true)
		{
			hit_dir_b.y += 2.0f;
		}
		else
		{
			if (m_forward == true)
			{
				hit_dir_b.y += 1.0f;
			}
		}
		if (m_back == true)
		{
			hit_dir_b.y -= 1.0f;
		}
		if (m_left == true)
		{
			hit_dir_b.x -= 1.0f;
		}
		if (m_right == true)
		{
			hit_dir_b.x += 1.0f;
		}
		return &hit_dir_b;
	};


	bool IsDown()
	{
		for (int i = 0; i < 10; i++)
		{
			if (touch_points[i].bActive == true)
			{
				return true;
			}
		}
		return false;

	}


	DirectX::XMFLOAT2* GetMousePixel()
	{
		for (int i = 0; i < 10; i++)
		{
			if (touch_points[i].bActive == true)
			{
				return &touch_points[i].pixel_position;
			}
		}
		return NULL;
	}


	bool IsJump()
	{
		if (bJump == true)
		{
			bJump = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	DirectX::XMFLOAT2* GetHitDirection() { return &hitDirection; }

	DirectX::XMFLOAT2* GetHitDirectionB()
	{
		DirectX::XMFLOAT2 hit_dir_b;

		if (distance_vector.length() > 0.01f)
		{
			hit_dir_b.x = -distance_vector.getX()*20.0f;
			hit_dir_b.y = -distance_vector.getZ()*20.0f;
		}
		else
		{
			hit_dir_b.x = 0.0f;
			hit_dir_b.y = 0.0f;
		}

		return &hit_dir_b;
	};

public:
	//btScalar* GetSwipeVector();

	void SetWindowsSize(Windows::Foundation::Size p_outputSize) { outputSize = p_outputSize; };

	Windows::Foundation::Point GetPointerPos() {  return m_pointer_pos; }

	void KeyFast(bool state)
	{
		m_forward_fast = state;
	}

	void KeyUp(bool state)
	{
		m_forward = state;
	}
	void KeyDown(bool state)
	{
		m_back = state;
	}
	void KeyLeft(bool state)
	{
		m_left = state;
	}
	void KeyRight(bool state)
	{
		m_right = state;
	}

	bool IsFreeze()
	{
		return m_freeze;
	}

protected private:

	DirectX::XMFLOAT2 hitDirection;
	bool bReleased;

	touch_point touch_points[20];

	Windows::Foundation::Size outputSize;

	int total_points;
	int current_point;

	float last_down;
	bool bJump;
	float last_jump;

	int touches_down;

private:
	Simple3DGame::IGameUIControl^ pUIControl;

	void ResetState();
	void UpdatePollingDevices();
	bool PollingFireInUse() { return m_gamepadTriggerInUse; }
	void ShowCursor();
	void HideCursor();

	void OnPointerPressed(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
		);
	void OnPointerMoved(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
		);
	void OnPointerReleased(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
		);
	void OnPointerExited(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
		);
	void OnKeyDown(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
		);
	void OnKeyUp(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
		);
	void OnMouseMoved(
		_In_ Windows::Devices::Input::MouseDevice^ mouseDevice,
		_In_ Windows::Devices::Input::MouseEventArgs^ args
		);
	// Hardware back button is only present on some device families such as Phone.
	//void OnHardwareBackButtonPressed(
	//	_In_ Platform::Object^ sender,
	//	_In_ Windows::Phone::UI::Input::BackPressedEventArgs^ args
	//	);
	void OnGamepadAdded(
		_In_ Platform::Object^ sender,
		_In_ Windows::Gaming::Input::Gamepad^ gamepad
		);
	void OnGamepadRemoved(
		_In_ Platform::Object^ sender,
		_In_ Windows::Gaming::Input::Gamepad^ gamepad
		);

#ifdef MOVELOOKCONTROLLER_TRACE
	void DebugTrace(const wchar_t *format, ...);
#endif

private:
	// CoreDispatcher is used to marshal execution back to the UI thread.
	// Only used when running in a XAML app.
	Windows::UI::Core::CoreDispatcher^  m_dispatcher;

	Windows::UI::Core::CoreWindow^ m_window;

	Windows::Foundation::Point m_pointer_pos;

	// Properties of the controller object.
	MoveLookControllerState     m_state;
	DirectX::XMFLOAT3           m_velocity;             // How far we move in this frame.
	float                       m_pitch;
	float                       m_yaw;                  // Orientation euler angles in radians.

														// Properties of the Move control.
	DirectX::XMFLOAT2           m_moveUpperLeft;        // Bounding box where this control will activate.
	DirectX::XMFLOAT2           m_moveLowerRight;
	bool                        m_moveInUse;            // The move control is in use.
	uint32                      m_movePointerID;        // Id of the pointer in this control.
	DirectX::XMFLOAT2           m_moveFirstDown;        // Point where initial contact occurred.
	DirectX::XMFLOAT2           m_movePointerPosition;  // Point where the move pointer is currently located.
	DirectX::XMFLOAT3           m_moveCommand;          // The net command from the move control.

														// Properties of the Look control.
	bool                        m_lookInUse;            // The look control is in use.
	uint32                      m_lookPointerID;        // Id of the pointer in this control.
	DirectX::XMFLOAT2           m_lookLastPoint;        // Last point (from last frame).
	DirectX::XMFLOAT2           m_lookLastDelta;        // The delta used for smoothing between frames.

														// Properties of the Fire control.
	bool                        m_autoFire;
	bool                        m_firePressed;
	DirectX::XMFLOAT2           m_fireUpperLeft;        // Bounding box where this control will activate.
	DirectX::XMFLOAT2           m_fireLowerRight;
	bool                        m_fireInUse;            // The fire control in in use.
	UINT32                      m_firePointerID;        // Id of the pointer in this control.
	DirectX::XMFLOAT2           m_fireLastPoint;        // Last fire position.

														// Properties of the Mouse control.  This is a combination of Look and Fire.
	bool                        m_mouseInUse;
	uint32                      m_mousePointerID;
	DirectX::XMFLOAT2           m_mouseLastPoint;
	bool                        m_mouseLeftInUse;
	bool                        m_mouseRightInUse;

	bool                        m_buttonInUse;
	uint32                      m_buttonPointerID;
	DirectX::XMFLOAT2           m_buttonUpperLeft;
	DirectX::XMFLOAT2           m_buttonLowerRight;
	bool                        m_buttonPressed;
	bool                        m_pausePressed;

	// Input states for Keyboard.
	bool                        m_forward;
	bool                        m_forward_fast;
	bool                        m_back;
	bool                        m_left;
	bool                        m_right;
	bool                        m_up;

	int                        m_shooting;

	// design mode rotation
	int							mk_yaw;
	int							mk_pitch;
	int							mk_roll;
	int							mk_height;
	int							mk_reset;

	// camera view move
	int							ca_z_pos, ca_x_pos,ca_y_pos,ca_yaw,ca_pitch,ca_roll,ca_fast;

	bool                        m_down;
	bool                        m_pause;

	bool                        m_freeze;

	// Game controller related members.
	Windows::Gaming::Input::Gamepad^    m_activeGamepad;
	std::atomic<bool>                   m_gamepadsChanged;
	bool                                m_gamepadStartButtonInUse;
	bool                                m_gamepadTriggerInUse;
};
