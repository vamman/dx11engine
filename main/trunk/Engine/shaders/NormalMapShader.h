#pragma once
#include "lightshader.h"
class NormalMapShader : public LightShader
{
	public:
		NormalMapShader();
		NormalMapShader(const NormalMapShader&);
		~NormalMapShader();

		virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		void Shutdown();

		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);
		virtual HRESULT SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType);
		virtual bool SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource);

		HRESULT RenderInstanced(ID3D11DeviceContext* deviceContext,
								int vertexCount,
								int instanceCount,
								D3DXMATRIX worldMatrix,
								D3DXMATRIX viewMatrix, 
								D3DXMATRIX projectionMatrix);

		HRESULT RenderOrdinary(ID3D11DeviceContext* deviceContext,
								int indexCount,
								D3DXMATRIX worldMatrix,
								D3DXMATRIX viewMatrix, 
								D3DXMATRIX projectionMatrix);
};
