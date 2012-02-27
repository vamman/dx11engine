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

HRESULT SpecMapShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;

	result = NormalMapShader::Initialize(lightSource, device, hwnd, filename, VSname, PSname);
	if (FAILED(result))
	{
		return result;
	}
	return result;
}

void SpecMapShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	NormalMapShader::SetTextureArray(deviceContext, textureArray);
}

HRESULT SpecMapShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	HRESULT result = LightShader::SetCameraPosition(deviceContext, cameraPosition, lightType);
	return result;
}

bool SpecMapShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	bool result = LightShader::SetLightSource(deviceContext, lightSource);
	return result;
}

HRESULT SpecMapShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
								    int vertexCount,
								    int instanceCount,
								    D3DXMATRIX worldMatrix,
								    D3DXMATRIX viewMatrix, 
								    D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	result = NormalMapShader::RenderInstanced(deviceContext,
											  vertexCount,
											  instanceCount,
											  worldMatrix,
											  viewMatrix, 
											  projectionMatrix);

	if (FAILED(result)) { return result; }

	return result;
}

HRESULT SpecMapShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
						int indexCount,
						D3DXMATRIX worldMatrix,
						D3DXMATRIX viewMatrix, 
						D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	result = NormalMapShader::RenderOrdinary(deviceContext,
									indexCount,
									worldMatrix,
									viewMatrix, 
									projectionMatrix);

	if (FAILED(result)) { return result; }

	return result;
}

