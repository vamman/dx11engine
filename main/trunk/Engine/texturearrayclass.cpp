////////////////////////////////////////////////////////////////////////////////
// Filename: texturearrayclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "texturearrayclass.h"

TextureArrayClass::TextureArrayClass()
{
	m_textures.reserve(2);
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}


TextureArrayClass::~TextureArrayClass()
{
}

bool TextureArrayClass::AddTexture(ID3D11ShaderResourceView* texture)
{
	int oldSize, newSize;
	oldSize = m_textures.size();
	m_textures.push_back(texture);
	newSize = m_textures.size();
	if (newSize > oldSize)
	{
		return true;
	}
	return false;
}

bool TextureArrayClass::RemoveTexture(ID3D11ShaderResourceView* texture)
{
	vector<ID3D11ShaderResourceView*>::iterator it;
	it = find(m_textures.begin(), m_textures.end(), texture);
	if ( it ==  m_textures.end() )
	{
		return false;
	}
	m_textures.erase(it);
	return true;
}

void TextureArrayClass::Shutdown()
{
	m_textures.clear();
	return;
}

vector<ID3D11ShaderResourceView*>& TextureArrayClass::GetTextureArray()
{
	return m_textures;
}