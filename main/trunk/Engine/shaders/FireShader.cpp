#include "FireShader.h"
#include "../MacroHelper.h"

FireShader::FireShader(void) 
	: TextureShader()
	, m_noiseBuffer(0)
	, m_sampleState2(0)
	, m_distortionBuffer(0)
{
}


FireShader::~FireShader(void)
{
}

HRESULT FireShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
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

HRESULT FireShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	unsigned int numElements;
	D3D11_BUFFER_DESC noiseBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc2;
	D3D11_BUFFER_DESC distortionBufferDesc;

	result = TextureShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}

	// Setup the description of the dynamic noise constant buffer that is in the vertex shader.
	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	// Create the noise buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&noiseBufferDesc, NULL, &m_noiseBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Create a second texture sampler state description for a Clamp sampler.
	samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc2.MipLODBias = 0.0f;
	samplerDesc2.MaxAnisotropy = 1;
	samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc2.BorderColor[0] = 0;
	samplerDesc2.BorderColor[1] = 0;
	samplerDesc2.BorderColor[2] = 0;
	samplerDesc2.BorderColor[3] = 0;
	samplerDesc2.MinLOD = 0;
	samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc2, &m_sampleState2);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic distortion constant buffer that is in the pixel shader.
	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	// Create the distortion buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&distortionBufferDesc, NULL, &m_distortionBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return result;
}

void FireShader::Shutdown()
{
	TextureShader::Shutdown();
	// Release the sampler state.
	if(m_sampleState2)
	{
		m_sampleState2->Release();
		m_sampleState2 = 0;
	}

	// Release the distortion constant buffer.
	if(m_distortionBuffer)
	{
		m_distortionBuffer->Release();
		m_distortionBuffer = 0;
	}

	// Release the noise constant buffer.
	if(m_noiseBuffer)
	{
		m_noiseBuffer->Release();
		m_noiseBuffer = 0;
	}
}

HRESULT FireShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix, float frameTime, D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, D3DXVECTOR2 distortion1, 
	D3DXVECTOR2 distortion2, D3DXVECTOR2 distortion3, float distortionScale, float distortionBias, bool isInstanced)
{
	HRESULT result = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	NoiseBufferType* dataPtr2;
	DistortionBufferType* dataPtr3;
	unsigned int bufferNumber;

	result = TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);

	// Lock the noise constant buffer so it can be written to.
	result = deviceContext->Map(m_noiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the noise constant buffer.
	dataPtr2 = (NoiseBufferType*)mappedResource.pData;

	// Copy the data into the noise constant buffer.
	dataPtr2->frameTime = frameTime;
	dataPtr2->scrollSpeeds = scrollSpeeds;
	dataPtr2->scales = scales;
	dataPtr2->padding = 0.0f;

	// Unlock the noise constant buffer.
	deviceContext->Unmap(m_noiseBuffer, 0);

	// Set the position of the noise constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the noise constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_noiseBuffer);

	// Lock the distortion constant buffer so it can be written to.
	result = deviceContext->Map(m_distortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the distortion constant buffer.
	dataPtr3 = (DistortionBufferType*)mappedResource.pData;

	// Copy the data into the distortion constant buffer.
	dataPtr3->distortion1 = distortion1;
	dataPtr3->distortion2 = distortion2;
	dataPtr3->distortion3 = distortion3;
	dataPtr3->distortionScale = distortionScale;
	dataPtr3->distortionBias = distortionBias;

	// Unlock the distortion constant buffer.
	deviceContext->Unmap(m_distortionBuffer, 0);

	// Set the position of the distortion constant buffer in the pixel shader.
	bufferNumber = 0;

	// Now set the distortion constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_distortionBuffer);

	return result;
}

void FireShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	// Set shader texture resource in the pixel shader.
	TextureShader::SetTextureArray(deviceContext, textureArray);
}

HRESULT FireShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
						   int vertexCount,
						   int instanceCount,
						   D3DXMATRIX worldMatrix, 
						   D3DXMATRIX viewMatrix, 
						   D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;
	ACTION_THROUGH_ASSERT(FALSE, L"FireShader::RenderInstanced is not supported !");
	/*
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, true);
	if(FAILED(result)) { return result; }
	
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState2);

	// Now render the prepared buffers with the shader.
	BasicShader::RenderShaderInstanced(deviceContext,
									   vertexCount,
									   instanceCount);
	*/
	return result;
}

// TODO: Figure out situation with sampler states here and in TextureShader. 
// Because of their conflict the rendering of fire works incorrect.
HRESULT FireShader::RenderOrdinary( ID3D11DeviceContext* deviceContext,
									int indexCount,
									D3DXMATRIX worldMatrix, 
									D3DXMATRIX viewMatrix, 
									D3DXMATRIX projectionMatrix, float frameTime,
									D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, 
									D3DXVECTOR2 distortion1, D3DXVECTOR2 distortion2,
									D3DXVECTOR2 distortion3, float distortionScale, float distortionBias )
{
	HRESULT result = S_OK;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, frameTime, scrollSpeeds, scales,
								 distortion1, distortion2, distortion3, distortionScale, distortionBias, false);
	if(FAILED(result)) { return result; }
	
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(1, 1, &m_sampleState2);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return result;
}