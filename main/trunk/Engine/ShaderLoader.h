#ifndef _SHADER_LOADER_H_
#define _SHADER_LOADER_H_

#include <d3d11.h>
#include <d3dx11tex.h>
#include "shaders/BasicShader.h"
#include "d3dclass.h"
#include "FileSystemHelper.h"

// Shaders
#include "Shaders/BasicShader.h"
#include "Shaders/TextureShader.h"
#include "Shaders/LightShader.h"
#include "Shaders/MultitextureShader.h"
#include "Shaders/NormalMapShader.h"
#include "Shaders/SpecMapShader.h"
#include "Shaders/FogShader.h"
#include "Shaders/ReflectionShader.h"
#include "Shaders/TerrainShader.h"
#include "shaders/SkyDomeShader.h"
#include "shaders/SkyPlaneShader.h"
#include "shaders/FireShader.h"
#include "shaders/FontShader.h"
#include "shaders/FireShader.h"
#include "shaders/ParticleShader.h"

#include "MacroHelper.h"

#include <map>

class ShaderLoader
{
public:
	ShaderLoader(void);
	~ShaderLoader(void);
	static BasicShader* LoadShader(wstring filePath, wstring resourceName);

private:
	static map<wstring, BasicShader* > mShadersMap;
	static LightClass* m_DirSpecLight;
	static LightClass* m_DirAmbLight;
	
};

#endif