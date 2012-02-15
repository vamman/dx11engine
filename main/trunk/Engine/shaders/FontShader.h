#pragma once

#include "TextureShader.h"

class FontShader :public TextureShader
{
	public:
		FontShader(void);
		~FontShader(void);
		virtual void Shutdown();
		virtual HRESULT Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
							  LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);
		HRESULT SetPixelBufferColor(ID3D11DeviceContext* deviceContext, D3DXVECTOR4 pixelColor);
		virtual bool RenderOrdinary(ID3D11DeviceContext* deviceContext,
									int indexCount,
									D3DXMATRIX worldMatrix,
									D3DXMATRIX viewMatrix, 
									D3DXMATRIX projectionMatrix);

	private:
		virtual bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);
		struct PixelBufferType
		{
			D3DXVECTOR4 pixelColor;
		};

	private:
		ID3D11Buffer* m_pixelBuffer;
};

