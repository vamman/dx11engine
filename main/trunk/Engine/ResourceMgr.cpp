#include "ResourceMgr.h"
#include "FileSystemHelper.h"
#include "MacroHelper.h"

#include "TextureLoader.h"
#include "textureclass.h"

#define SVN_DIRECTORY L".svn"

ResourceMgr* ResourceMgr::m_pInstance = NULL;

ResourceMgr* ResourceMgr::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new ResourceMgr;
	}
	return m_pInstance;
}

ResourceMgr::ResourceMgr(void)
{
	ShaderLoader* shaderLoader = new ShaderLoader();
}


ResourceMgr::~ResourceMgr(void)
{
}

bool ResourceMgr::LoadResources()
{
	vector<wstring> files;
	bool result = ListFiles(FileSystemHelper::GetResourceFolder(), L"*", files);
	return result;
}

bool ResourceMgr::ListFiles(wstring path, wstring mask, vector<wstring>& files) 
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	wstring spec;
	stack<wstring> directories;

	directories.push(path);
	files.clear();

	// Go through resource directories and load all content
	while (!directories.empty()) 
	{
		path = directories.top();
		spec = path + L"\\" + mask;
		directories.pop();

		hFind = FindFirstFile(spec.c_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return false;
		} 

		do 
		{
			if (wcscmp(ffd.cFileName, L".") != 0  &&
				wcscmp(ffd.cFileName, L"..") != 0 &&
				wcscmp(ffd.cFileName, SVN_DIRECTORY) != 0) 
			{
				if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					directories.push(path + L"\\" + ffd.cFileName);
				}
				else 
				{
					// Load certain type of resource according to file extension
					char fileNameChars[260];
					WideCharToMultiByte(CP_ACP, 0, ffd.cFileName, -1, fileNameChars, 260, NULL, NULL);
					string fileNameString(fileNameChars);

					wstring resourceName = FileSystemHelper::GetFilenameWithoutExtension(FileSystemHelper::ConvertStringToWString(fileNameString));
					FileSystemHelper::FileExtensions fileExtension = FileSystemHelper::GetFileExtension(FileSystemHelper::ConvertStringToWString(fileNameString));
					
					wstring filePath = path + wstring(L"/") + ffd.cFileName;

					// Load all texures
					if (fileExtension == FileSystemHelper::ExtensionDDS || fileExtension == FileSystemHelper::ExtensionJPG || 
						fileExtension == FileSystemHelper::ExtensionPNG || fileExtension == FileSystemHelper::ExtensionRAW || 
						fileExtension == FileSystemHelper::ExtensionBMP)
					{
						Texture* newTexture = TextureLoader::LoadTexture((WCHAR* )filePath.c_str());
						newTexture->SetResourceName(FileSystemHelper::ConvertWStringToString(resourceName).c_str());
						m_Resources[resourceName] = newTexture;
					}

					// TODO: Load all shaders
					if (fileExtension == FileSystemHelper::ExtensionFX)
					{
						BasicShader* shader = ShaderLoader::LoadShader(filePath, resourceName);
						shader->SetResourceName(FileSystemHelper::ConvertWStringToString(resourceName).c_str());
						m_Resources[resourceName] = shader;
					}

					// TODO: Load all models
					if (fileExtension == FileSystemHelper::ExtensionOBJ)
					{
						
					}

					// TODO: Load all audio
					if (fileExtension == FileSystemHelper::ExtensionWAV)
					{
						
					}

					files.push_back(path + L"\\" + ffd.cFileName);
				}
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES) 
		{
			FindClose(hFind);
			return false;
		}

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}

BasicResource* ResourceMgr::GetResourceByName(wstring name, ResourceType resourceType)
{
	BasicResource* resource = m_Resources[name];
	if (resource != NULL)
	{
		return resource;
	}

	ASSERT(false, L"Resource not found");
	return NULL;
}

void ResourceMgr::Shutdown()
{
	map<wstring, BasicResource* >::iterator iter;

	for (iter = m_Resources.begin(); iter != m_Resources.end(); iter++)
	{
		iter->second->Shutdown();
	}
}
