#pragma once
#include "BasicShader.h"
class TextureShader : public BasicShader
{
	public:
		TextureShader(void);
		~TextureShader(void);

		virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
							  LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		virtual void Shutdown();

		virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
										 D3DXMATRIX projectionMatrix, bool isInstanced);
		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);

		virtual HRESULT RenderInstanced(ID3D11DeviceContext* deviceContext,
							int vertexCount,
							int instanceCount,
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix);

		virtual HRESULT RenderOrdinary(ID3D11DeviceContext* deviceContext,
							int indexCount,
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix);

		ID3D11SamplerState* m_sampleState;

	private:

		// ID3D11SamplerState* m_sampleState;
};

