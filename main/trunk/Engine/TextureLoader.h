#ifndef _TEXTURE_LOADER_H_
#define _TEXTURE_LOADER_H_

#include <d3d11.h>
#include <d3dx11tex.h>
#include <stdint.h>
#include <memory>
#include <dxgiformat.h>
#include <assert.h>
#include <wincodec.h>

bool CreateShaderResourceViewFromFile(ID3D11Device* device, WCHAR* filename,
									  ID3D11ShaderResourceView** shaderResourceView, D3DX11_IMAGE_INFO* imageInfo);

HRESULT CreateWICTextureFromMemory( _In_ ID3D11Device* d3dDevice,
									_In_opt_ ID3D11DeviceContext* d3dContext,
									_In_bytecount_(wicDataSize) const uint8_t* wicData,
									_In_ size_t wicDataSize,
									_Out_opt_ ID3D11Resource** texture,
									_Out_opt_ ID3D11ShaderResourceView** textureView,
									_In_ size_t maxsize = 0
									);

HRESULT CreateWICTextureFromFile(	_In_ ID3D11Device* d3dDevice,
									_In_opt_ ID3D11DeviceContext* d3dContext,
									_In_z_ const wchar_t* szFileName,
									_Out_opt_ ID3D11Resource** texture,
									_Out_opt_ ID3D11ShaderResourceView** textureView,
									_In_ size_t maxsize = 0
									);

#endif