#include "ShaderLoader.h"

LightClass* ShaderLoader::m_DirSpecLight = NULL;
LightClass* ShaderLoader::m_DirAmbLight = NULL;
map<wstring, BasicShader* > ShaderLoader::mShadersMap = map<wstring, BasicShader* >();

ShaderLoader::ShaderLoader()
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

	ParticleShader* particleShader = new ParticleShader;
	mShadersMap[L"ParticleShader"] = particleShader;


	// Create directional specular light object.
	m_DirSpecLight = new LightClass;
	m_DirSpecLight->SetLightType(LightClass::DIRECTIONAL_SPECULAR_LIGHT);

	// Create directional ambient light object.
	m_DirAmbLight = new LightClass;
	m_DirAmbLight->SetLightType(LightClass::DIRECTIONAL_AMBIENT_LIGHT);
}


ShaderLoader::~ShaderLoader()
{
}

BasicShader* ShaderLoader::LoadShader(wstring filePath, wstring resourceName)
{
	HWND hwnd = FindWindow(L"Engine", NULL);
	wstring messageStr = L"Could not initialize shader: " + resourceName;

	// Create shader
	BasicShader* shader = mShadersMap[resourceName];

	// Initialize method due to shader class instance. 
	if (strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "AmbientLight") == 0 ||
		strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "TerrainWithMaterials") == 0 ||
		strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "TerrainWithQuadTree") == 0 ||
		strcmp(FileSystemHelper::ConvertWStringToString(resourceName).c_str(), "NormalMapShader") == 0)
	{
		V_RETURN(shader->Initialize(m_DirAmbLight, D3DClass::GetInstance()->GetDevice(), hwnd,
			const_cast<WCHAR*>(filePath.c_str()),
			"VertexShaderFunction", "PixelShaderFunction"), L"Error", messageStr.c_str());
	}
	else
	{
		V_RETURN(shader->Initialize(m_DirSpecLight, D3DClass::GetInstance()->GetDevice(), hwnd,
			const_cast<WCHAR*>(filePath.c_str()),
			"VertexShaderFunction", "PixelShaderFunction"), L"Error", messageStr.c_str());
	}

	return shader;
}
