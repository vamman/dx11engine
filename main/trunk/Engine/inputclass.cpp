////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "inputclass.h"

InputClass* InputClass::instance = 0;

InputClass::InputClass() : isWireframeModeOn(true), isAllowToBBRender(true), isAllowToCameraDisplayRender(true), mMouseX(0), mMouseY(0), mMouseDeltaX(0), mMouseDeltaY(0) 
{
	m_directInput = 0;
	mKeyboard = 0;
	mMouse = 0;
}

InputClass* InputClass::GetInstance()
{
	if (instance == NULL)
	{
		instance = new InputClass();
	}
	return instance;
}

HRESULT InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mMouseDeltaX = 0;
	mMouseDeltaY = 0;

	// mLeftRightRot = 0.0f;// D3DX_PI / 2; // MathHelper.PiOver2;
	// mUpDownRot = 0.0f;// - D3DX_PI / 10; // - MathHelper.Pi / 10.0f;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return result;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL);
	if(FAILED(result))
	{
		return result;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return result;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = mKeyboard->SetCooperativeLevel(hwnd,  DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); //  DISCL_FOREGROUND | DISCL_EXCLUSIVE
	if(FAILED(result))
	{
		return result;
	}
 
	// Now acquire the keyboard.
	result = mKeyboard->Acquire();
	if(FAILED(result))
	{
		int i = 0;
		i++;
		return result;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &mMouse, NULL);
	if(FAILED(result))
	{
		return result;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = mMouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return result;
	}
 
	// Set the cooperative level of the mouse to share with other programs.
	result = mMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return result;
	}

	// Acquire the mouse.
	result = mMouse->Acquire();
	if(FAILED(result))
	{
		return result;
	}

	return result;
}

void InputClass::Shutdown()
{
	// Release the mouse.
	if(mMouse)
	{
		mMouse->Unacquire();
		mMouse->Release();
		mMouse = 0;
	}

	// Release the keyboard.
	if(mKeyboard)
	{
		mKeyboard->Unacquire();
		mKeyboard->Release();
		mKeyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	float startTime = Timer::GetInstance()->GetStartTime();
	float timePassed = (float) timeGetTime() - startTime;
	float timeDifference = timePassed / 1000.0f;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse(timeDifference);
	if(!result)
	{
		return false;
	}

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	// Remember previous keyboard state
	memcpy(mPreviousKeyboardState, mCurrentKeyboardState, sizeof(mCurrentKeyboardState));

	// Read the keyboard device.
	result = mKeyboard->GetDeviceState(sizeof(mCurrentKeyboardState), (LPVOID)&mCurrentKeyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse(float amount)
{
	HRESULT result;
	float rotationSpeed = 0.03f;
	mMouseDeltaX = 0;
	mMouseDeltaY = 0;
	//float amount = 1.0f;

	// Read the mouse device.
	result = mMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMouseCurrentState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mMouse->Acquire();
		}
		else
		{
			return false;
		}
	}
	
	if((mMouseCurrentState.lX != mMousePreviouseState.lX) || (mMouseCurrentState.lY != mMousePreviouseState.lY))
	{
		mMouseDeltaX = (mMouseCurrentState.lX - mMousePreviouseState.lX);
		mMouseDeltaY = (mMouseCurrentState.lY - mMousePreviouseState.lY);
	}
	
	CenterMouseLocation();

	return true;
}

HRESULT InputClass::CenterMouseLocation()
{
	HRESULT result = S_OK;
	mMouseX = mScreenWidth / 2;
	mMouseY = mScreenHeight / 2;
	mMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mMousePreviouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mMouse->Acquire();
		}
		else
		{
			return result;
		}
	}
	return result;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = mMouseX;
	mouseY = mMouseY;
	return;
}

void InputClass::GetMouseDelta(int& mouseDeltaX, int& mouseDeltaY)
{
	mouseDeltaX = mMouseDeltaX;
	mouseDeltaY = mMouseDeltaY;
}

void InputClass::GetMouseRotations(float& leftRight, float& upDown)
{
	leftRight = mLeftRightRot;
	upDown = mUpDownRot;
}

bool InputClass::IsTAB_PressedAndUnpressed(bool& isWireFrameModeOn)
{
	IsBtnPressedAndUnpressed(DIK_TAB, isWireFrameModeOn);
	return isWireFrameModeOn;
}

bool InputClass::IsHOME_PressedAndUnpressed()
{
	IsBtnPressedAndUnpressed(DIK_HOME, isAllowToBBRender);
	return isAllowToBBRender;
}

bool InputClass::IsEND_PressedAndUnpressed()
{
	IsBtnPressedAndUnpressed(DIK_END, isAllowToCameraDisplayRender);
	return isAllowToCameraDisplayRender;
}

bool InputClass::IsEscapePressed()
{
	if(mCurrentKeyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::IsWPressed()
{
	return IsBtnPressed(DIK_W);
}


bool InputClass::IsSPressed()
{
	return IsBtnPressed(DIK_S);
}

bool InputClass::IsAPressed()
{
	return IsBtnPressed(DIK_A);
}

bool InputClass::IsDPressed()
{
	return IsBtnPressed(DIK_D);
}

bool InputClass::IsQPressed()
{
	return IsBtnPressed(DIK_Q);
}

bool InputClass::IsEPressed()
{
	return IsBtnPressed(DIK_E);
}

bool InputClass::IsZPressed()
{
	return IsBtnPressed(DIK_Z);
}


bool InputClass::IsPgUpPressed()
{
	return IsBtnPressed(DIK_PGUP);
}


bool InputClass::IsPgDownPressed()
{
	return IsBtnPressed(DIK_PGDN);
}

bool InputClass::Is1Pressed()
{
	return IsBtnPressed(DIK_1);
}

bool InputClass::Is2Pressed()
{
	return IsBtnPressed(DIK_2);
}

bool InputClass::IsNumPad0Pressed()
{
	return IsBtnPressed(DIK_NUMPAD0);
}

bool InputClass::IsNumPad1Pressed()
{
	return IsBtnPressed(DIK_NUMPAD1);
}

bool InputClass::IsBtnPressedAndUnpressed(byte keyKode, bool& boolValue)
{

	bool prevState = mPreviousKeyboardState[keyKode] & 0x80;
	bool currState = mCurrentKeyboardState[keyKode] & 0x80;

	if ((prevState && !currState ) && boolValue == true) // false
	{
		boolValue = !boolValue; // true;
	}
	
	else if ((prevState && !currState ) && boolValue == false) // true
	{
		boolValue = !boolValue; // false;
	}
	
	return boolValue;
}

bool InputClass::IsBtnPressed(byte keyKode)
{
	if(mCurrentKeyboardState[keyKode] & 0x80)
	{
		return true;
	}
	return false;
}

