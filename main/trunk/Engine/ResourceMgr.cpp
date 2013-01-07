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

	// Create directional specular light object.
	LightClass* dirSpecLight = new LightClass;
	if(!dirSpecLight)
	{
		return false;
	}

	dirSpecLight->SetLightType(LightClass::DIRECTIONAL_SPECULAR_LIGHT);
	// Initialize the light object.
	dirSpecLight->SetAmbientColor(0.4f, 0.4f, 0.4f, 1.0f);
	dirSpecLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	dirSpecLight->SetDirection(0.0f, 0.0f, 0.75f);
	dirSpecLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	dirSpecLight->SetSpecularPower(16.0f);

	// Create directional ambient light object.
	LightClass* dirAmbLight = new LightClass;
	if(!dirAmbLight)
	{
		return false;
	}

	dirAmbLight->SetLightType(LightClass::DIRECTIONAL_AMBIENT_LIGHT);
	// Initialize the light object.
	dirAmbLight->SetAmbientColor(0.5f, 0.5f, 0.5f, 1.0f); // 0.5f, 0.5f, 0.5f, 1.0f
	dirAmbLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	dirAmbLight->SetDirection(0.0f, 0.0f, 0.75f); //  -0.5f, -1.0f, 0.0f

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

					// Load all texures
					if (fileExtension == FileSystemHelper::ExtensionDDS || fileExtension == FileSystemHelper::ExtensionJPG || 
						fileExtension == FileSystemHelper::ExtensionPNG || fileExtension == FileSystemHelper::ExtensionRAW || 
						fileExtension == FileSystemHelper::ExtensionBMP)
					{
						Texture* newTexture = new Texture();
						wstring filePath = path + wstring(L"/") + ffd.cFileName;

						D3DX11_IMAGE_INFO imageInfo;
						CreateShaderResourceViewFromFile(D3DClass::GetInstance()->GetDevice(),
														 (WCHAR* )filePath.c_str(), newTexture->GetShaderView(),
														 &imageInfo);

						newTexture->SetWidth(imageInfo.Width);
						newTexture->SetHeight(imageInfo.Height);

						newTexture->SetResourceName(FileSystemHelper::ConvertWStringToString(resourceName).c_str());
						m_Resources[resourceName] = newTexture;
					}

					// TODO: Load all shaders
					if (fileExtension == FileSystemHelper::ExtensionFX)
					{
						wstring filePath = path + wstring(L"/") + ffd.cFileName;
						LoadShader(filePath, resourceName, dirAmbLight, dirSpecLight);
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

HRESULT ResourceMgr::LoadShader(wstring filePath, wstring resourceName, LightClass* lightSource1, LightClass* lightSource2)
{
	HWND hwnd = FindWindow(L"Engine", NULL);

	// Create shader
	BasicShader* shader = mShadersMap[resourceName];
	
	// Initialize method due to shader class instance. 
	if (strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "AmbientLight") == 0 ||
		strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "TerrainWithMaterials") == 0 ||
		strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "TerrainWithQuadTree") == 0 ||
		strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "NormalMapShader") == 0)
	{
		V_RETURN(shader->Initialize(lightSource1, D3DClass::GetInstance()->GetDevice(), hwnd,
			const_cast<WCHAR*>(filePath.c_str()),
			"VertexShaderFunction", "PixelShaderFunction"), L"Error", /*L"Could not initialize the basic shader object."*/ resourceName.c_str());
	}
	else
	{
		V_RETURN(shader->Initialize(lightSource2, D3DClass::GetInstance()->GetDevice(), hwnd,
			const_cast<WCHAR*>(filePath.c_str()),
			"VertexShaderFunction", "PixelShaderFunction"), L"Error", /*L"Could not initialize the basic shader object."*/ resourceName.c_str());
	}
	
	shader->SetResourceName(FileSystemHelper::ConvertWStringToString(resourceName).c_str());
	m_Resources[resourceName] = shader;
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
