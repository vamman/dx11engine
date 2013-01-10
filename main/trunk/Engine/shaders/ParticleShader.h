#pragma once

#include "TextureShader.h"
#include "..\ModelObject.h"
#include "..\d3dclass.h"
#include "..\cameraclass.h"

class ParticleShader :public TextureShader
{
public:
	ParticleShader(void);
	~ParticleShader(void);
	virtual void Shutdown();
	virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
	HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
		LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
	virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);
	virtual HRESULT RenderOrdinary(ID3D11DeviceContext* deviceContext,
		int indexCount,
		D3DXMATRIX worldMatrix,
		D3DXMATRIX viewMatrix, 
		D3DXMATRIX projectionMatrix);

private:
	virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

private:
};