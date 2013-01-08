#include "TextureLoader.h"

TextureLoader::TextureLoader(void)
{
}


TextureLoader::~TextureLoader(void)
{
}

Texture* TextureLoader::LoadTexture(WCHAR* filePath)
{
	Texture* newTexture = new Texture();

	D3DX11_IMAGE_INFO imageInfo;
	CreateShaderResourceViewFromFile(D3DClass::GetInstance()->GetDevice(),	filePath, newTexture->GetShaderView(), &imageInfo);

	newTexture->SetWidth(imageInfo.Width);
	newTexture->SetHeight(imageInfo.Height);

	return newTexture;
}
bool TextureLoader::CreateShaderResourceViewFromFile(ID3D11Device* device, WCHAR* filename,
	ID3D11ShaderResourceView** shaderResourceView, D3DX11_IMAGE_INFO* imageInfo)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, shaderResourceView, NULL);
	if(FAILED(result))
	{
		return false;
	}

	D3DX11GetImageInfoFromFile(filename, NULL, imageInfo, NULL);

	return true;
}
