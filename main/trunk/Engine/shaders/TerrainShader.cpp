#include "TerrainShader.h"

TerrainShader::TerrainShader(void)
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_textureInfoBuffer = 0;
}

TerrainShader::~TerrainShader(void)
{
}

HRESULT TerrainShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	D3D11_BUFFER_DESC textureInfoBufferDesc;
	vector<char *> layouts;

	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("NORMAL");
	layouts.push_back("COLOR");

	result = LightShader::InitializeShader(lightSource, device, hwnd, filename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}

	// Setup the description of the texture info constant buffer that is in the pixel shader.
	textureInfoBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureInfoBufferDesc.ByteWidth = sizeof(TextureInfoBufferType);
	textureInfoBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	textureInfoBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureInfoBufferDesc.MiscFlags = 0;
	textureInfoBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&textureInfoBufferDesc, NULL, &m_textureInfoBuffer);
	if(FAILED(result))
	{
		return result;
	}

	return result;
}

void TerrainShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	LightShader::Shutdown();

	// Release the texture info constant buffer.
	if(m_textureInfoBuffer)
	{
		m_textureInfoBuffer->Release();
		m_textureInfoBuffer = 0;
	}

	return;
}

HRESULT TerrainShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	HRESULT result = LightShader::SetCameraPosition(deviceContext, cameraPosition, lightType);
	return result;
}

bool TerrainShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	bool result = LightShader::SetLightSource(deviceContext, lightSource);
	return result;
}

bool TerrainShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray, bool useAlpha)
{
	// LightShader::SetTextureArray(deviceContext, textureArray);
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	TextureInfoBufferType* dataPtr;
	unsigned int bufferNumber;

	// Set shader texture resources in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &textureArray[0]);

	// If this is a blended polygon then also set the second texture and the alpha map for blending.
	if(useAlpha)
	{
		deviceContext->PSSetShaderResources(1, 1, &textureArray[1]);
		deviceContext->PSSetShaderResources(2, 1, &textureArray[2]);
	}
	else
	{
		deviceContext->PSSetShaderResources(1, 1, &textureArray[0]);
		deviceContext->PSSetShaderResources(2, 1, &textureArray[0]);
	}

	// Lock the texture info constant buffer so it can be written to.
	result = deviceContext->Map(m_textureInfoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (TextureInfoBufferType*)mappedResource.pData;

	// Copy the texture info variables into the constant buffer.
	dataPtr->useAlplha = useAlpha;
	dataPtr->padding2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_textureInfoBuffer, 0);

	// Set the position of the texture info constant buffer in the pixel shader.
	bufferNumber = 1;

	// Finally set the texture info constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_textureInfoBuffer);
	return true;
}

HRESULT TerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,  D3DXMATRIX projectionMatrix, bool isInstanced)
{
	HRESULT result = LightShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);
	return result;
}

void TerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	BasicShader::RenderShaderOrdinary(deviceContext, indexCount);
}