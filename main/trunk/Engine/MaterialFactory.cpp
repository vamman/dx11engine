#include "MaterialFactory.h"

MaterialFactory* MaterialFactory::mInstance = NULL;

MaterialFactory* MaterialFactory::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new MaterialFactory;
	}
	return mInstance;
}

MaterialFactory::MaterialFactory(void)
{
	Log::GetInstance()->WriteTimedMessageToOutput(100, "MaterialFactory::MaterialFactory");
}

MaterialFactory::~MaterialFactory(void)
{

}

Material* MaterialFactory::CreateMaterial(char* materialName)
{
	Material* material = new Material(materialName);
	if (!material)
	{
		return false;
	}
	mVectorOfMaterials.push_back(material);
	return material;
}

Material* MaterialFactory::GetMaterialByName(char* materialName)
{
	vector<Material*>::iterator materiallIt;
	for (materiallIt = mVectorOfMaterials.begin(); materiallIt != mVectorOfMaterials.end(); ++materiallIt)
	{
		if (strcmp((*materiallIt)->GetMaterialName(), materialName) == 0)
		{
			return (*materiallIt);
		}
	}
	return 0;
}

vector<Material*>& MaterialFactory::GetVectorOfMaterials()
{
	return mVectorOfMaterials;
}

void MaterialFactory::Shutdown()
{
	vector<Material*>::iterator materialIt;
	for (materialIt = mVectorOfMaterials.begin(); materialIt != mVectorOfMaterials.end(); ++materialIt)
	{
		(*materialIt)->Shutdown();
	}

	return;
}