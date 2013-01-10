#include "ParticleShader.h"


ParticleShader::ParticleShader(void)
{
}


ParticleShader::~ParticleShader(void)
{
}

HRESULT ParticleShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;

	vector<char *> layouts;
	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("COLOR");

	result = InitializeShader(device, hwnd, filename, VSname, PSname, layouts);

	if (FAILED(result))
	{
		return result;
	}
	return result;
}

HRESULT ParticleShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
	LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	D3D11_BUFFER_DESC pixelBufferDesc;

	result = TextureShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}
}

void ParticleShader::Shutdown()
{
	TextureShader::Shutdown();
}

HRESULT ParticleShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	result = TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	return result;
}

void ParticleShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	TextureShader::SetTextureArray(deviceContext, textureArray);
}

HRESULT ParticleShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
	int indexCount,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if(FAILED(result)) { return result; }

	result = TextureShader::RenderOrdinary(deviceContext, 
		indexCount,
		worldMatrix,
		viewMatrix,
		projectionMatrix);
	if(FAILED(result)) { return result; }

	return result;
}
