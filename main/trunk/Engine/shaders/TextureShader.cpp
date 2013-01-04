#include "TextureShader.h"


TextureShader::TextureShader(void) : BasicShader()
{
		m_sampleState = 0;
}


TextureShader::~TextureShader(void)
{
}

HRESULT TextureShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;

	vector<char *> layouts;
	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("TEXCOORD_INST");

	result = InitializeShader(device, hwnd, filename, VSname, PSname, layouts);

	if (FAILED(result))
	{
		return result;
	}
	return result;
}

HRESULT TextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	D3D11_SAMPLER_DESC samplerDesc;

	result = BasicShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
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
	return result;
}

void TextureShader::Shutdown()
{
	BasicShader::Shutdown();
	// Release the sampler state.
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}
}

HRESULT TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix, bool isInstanced)
{
	HRESULT result = S_OK;
	result = BasicShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);
	return result;
}

void TextureShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	// Set shader texture resource in the pixel shader.
	vector<ID3D11ShaderResourceView*>::iterator texturelIt;
	int count = 0;
	for (texturelIt = textureArray.begin(); texturelIt != textureArray.end(); ++texturelIt)
	{
		ID3D11ShaderResourceView* texture = (*texturelIt);
		deviceContext->PSSetShaderResources(count, 1, &texture);
		count++;
	}
}

HRESULT TextureShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
						   int vertexCount,
						   int instanceCount,
						   D3DXMATRIX worldMatrix, 
						   D3DXMATRIX viewMatrix, 
						   D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, true);
	if(FAILED(result)) { return result; }
	
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Now render the prepared buffers with the shader.
	BasicShader::RenderShaderInstanced(deviceContext,
									   vertexCount,
									   instanceCount);
	return result;
}

HRESULT TextureShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
							int indexCount,
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix)
{
	HRESULT result = S_OK;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(FAILED(result)) { return result; }

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Now render the prepared buffers with the shader.
	BasicShader::RenderShaderOrdinary(deviceContext,
							  indexCount);

	return result;
}