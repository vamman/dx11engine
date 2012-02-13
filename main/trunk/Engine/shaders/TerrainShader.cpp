#include "TerrainShader.h"

TerrainShader::TerrainShader(void)
{
}

TerrainShader::~TerrainShader(void)
{
}

bool TerrainShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	// result = LightShader::Initialize(lightSource, device, hwnd, filename, VSname, PSname);
	vector<char *> layouts;

	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("NORMAL");
	layouts.push_back("COLOR");

	result = LightShader::InitializeShader(lightSource, device, hwnd, filename, VSname, PSname, layouts);
	if (!result)
	{
		return false;
	}
	return true;
}

void TerrainShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	LightShader::Shutdown();
	return;
}

bool TerrainShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	bool result = LightShader::SetCameraPosition(deviceContext, cameraPosition, lightType);
	return result;
}

bool TerrainShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	bool result = LightShader::SetLightSource(deviceContext, lightSource);
	return result;
}

void TerrainShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	LightShader::SetTextureArray(deviceContext, textureArray);
}

bool TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,  D3DXMATRIX projectionMatrix, bool isInstanced)
{
	bool result = LightShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);
	return result;
}

void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	BasicShader::RenderShaderOrdinary(deviceContext, indexCount);
}