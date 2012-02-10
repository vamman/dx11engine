////////////////////////////////////////////////////////////////////////////////
// Filename: UserInput.h
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

class UserInput
{
	public:
		static UserInput* GetInstance();
		bool Initialize(HINSTANCE, HWND, int, int);
		void Shutdown();
		bool Frame();
		bool IsEscapePressed();
		void GetMouseLocation(int&, int&);

	private:
		UserInput(){};  // Private so that it can  not be called
		UserInput(UserInput const&){};             // copy constructor is private
		UserInput& operator=(UserInput const&){};  // assignment operator is private

		bool ReadKeyboard();
		bool ReadMouse();
		void ProcessInput();

		IDirectInput8* m_directInput;
		IDirectInputDevice8* m_keyboard;
		IDirectInputDevice8* m_mouse;

		unsigned char m_keyboardState[256];
		DIMOUSESTATE m_mouseState;

		int m_screenWidth, m_screenHeight;
		int m_mouseX, m_mouseY;

		static UserInput* instance;
};
#endif
