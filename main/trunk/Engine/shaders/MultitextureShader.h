#pragma once
#include "TextureShader.h"

class MultitextureShader : public TextureShader
{
	public:
		MultitextureShader(void);
		~MultitextureShader(void);
		virtual HRESULT Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);

		virtual HRESULT RenderInstanced(ID3D11DeviceContext* deviceContext,
										int vertexCount,
										int instanceCount,
										D3DXMATRIX worldMatrix, 
										D3DXMATRIX viewMatrix,
										D3DXMATRIX projectionMatrix,
										vector<ID3D11ShaderResourceView*>& textureArray);

		virtual HRESULT RenderOrdinary(ID3D11DeviceContext* deviceContext,
										int indexCount,
										D3DXMATRIX worldMatrix, 
										D3DXMATRIX viewMatrix,
										D3DXMATRIX projectionMatrix,
										vector<ID3D11ShaderResourceView*>& textureArray);

		virtual void Shutdown();

	private:
		virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
			D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
			vector<ID3D11ShaderResourceView*>& textureArray, bool isInstanced);
		ID3D11InputLayout* m_layout;
};
