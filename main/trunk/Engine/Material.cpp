#include "Material.h"


Material::Material()
{
	mTextureVector = new TextureArrayClass();
	mShader = 0;
}

Material::Material(char* materialName)
{
	mTextureVector = new TextureArrayClass();
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

bool Material::AppentTextureToMaterial(ID3D11Device* device, WCHAR* textureName)
{
	bool result;
	// Load the textures for this model.
	result = LoadTexture(device, textureName);
	if(!result)
	{
		return false;
	}
	return true;
}

bool Material::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	TextureClass* texture = new TextureClass;
	if(!texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	result = mTextureVector->AddTexture(texture->GetTexture());

	return true;
}

vector<ID3D11ShaderResourceView*>& Material::GetTextureVector()
{
	return mTextureVector->GetTextureArray();
}

char* Material::GetMaterialName()
{
	return mMaterialName;
}

void Material::Shutdown()
{
	mTextureVector->Shutdown();
}