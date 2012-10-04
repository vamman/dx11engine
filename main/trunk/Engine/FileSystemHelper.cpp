#include "FileSystemHelper.h"

namespace FileSystemHelper
{
	FileExtensions GetFileExtension(wstring fileName)
	{
		char* allExtensions[] = {EXTENSION_DDS, EXTENSION_RAW, EXTENSION_BMP, EXTENSION_JPG, EXTENSION_PNG, 
			EXTENSION_TXT, EXTENSION_OBJ, EXTENSION_FX, EXTENSION_VS, EXTENSION_PS, EXTENSION_WAV, EXTENSION_MP3};

		wstring extension = fileName.substr(fileName.find_last_of(L".") + 1);

		for (int i = 0; i < ExtensionCount; ++i)
		{
			if (strcmp(FileSystemHelper::ConvertWStringToString(extension).c_str(), allExtensions[i]) == 0)
			{
				return FileExtensions(i);
			}
		}

		return ExtensionInvalid;
	}
}