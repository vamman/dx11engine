#include "NormalMapShader.h"


NormalMapShader::NormalMapShader(void) : LightShader()
{
}


NormalMapShader::~NormalMapShader(void)
{
}

void NormalMapShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	LightShader::Shutdown();
	return;
}

HRESULT NormalMapShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;

	vector<char *> layouts;
	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("TEXCOORD_INST");
	layouts.push_back("NORMAL");
	layouts.push_back("TANGENT");
	layouts.push_back("BINORMAL");

	result = LightShader::InitializeShader(lightSource, device, hwnd, filename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}
	return result;
}

void NormalMapShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	LightShader::SetTextureArray(deviceContext, textureArray);
}

HRESULT NormalMapShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	HRESULT result = LightShader::SetCameraPosition(deviceContext, cameraPosition, lightType);
	return result;
}

bool NormalMapShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	bool result = LightShader::SetLightSource(deviceContext, lightSource);
	return result;
}

bool NormalMapShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
							 int vertexCount,
							 int instanceCount,
							 D3DXMATRIX worldMatrix,
							 D3DXMATRIX viewMatrix, 
							 D3DXMATRIX projectionMatrix)
{
	bool result;

	result = LightShader::RenderInstanced(deviceContext, 
								 vertexCount,
								 instanceCount,
								 worldMatrix, 
								 viewMatrix, 
								 projectionMatrix);

	if (!result)
	{
		return false;
	}

	return result;
}

bool NormalMapShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
							 int indexCount,
							 D3DXMATRIX worldMatrix,
							 D3DXMATRIX viewMatrix, 
							 D3DXMATRIX projectionMatrix)
{
	bool result;

	result = LightShader::RenderOrdinary(deviceContext, 
								 indexCount,
								 worldMatrix, 
								 viewMatrix, 
								 projectionMatrix);
	if (!result)
	{
		return false;
	}

	return result;
}