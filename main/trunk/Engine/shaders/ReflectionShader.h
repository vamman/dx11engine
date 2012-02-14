#pragma once
#include "TextureShader.h"
class ReflectionShader :
	public TextureShader
{
	private:
		struct ReflectionBufferType
		{
			D3DXMATRIX reflectionMatrix;
		};

	public:
		ReflectionShader(void);
		~ReflectionShader(void);

		virtual HRESULT Initialize(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname);
		void Shutdown();
		bool RenderInstanced(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount, D3DXMATRIX worldMatrix, 
					D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,  vector<ID3D11ShaderResourceView*> textureVector,
					ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix);

		bool RenderOrdinary(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
			D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,  vector<ID3D11ShaderResourceView*> textureVector,
			ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix);

	private:
		void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);
		bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
			D3DXMATRIX projectionMatrix, vector<ID3D11ShaderResourceView*> textureVector,
			ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix, bool isInstanced);
		void RenderShaderOrdinary(ID3D11DeviceContext*, int);

	private:
		ID3D11Buffer* m_reflectionBuffer;
};

