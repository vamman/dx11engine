#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include <windows.h>
#include <string>
#include <vector>
#include <stack>
#include <typeinfo>
#include <iostream>

#include "d3dclass.h"

#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "BasicResource.h"
#include "Log.h"

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

#include <map>

using namespace std;

class ResourceMgr
{
	public:
		enum ResourceType
		{
			ResourceTypeTexture,
			ResourceTypeModel,
			ResourceTypeShader,
			ResourceTypeSound,

			ResourceTypeCount
		};
		~ResourceMgr(void);

		static ResourceMgr* GetInstance();
		bool LoadResources();
		BasicResource* GetResourceByName(wstring name, ResourceType resourceType);
		void Shutdown();

	private:
		ResourceMgr(void);
		bool ListFiles(wstring path, wstring mask, vector<wstring>& files);

	private:
		static ResourceMgr*			m_pInstance;
		map<wstring, BasicResource* > m_Resources;
		map<wstring, BasicShader* > mShadersMap;
};

#endif