////////////////////////////////////////////////////////////////////////////////
// Filename: texturearrayclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTUREARRAYCLASS_H_
#define _TEXTUREARRAYCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11tex.h>

#include <vector>
#include <algorithm>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureArrayClass
////////////////////////////////////////////////////////////////////////////////
class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	void Shutdown();

	vector<ID3D11ShaderResourceView*>& GetTextureArray();
	bool AddTexture(ID3D11ShaderResourceView* texture);
	bool RemoveTexture(ID3D11ShaderResourceView* texture);

private:
	vector<ID3D11ShaderResourceView*> m_textures;
};

#endif