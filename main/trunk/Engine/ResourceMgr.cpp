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
	: m_ModelLoader(0)
	, m_ShaderLoader(0)
	, m_TextureFirstIndex(-1)
	, m_TextureLastIndex(-1)
{
	LightShader* ambientLightShader = new LightShader;
	mShadersMap[L"AmbientLight"] = ambientLightShader;

	BasicShader* basicShader = new BasicShader;
	mShadersMap[L"BasicShader"] = basicShader;

	FontShader* cursorShader = new FontShader;
	mShadersMap[L"CursorShader"] = cursorShader;

	FireShader* fireShader = new FireShader;
	mShadersMap[L"FireShader"] = fireShader;

	FogShader* fogShader = new FogShader;
	mShadersMap[L"FogShader"] = fogShader;

	FontShader* fontShader = new FontShader;
	mShadersMap[L"FontShader"] = fontShader;

	MultitextureShader* lightMapShader = new MultitextureShader;
	mShadersMap[L"LightmapShader"] = lightMapShader;

	MultitextureShader* multitextureShader = new MultitextureShader;
	mShadersMap[L"MultitextureShader"] = multitextureShader;

	NormalMapShader* normalMapShader = new NormalMapShader;
	mShadersMap[L"NormalMapShader"] = normalMapShader;

	LightShader* pointLightShader = new LightShader;
	mShadersMap[L"PointLight"] = pointLightShader;

	ReflectionShader* reflectionShader = new ReflectionShader;
	mShadersMap[L"ReflectionShader"] = reflectionShader;

	SkyDomeShader* skyDomeShader = new SkyDomeShader;
	mShadersMap[L"SkyDomeShader"] = skyDomeShader;

	SkyPlaneShader* skyPlaneShader = new SkyPlaneShader;
	mShadersMap[L"SkyPlaneShader"] = skyPlaneShader;

	SpecMapShader* specMapShader = new SpecMapShader;
	mShadersMap[L"SpecMapShader"] = specMapShader;

	SpecMapShader* specMapShaderNonInstanced = new SpecMapShader;
	mShadersMap[L"SpecMapShaderNonInstanced"] = specMapShaderNonInstanced;

	LightShader* specularLightShader = new LightShader;
	mShadersMap[L"SpecularLight"] = specularLightShader;

	TerrainShader* terrainWithMaterialShader = new TerrainShader;
	mShadersMap[L"TerrainWithMaterials"] = terrainWithMaterialShader;

	TerrainShader* terrainWithQuadTreeShader = new TerrainShader;
	mShadersMap[L"TerrainWithQuadTree"] = terrainWithQuadTreeShader;

	TextureShader* textureShaderInstanced = new TextureShader;
	mShadersMap[L"TextureShaderInstanced"] = textureShaderInstanced;

	TextureShader* textureShaderNonInstanced = new TextureShader;
	mShadersMap[L"TextureShaderNonInstanced"] = textureShaderNonInstanced;	
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

					// Load all texures
					if (fileExtension == FileSystemHelper::ExtensionDDS || fileExtension == FileSystemHelper::ExtensionJPG || 
						fileExtension == FileSystemHelper::ExtensionPNG || fileExtension == FileSystemHelper::ExtensionRAW || 
						fileExtension == FileSystemHelper::ExtensionBMP)
					{
						// Set first index for textures
						if (m_TextureFirstIndex == -1)
						{
							m_TextureFirstIndex = m_Resources.size();
						}

						Texture* newTexture = new Texture();
						wstring filePath = path + wstring(L"/") + ffd.cFileName;

						D3DX11_IMAGE_INFO imageInfo;
						CreateShaderResourceViewFromFile(D3DClass::GetInstance()->GetDevice(),
														 (WCHAR* )filePath.c_str(), newTexture->GetShaderView(),
														 &imageInfo);

						newTexture->SetWidth(imageInfo.Width);
						newTexture->SetHeight(imageInfo.Height);

						newTexture->SetResourceName(FileSystemHelper::ConvertWStringToString(resourceName).c_str());
						m_Resources.push_back(newTexture);
					}

					// TODO: Load all shaders
					if (fileExtension == FileSystemHelper::ExtensionFX)
					{
						if (m_TextureFirstIndex != -1 && m_TextureLastIndex == -1)
						{
							m_TextureLastIndex =  m_Resources.size();
						}

						wstring filePath = path + wstring(L"/") + ffd.cFileName;
						if (strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "AmbientLight") != 0)
						{
							LoadShader(filePath, resourceName);
						}
					}

					// TODO: Load all models
					if (fileExtension == FileSystemHelper::ExtensionOBJ)
					{
						if (m_TextureFirstIndex != -1 && m_TextureLastIndex == -1)
						{
							m_TextureLastIndex =  m_Resources.size();
						}
					}

					// TODO: Load all audio
					if (fileExtension == FileSystemHelper::ExtensionWAV)
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

// TODO: Rename all vertex and pixel shader functions the same way (for example VertextShader(...) and PixelShader(...)), so Resource Manager can load all shaders inspite of its function names.
// TODO: Check if common shader loading is possible along with different layouts needed for each shader...
HRESULT ResourceMgr::LoadShader(wstring filePath, wstring resourceName)
{
	HWND hwnd = FindWindow(L"Engine", NULL);

	// Create shader
	//BasicShader* shader = mShadersMap[resourceName];
	V_RETURN(mShadersMap[resourceName]->Initialize(D3DClass::GetInstance()->GetDevice(), hwnd,
		const_cast<WCHAR*>(filePath.c_str()),
		"LightVertexShader", "LightPixelShader"), L"Error", L"Could not initialize the basic shader object.");

	mShadersMap[resourceName]->SetResourceName(FileSystemHelper::ConvertWStringToString(resourceName).c_str());
	m_Resources.push_back(mShadersMap[resourceName]);
}

BasicResource* ResourceMgr::GetResourceByName(wstring name, ResourceType resourceType)
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

BasicResource* ResourceMgr::FindResourceByName(wstring name, int firstIndex, int lastIndex)
{
	for (int i = firstIndex; i < lastIndex; ++i)
	{
		if (strcmp(m_Resources[i]->GetResourceName(), FileSystemHelper::ConvertWStringToString(name).c_str()) == 0)
		{
			return m_Resources[i];
		}
	}
	ASSERT(false, L"Resource not found");
	return NULL;
}