#include "SkyPlaneShader.h"


SkyPlaneShader::SkyPlaneShader(void)
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_skyBuffer = 0;
}


SkyPlaneShader::~SkyPlaneShader(void)
{
}

HRESULT SkyPlaneShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	vector<char *> layouts;

	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");

	result = InitializeShader(device, hwnd, filename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}
	
	return result;
}

HRESULT SkyPlaneShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, 
	vector<char *>& layouts)
{
	HRESULT result;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC skyBufferDesc;

	result = BasicShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if(FAILED(result))
	{
		return result;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		return result;
	}

	// Setup the description of the sky constant buffer that is in the pixel shader.
	skyBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	skyBufferDesc.ByteWidth = sizeof(SkyBufferType);
	skyBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	skyBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	skyBufferDesc.MiscFlags = 0;
	skyBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&skyBufferDesc, NULL, &m_skyBuffer);
	if(FAILED(result))
	{
		return result;
	}

	return result;

}

void SkyPlaneShader::Shutdown()
{
	BasicShader::Shutdown();
	// Release the sky constant buffer.
	if(m_skyBuffer)
	{
		m_skyBuffer->Release();
		m_skyBuffer = 0;
	}

	// Release the sampler states.
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void SkyPlaneShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, textureArray.size(), &textureArray[0]);
}

HRESULT SkyPlaneShader::SetSkyBuffer(ID3D11DeviceContext* deviceContext, float firstTranslationX, float firstTranslationZ,
								  float secondTranslationX, float secondTranslationZ, float brightness)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	SkyBufferType* dataPtr2;
	unsigned int bufferNumber;

	// Lock the sky constant buffer so it can be written to.
	result = deviceContext->Map(m_skyBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return result;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (SkyBufferType*)mappedResource.pData;

	// Copy the sky variables into the constant buffer.
	dataPtr2->firstTranslationX = firstTranslationX;
	dataPtr2->firstTranslationZ = firstTranslationZ;
	dataPtr2->secondTranslationX = secondTranslationX;
	dataPtr2->secondTranslationZ = secondTranslationZ;
	dataPtr2->brightness = brightness;
	dataPtr2->padding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_skyBuffer, 0);

	// Set the position of the sky constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the sky constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_skyBuffer);
	return result;
}

HRESULT SkyPlaneShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
										 D3DXMATRIX projectionMatrix)
{

	HRESULT result = S_OK;
	result = BasicShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	return result;
}

HRESULT SkyPlaneShader::Render(ID3D11DeviceContext* deviceContext,
							   int indexCount,
							   D3DXMATRIX worldMatrix, 
							   D3DXMATRIX viewMatrix, 
							   D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if(FAILED(result)) { return result; }

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Now render the prepared buffers with the shader.
	BasicShader::RenderShaderOrdinary(deviceContext, indexCount);

	return result;
}
