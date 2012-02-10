#include "SpecMapShader.h"


SpecMapShader::SpecMapShader(void) : NormalMapShader()
{

}

SpecMapShader::~SpecMapShader(void)
{

}

void SpecMapShader::Shutdown()
{
	NormalMapShader::Shutdown();
}

bool SpecMapShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	bool result;

	result = NormalMapShader::Initialize(lightSource, device, hwnd, filename, VSname, PSname);
	if (!result)
	{
		return false;
	}
	return true;
}

void SpecMapShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	NormalMapShader::SetTextureArray(deviceContext, textureArray);
}

bool SpecMapShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	bool result = LightShader::SetCameraPosition(deviceContext, cameraPosition, lightType);
	return result;
}

bool SpecMapShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	bool result = LightShader::SetLightSource(deviceContext, lightSource);
	return result;
}

bool SpecMapShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
								    int vertexCount,
								    int instanceCount,
								    D3DXMATRIX worldMatrix,
								    D3DXMATRIX viewMatrix, 
								    D3DXMATRIX projectionMatrix)
{
	bool result;

	result = NormalMapShader::RenderInstanced(deviceContext,
											  vertexCount,
											  instanceCount,
											  worldMatrix,
											  viewMatrix, 
											  projectionMatrix);

	if (!result)
	{
		return false;
	}

	return true;
}

bool SpecMapShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
						int indexCount,
						D3DXMATRIX worldMatrix,
						D3DXMATRIX viewMatrix, 
						D3DXMATRIX projectionMatrix)
{
	bool result;

	result = NormalMapShader::RenderOrdinary(deviceContext,
									indexCount,
									worldMatrix,
									viewMatrix, 
									projectionMatrix);

	if (!result)
	{
		return false;
	}

	return true;
}

