#include "ResourceMgr.h"
#include "FileSystemHelper.h"
#include "graphicsclass.h"

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
	: m_ModelLoader(0)
	, m_ShaderLoader(0)
	, m_TextureFirstIndex(-1)
	, m_TextureLastIndex(-1)
{
}


ResourceMgr::~ResourceMgr(void)
{
}

bool ResourceMgr::LoadResources()
{
	vector<wstring> files;

	bool result = ListFiles(GetResourceFolder(), L"*", files);
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

					string resourceName = GetFilenameWithoutExtension(fileNameString);
					FileExtensions fileExtension = GetFileExtension(fileNameString);

					// Load all texures
					if (fileExtension == ExtensionDDS || fileExtension == ExtensionJPG || 
						fileExtension == ExtensionPNG || fileExtension == ExtensionRAW || 
						fileExtension == ExtensionBMP)
					{
						// Set first index for textures
						if (m_TextureFirstIndex == -1)
						{
							m_TextureFirstIndex = m_Resources.size();
						}

						Texture* newTexture = new Texture();
						wstring filePath = path + wstring(L"/") + ffd.cFileName;

						/*
						CreateWICTextureFromFile(D3DClass::GetInstance()->GetDevice(),
												 D3DClass::GetInstance()->GetDeviceContext(),
												 filePath.c_str(), &resource, &shaderResourceView);
						*/

						CreateShaderResourceViewFromFile(D3DClass::GetInstance()->GetDevice(),
														 (WCHAR* )filePath.c_str(), newTexture->GetShaderView());

						newTexture->SetResourceName(resourceName.c_str());
						m_Resources.push_back(newTexture);
					}

					// Load all models
					if (fileExtension == ExtensionOBJ)
					{
						if (m_TextureFirstIndex != -1 && m_TextureLastIndex == -1)
						{
							m_TextureLastIndex =  m_Resources.size();
						}
					}

					// Load all shaders
					if (fileExtension == ExtensionFX)
					{
						if (m_TextureFirstIndex != -1 && m_TextureLastIndex == -1)
						{
							m_TextureLastIndex =  m_Resources.size();
						}
					}

					// Load all audio
					if (fileExtension == ExtensionWAV)
					{
						if (m_TextureFirstIndex != -1 && m_TextureLastIndex == -1)
						{
							m_TextureLastIndex =  m_Resources.size();
						}
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

BasicResource* ResourceMgr::GetResourceByName(string name, ResourceType resourceType)
{
	switch (resourceType)
	{
		case ResourceTypeTexture:
		{
			return FindResourceByName(name, m_TextureFirstIndex, m_TextureLastIndex);
			break;
		}
		default:
			Log::GetInstance()->WriteTextMessageToOutput("No such type of resource !");
	}
}

BasicResource* ResourceMgr::FindResourceByName(string name, int firstIndex, int lastIndex)
{
	for (int i = firstIndex; i < lastIndex; ++i)
	{
		if (strcmp(m_Resources[i]->GetResourceName(), name.c_str()) == 0)
		{
			return m_Resources[i];
		}
	}
	ASSERT(false, L"Resource not found");
	return NULL;
}