////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"

Texture::Texture() : m_Width(0), m_Height(0), m_ShaderResourceView(NULL)
{
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
}


bool Texture::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	// Load the texture in.
	D3DX11_IMAGE_LOAD_INFO pImageInfo;
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, &pImageInfo, NULL, &m_ShaderResourceView, NULL);
	if(FAILED(result))
	{
		return false;
	}

	m_Width = pImageInfo.Width;
	m_Height = pImageInfo.Height;

	return true;
}


void Texture::Shutdown()
{
	// Release the texture resource.
	if(m_ShaderResourceView)
	{
		m_ShaderResourceView->Release();
		m_ShaderResourceView = 0;
	}

	return;
}