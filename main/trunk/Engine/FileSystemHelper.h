#ifndef _FILE_SYSTEM_HELPER_H_
#define _FILE_SYSTEM_HELPER_H_

#include <string>
#include <windows.h>

using namespace std;

#define  EXTENSION_DDS	"dds"
#define  EXTENSION_RAW	"raw"
#define  EXTENSION_BMP	"bmp"
#define  EXTENSION_JPG	"jpg"
#define  EXTENSION_PNG	"png"
#define  EXTENSION_TXT	"txt"
#define  EXTENSION_OBJ	"obj"
#define  EXTENSION_FX	"fx"
#define  EXTENSION_VS	"vs"
#define  EXTENSION_PS	"ps"
#define  EXTENSION_WAV	"wav"
#define  EXTENSION_MP3	"mp3"

namespace FileSystemHelper
{
	enum FileExtensions
	{
		ExtensionInvalid = -1,

		ExtensionDDS,
		ExtensionRAW,
		ExtensionBMP,
		ExtensionJPG,
		ExtensionPNG,
		ExtensionTXT,
		ExtensionOBJ,
		ExtensionFX,
		ExtensionVS,
		ExtensionPS,
		ExtensionWAV,
		ExtensionMP3,

		ExtensionCount
	};

	FileExtensions	GetFileExtension(wstring fileName);
	wstring			GetFilenameWithoutExtension(wstring fileName);
	wstring			GetResourceFolder();
	wstring			ConvertStringToWString(string lpa);
	string			ConvertWStringToString(wstring lpa);
	wstring			GetResourcePath(wstring str);

	inline wstring GetResourceFolder()
	{
		return L"../Engine/data";
	}

	inline wstring GetFilenameWithoutExtension(wstring fileName)
	{
		int lastSlashPos = fileName.find_last_of(L"/");
		int strLength = fileName.find_last_of(L".") - (lastSlashPos + 1);
		return fileName.substr(lastSlashPos + 1, strLength);
	}

	inline wstring ConvertStringToWString(string lpa)
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, &lpa[0], (int)lpa.size(), NULL, 0);
		wstring lpw(size, 0);
		MultiByteToWideChar(CP_UTF8, 0, &lpa[0], (int)lpa.size(), &lpw[0], size);
		return lpw;
	}

	inline string ConvertWStringToString(wstring lpw)
	{
		int size = WideCharToMultiByte(CP_ACP, 0, &lpw[0], (int)lpw.size(), 0, 0, 0, 0);
		string lpa(size, 0);
		WideCharToMultiByte(CP_ACP, 0, &lpw[0], (int)lpw.size(), &lpa[0], size, 0, 0);
		return lpa;
	}

	inline wstring GetResourcePath(wstring str)
	{
		return GetResourceFolder().append(str);
	}
}

#endif