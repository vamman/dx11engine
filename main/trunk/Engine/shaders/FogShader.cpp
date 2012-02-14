#include "FogShader.h"


FogShader::FogShader(void) : TextureShader()
{
	m_fogBuffer = 0;
}


FogShader::~FogShader(void)
{

}

void FogShader::Shutdown()
{
	TextureShader::Shutdown();

	// Release the fog constant buffer.
	if(m_fogBuffer)
	{
		m_fogBuffer->Release();
		m_fogBuffer = 0;
	}
}

HRESULT FogShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	vector<char *> layouts;
	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("TEXCOORD_INST");

	result = InitializeShader(device, hwnd, FXfilename, VSname, PSname,  layouts);

	if (FAILED(result))
	{
		return result;
	}
	return result;
}

HRESULT FogShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
						   LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	D3D11_BUFFER_DESC fogBufferDesc;

	result = TextureShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}

	// Setup the description of the dynamic fog constant buffer that is in the vertex shader.
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(FogBufferType);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;

	// Create the fog buffer pointer so we can access the vertex shader fog constant buffer from within this class.
	result = device->CreateBuffer(&fogBufferDesc, NULL, &m_fogBuffer);
	if(FAILED(result))
	{
		return result;
	}

	return result;
}

bool FogShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
									D3DXMATRIX worldMatrix, 
									D3DXMATRIX viewMatrix,
									D3DXMATRIX projectionMatrix,
									vector<ID3D11ShaderResourceView*> textureVector, 
									float fogStart, 
									float fogEnd,
									bool isInstanced)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	FogBufferType* dataPtr2;
	TextureShader::SetTextureArray(deviceContext, textureVector);
	TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);

	// Lock the fog constant buffer so it can be written to.
	result = deviceContext->Map(m_fogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (FogBufferType*)mappedResource.pData;

	// Copy the fog information into the fog constant buffer.
	dataPtr2->fogStart = fogStart;
	dataPtr2->fogEnd = fogEnd;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_fogBuffer, 0);

	// Set the position of the fog constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the fog buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_fogBuffer);

	return true;
}

bool FogShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
					   int indexCount, 
					   D3DXMATRIX worldMatrix, 
					   D3DXMATRIX viewMatrix,
					   D3DXMATRIX projectionMatrix, 
					   vector<ID3D11ShaderResourceView*> textureVector, 
					   float fogStart,
					   float fogEnd)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureVector, fogStart, fogEnd, false);
	if(!result)
	{
		return false;
	}

	result = TextureShader::RenderOrdinary(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}

bool FogShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
	int vertexCount,
	int instanceCount,
	D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, 
	vector<ID3D11ShaderResourceView*> textureVector, 
	float fogStart,
	float fogEnd)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureVector, fogStart, fogEnd, true);
	if(!result)
	{
		return false;
	}
	TextureShader::SetTextureArray(deviceContext, textureVector);
	result = TextureShader::RenderInstanced(deviceContext, vertexCount, instanceCount, worldMatrix, viewMatrix, projectionMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}
