////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800

////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////
// INCLUDES //
//////////////

#include <dinput.h>

class InputClass
{
	public:
		static InputClass* GetInstance();
		bool Initialize(HINSTANCE, HWND, int, int);
		void Shutdown();
		bool Frame();
		bool IsEscapePressed();
		bool IsWireframeModeOn();
		bool IsAllowToBBRender();
		void GetMouseLocation(int&, int&);

		bool IsLeftPressed();
		bool IsRightPressed();
		bool IsUpPressed();
		bool IsDownPressed();
		bool IsAPressed();
		bool IsZPressed();
		bool IsPgUpPressed();
		bool IsPgDownPressed();

	private:
		InputClass();
		bool ReadKeyboard();
		bool ReadMouse();
		void ProcessInput();
		bool IsBtnPressedAndUnpressed(byte keyKode, bool& boolValue);

	private:
		IDirectInput8* m_directInput;
		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		unsigned char mCurrentKeyboardState[256];
		unsigned char mPreviousKeyboardState[256];
		
		DIMOUSESTATE m_mouseState;

		int m_screenWidth, m_screenHeight;
		int m_mouseX, m_mouseY;
		bool isWireframeModeOn, isAllowToBBRender;

		static InputClass* instance;
};
#endif
