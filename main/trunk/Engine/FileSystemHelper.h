#ifndef _FILE_SYSTEM_HELPER_H_
#define _FILE_SYSTEM_HELPER_H_

#include <string>

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

FileExtensions	GetFileExtension(string fileName);
string			GetFilenameWithoutExtension(string fileName);

wstring GetResourceFolder();

inline wstring GetResourceFolder()
{
	return L"Engine/data";
}

inline string GetFilenameWithoutExtension(string fileName)
{
	int lastSlashPos = fileName.find_last_of("/");
	int strLength = fileName.find_last_of(".") - (lastSlashPos + 1);
	return fileName.substr(lastSlashPos + 1, strLength);
}

#endif