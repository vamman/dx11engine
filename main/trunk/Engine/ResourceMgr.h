#ifndef _RESOURCEMGR_H_
#define _RESOURCEMGR_H_

#include <windows.h>
#include <string>
#include <vector>
#include <stack>
#include <iostream>

#include "d3dclass.h"

#include "ShaderLoader.h"
#include "ModelLoader.h"
#include "BasicResource.h"
#include "Log.h"

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

	private:
		ResourceMgr(void);
		BasicResource* FindResourceByName(wstring name, int firstIndex, int lastIndex);
		bool ListFiles(wstring path, wstring mask, vector<wstring>& files);

	private:
		static ResourceMgr*		m_pInstance;
		ModelLoader*			m_ModelLoader;
		ShaderLoader*			m_ShaderLoader;
		vector<BasicResource* > m_Resources;
		int						m_TextureFirstIndex;
		int						m_TextureLastIndex;
};

#endif