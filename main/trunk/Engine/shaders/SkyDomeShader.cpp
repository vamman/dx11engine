// TODO: Fit sky dome shader into existing architecture

#include "SkyDomeShader.h"

SkyDomeShader::SkyDomeShader(void)
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_gradientBuffer = 0;
}

SkyDomeShader::~SkyDomeShader(void)
{
}

HRESULT SkyDomeShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	vector<char *> layouts;
	layouts.push_back("POSITION");

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, filename, VSname, PSname, layouts);
	if(!result)
	{
		return false;
	}

	return true;
}

HRESULT SkyDomeShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	D3D11_BUFFER_DESC gradientBufferDesc;
	HRESULT result = S_OK;
	
	BasicShader::InitializeShader(device, hwnd, filename, VSname, PSname, layouts);

	// Setup the description of the gradient constant buffer that is in the pixel shader.
	gradientBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) + sizeof(D3DXVECTOR4) + sizeof(D3DXVECTOR4); // sizeof(GradientBufferType);
	gradientBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientBufferDesc.MiscFlags = 0;
	gradientBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&gradientBufferDesc, NULL, &m_gradientBuffer);
	if(FAILED(result))
	{
		return result;
	}

	return result;
}

void SkyDomeShader::Shutdown()
{
	// Release the gradient constant buffer.
	if(m_gradientBuffer)
	{
		m_gradientBuffer->Release();
		m_gradientBuffer = 0;
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

bool SkyDomeShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
						   D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor, float pixelShaderMode)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor, pixelShaderMode);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool SkyDomeShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
										D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor, float pixelShaderMode)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	GradientBufferType* dataPtr2;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	// D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	// D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	// D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	BasicShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);

	// Lock the gradient constant buffer so it can be written to.
	result = deviceContext->Map(m_gradientBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (GradientBufferType*)mappedResource.pData;

	// Copy the gradient color variables into the constant buffer.
	dataPtr2->apexColor = apexColor;
	dataPtr2->centerColor = centerColor;
	dataPtr2->pixelShaderType = D3DXVECTOR4(pixelShaderMode, 0.0f, 0.0f, 0.0f);// (float)0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_gradientBuffer, 0);

	// Set the position of the gradient constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the gradient constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_gradientBuffer);

	return true;
}

void SkyDomeShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	BasicShader::RenderShaderOrdinary(deviceContext, indexCount);
	return;
}
