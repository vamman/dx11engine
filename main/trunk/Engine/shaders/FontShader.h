#pragma once

#include "TextureShader.h"

class FontShader :public TextureShader
{
	public:
		FontShader(void);
		~FontShader(void);
		virtual void Shutdown();
		virtual bool Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
							  LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		bool Render(ID3D11DeviceContext* deviceContext,
					int indexCount,
					D3DXMATRIX worldMatrix,
					D3DXMATRIX viewMatrix, 
					D3DXMATRIX projectionMatrix,
					ID3D11ShaderResourceView* texture,
					D3DXVECTOR4 pixelColor);

	private:
		virtual bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX,
			D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);
		struct PixelBufferType
		{
			D3DXVECTOR4 pixelColor;
		};

	private:
		ID3D11Buffer* m_pixelBuffer;
};

