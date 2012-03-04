#pragma once

#include <vector>
#include "Material.h"
#include "Log.h"

using namespace std;

// TODO: Make this class a singleton
class MaterialFactory
{
	public:
		static MaterialFactory* GetInstance();
		Material* CreateMaterial(char* materialName);
		Material* GetMaterialByName(char* materialName);
		vector<Material*>& GetVectorOfMaterials();
		void Shutdown();

	private:
		MaterialFactory(void);
		~MaterialFactory(void);
		vector<Material*> mVectorOfMaterials;
		static MaterialFactory* mInstance;
};

