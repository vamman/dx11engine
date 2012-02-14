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
#include <d3d11.h>
#include <d3dx10math.h>
#include "timerclass.h"

class InputClass
{
	public:
		static InputClass* GetInstance();
		HRESULT Initialize(HINSTANCE, HWND, int, int);
		void Shutdown();
		bool Frame();
		bool IsEscapePressed();
		void GetMouseLocation(int&, int&);

		void CenterMouseLocation();

		void GetMouseDelta(float&, float&);

		bool IsWPressed();
		bool IsSPressed();
		bool IsAPressed();
		bool IsDPressed();

		bool IsQPressed();
		bool IsEPressed();
		
		bool IsZPressed();
		bool IsPgUpPressed();
		bool IsPgDownPressed();

		// Press/Unpress functions
		bool IsWireframeModeOn();
		bool IsAllowToBBRender();
		bool IsAllowToCameraDisplayRender();

		void GetMouseRotations(float& leftRight, float& upDown);

	private:
		InputClass();
		bool ReadKeyboard();
		bool ReadMouse(float amount);
		void ProcessInput();
		bool IsBtnPressed(byte keyKode);
		bool IsBtnPressedAndUnpressed(byte keyKode, bool& boolValue);

	private:
		IDirectInput8* m_directInput;
		IDirectInputDevice8* mKeyboard;
		IDirectInputDevice8* mMouse;

		unsigned char mCurrentKeyboardState[256];
		unsigned char mPreviousKeyboardState[256];
		
		DIMOUSESTATE mMouseCurrentState, mMousePreviouseState;

		int mScreenWidth, mScreenHeight;
		int mMouseX, mMouseY;
		float mMouseDeltaX, mMouseDeltaY;
		float mLeftRightRot, mUpDownRot;

		// Press/Unpress flags
		bool isWireframeModeOn, isAllowToBBRender, isAllowToCameraDisplayRender;

		static InputClass* instance;
};
#endif
