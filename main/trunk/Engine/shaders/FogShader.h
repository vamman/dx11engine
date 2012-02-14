#pragma once
#include "Textureshader.h"
class FogShader : public TextureShader
{
	private:
		struct FogBufferType
		{
			float fogStart;
			float fogEnd;
			float padding1, padding2;
		};

	public:
		FogShader(void);
		~FogShader(void);

		virtual HRESULT Initialize(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname);
		virtual HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
						LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		void Shutdown();

		bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
								 D3DXMATRIX worldMatrix,
								 D3DXMATRIX viewMatrix,
								 D3DXMATRIX projectionMatrix,
								 vector<ID3D11ShaderResourceView*> textureVector, 
								 float fogStart, 
								 float fogEnd,
								 bool isInstanced);

		bool RenderOrdinary(ID3D11DeviceContext* deviceContext,
					int indexCount, 
					D3DXMATRIX worldMatrix, 
					D3DXMATRIX viewMatrix,
					D3DXMATRIX projectionMatrix, 
					vector<ID3D11ShaderResourceView*> textureVector, 
					float fogStart,
					float fogEnd);

		bool RenderInstanced(ID3D11DeviceContext* deviceContext,
					int vertexCount,
					int instanceCount,
					D3DXMATRIX worldMatrix, 
					D3DXMATRIX viewMatrix,
					D3DXMATRIX projectionMatrix, 
					vector<ID3D11ShaderResourceView*> textureVector, 
					float fogStart,
					float fogEnd);

	private:
		ID3D11Buffer* m_fogBuffer;
};

