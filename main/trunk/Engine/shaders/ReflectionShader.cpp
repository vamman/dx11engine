#include "ReflectionShader.h"


ReflectionShader::ReflectionShader(void)
{
}


ReflectionShader::~ReflectionShader(void)
{
}

HRESULT ReflectionShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
								  LPCSTR VSname, LPCSTR PSname)
{
	D3D11_BUFFER_DESC reflectionBufferDesc;

	HRESULT result;

	// TODO
	vector<char *> layouts;
	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	// layouts.push_back("TEXCOORD_INST");

	result = TextureShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the reflection dynamic constant buffer that is in the vertex shader.
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&reflectionBufferDesc, NULL, &m_reflectionBuffer);
	if(FAILED(result))
	{
		return result;
	}
	return result;
}


void ReflectionShader::Shutdown()
{
	TextureShader::Shutdown();

	// Release the reflection constant buffer.
	if(m_reflectionBuffer)
	{
		m_reflectionBuffer->Release();
		m_reflectionBuffer = 0;
	}

	return;
}

bool ReflectionShader::RenderInstanced(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount, D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,  vector<ID3D11ShaderResourceView*> textureVector,
	ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureVector, reflectionTexture, 
		reflectionMatrix, true);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	TextureShader::SetTextureArray(deviceContext, textureVector);
	TextureShader::RenderInstanced(deviceContext, vertexCount, instanceCount,
								   worldMatrix, viewMatrix, projectionMatrix);
	return true;
}

bool ReflectionShader::RenderOrdinary(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,  vector<ID3D11ShaderResourceView*> textureVector,
	ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureVector, reflectionTexture, 
		reflectionMatrix, false);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	TextureShader::SetTextureArray(deviceContext, textureVector);
	TextureShader::RenderOrdinary(deviceContext, indexCount,
		worldMatrix, viewMatrix, projectionMatrix);
	return true;
}

void ReflectionShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	// Set shader texture resource in the pixel shader.
	TextureShader::SetTextureArray(deviceContext, textureArray);
}

bool ReflectionShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix, vector<ID3D11ShaderResourceView*> textureVector,
	ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix, bool isInstanced)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	ReflectionBufferType* dataPtr2;

	TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);

	// Lock the reflection constant buffer so it can be written to.
	result = deviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the matrix constant buffer.
	dataPtr2 = (ReflectionBufferType*)mappedResource.pData;

	// Copy the matrix into the reflection constant buffer.
	dataPtr2->reflectionMatrix = reflectionMatrix;

	// Unlock the reflection constant buffer.
	deviceContext->Unmap(m_reflectionBuffer, 0);

	// Set the position of the reflection constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the reflection constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);

	// Set the reflection texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);

	return true;
}
