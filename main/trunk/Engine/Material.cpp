#include "Material.h"
#include "ResourceMgr.h"

Material::Material()
{
	mShader = 0;
}

Material::Material(char* materialName)
{
	strcpy_s(mMaterialName, materialName);
}

Material::~Material(void)
{

}

void Material::SetMaterialShader(BasicShader* shader)
{
	mShader = shader;
}

BasicShader* Material::GetMaterialShader()
{
	return mShader;
}

bool Material::AppentTextureToMaterial(ID3D11Device* device, wchar_t* textureName)
{
	bool result;

	Texture* texture = new Texture;
	texture = reinterpret_cast<Texture* >(ResourceMgr::GetInstance()->GetResourceByName(textureName, ResourceMgr::ResourceTypeTexture));
	mTextureVector.push_back(texture->GetShaderResourceView());

	return true;
}

vector<ID3D11ShaderResourceView*>& Material::GetTextureVector()
{
	return mTextureVector;
}

char* Material::GetMaterialName()
{
	return mMaterialName;
}

void Material::Shutdown()
{
	mTextureVector.clear();
}