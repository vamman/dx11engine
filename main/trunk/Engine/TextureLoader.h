#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_

#include <d3d11.h>
#include <d3dx11tex.h>
#include "textureclass.h"
#include "d3dclass.h"
#include "FileSystemHelper.h"
//#include <stdint.h>
//#include <memory>
//#include <dxgiformat.h>
//#include <assert.h>
//#include <wincodec.h>

class TextureLoader
{
	public:
		TextureLoader();
		~TextureLoader();
		static Texture* LoadTexture(WCHAR* filePath);
		//static bool CreateShaderResourceViewFromFile(ID3D11Device* device, WCHAR* filename,
		//											 ID3D11ShaderResourceView** shaderResourceView, D3DX11_IMAGE_INFO* imageInfo);
	private:
		static bool CreateShaderResourceViewFromFile(ID3D11Device* device, WCHAR* filename,
											  ID3D11ShaderResourceView** shaderResourceView, D3DX11_IMAGE_INFO* imageInfo);
};

#endif