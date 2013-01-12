////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "graphicsclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "SoundClass.h"

#define RESOLUTION_X 1600 // 1600 - for desktop machine; 1024 - for laptop machine
#define RESOLUTION_Y 900  // 900 - for desktop machine; 768 - for laptop machine 


////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
	public:
		SystemClass();
		SystemClass(const SystemClass&);
		~SystemClass();

		HRESULT Initialize();
		void Shutdown();
		void Run();
		LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	private:
		bool Frame();
		void InitializeWindows(int&, int&);
		void ShutdownWindows();

	private:
		LPCWSTR m_applicationName;
		HINSTANCE m_hinstance;
		HWND m_hwnd;
		GraphicsClass* m_Graphics;

		SoundClass* m_Sound;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;

#endif