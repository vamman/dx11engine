#pragma once

#include <vector>
#include "textureclass.h"
#include "texturearrayclass.h"
#include "Shaders/BasicShader.h"

using namespace std;

class Material
{
	public:
		Material();
		Material(char* materialName);
		~Material();
		bool AppentTextureToMaterial(ID3D11Device* device, WCHAR* textureName);
		vector<ID3D11ShaderResourceView*>& GetTextureVector();
		char* GetMaterialName();
		void Shutdown();
		void SetMaterialShader(BasicShader* shader);
		BasicShader* GetMaterialShader();

	private:
		bool LoadTexture(ID3D11Device* device, WCHAR* filename);
		char mMaterialName[20];
		TextureArrayClass* mTextureVector;
		BasicShader* mShader;
};

