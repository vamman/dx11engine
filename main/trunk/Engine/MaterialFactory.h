#pragma once

#include <vector>
#include "Material.h"

using namespace std;

class MaterialFactory
{
	public:
		MaterialFactory(void);
		~MaterialFactory(void);
		Material* CreateMaterial(char* materialName);
		Material* GetMaterialByName(char* materialName);
		vector<Material*>& GetVectorOfMaterials();
		void Shutdown();

	private:
		vector<Material*> mVectorOfMaterials;
};

