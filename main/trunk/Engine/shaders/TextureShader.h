#pragma once
#include "BasicShader.h"
class TextureShader : public BasicShader
{
	public:
		TextureShader(void);
		~TextureShader(void);

		virtual bool Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
							  LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		virtual void Shutdown();

		virtual bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
										 D3DXMATRIX projectionMatrix, bool isInstanced);
		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);

		virtual bool RenderInstanced(ID3D11DeviceContext* deviceContext,
							int vertexCount,
							int instanceCount,
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix);

		virtual bool RenderOrdinary(ID3D11DeviceContext* deviceContext,
							int indexCount,
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix);

	private:

		ID3D11SamplerState* m_sampleState;
};

