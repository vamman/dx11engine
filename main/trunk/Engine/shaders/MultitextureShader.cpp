#include "MultitextureShader.h"

MultitextureShader::MultitextureShader(void) : TextureShader()
{

}

MultitextureShader::~MultitextureShader(void)
{

}

void MultitextureShader::Shutdown()
{
	TextureShader::Shutdown();
}

HRESULT MultitextureShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	result = TextureShader::Initialize(device, hwnd, filename, VSname, PSname);
	if(FAILED(result))
	{
		return result;
	}
	return result;
}

HRESULT MultitextureShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
										int vertexCount,
										int instanceCount,
										D3DXMATRIX worldMatrix, 
										D3DXMATRIX viewMatrix,
										D3DXMATRIX projectionMatrix,
										vector<ID3D11ShaderResourceView*>& textureArray)
{
	HRESULT result = S_OK;

	// Set the shader parameters that it will use for rendering.

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, true);
	if(FAILED(result)) { return result; }

	// Now render the prepared buffers with the shader.
	TextureShader::SetTextureArray(deviceContext, textureArray);
	result = TextureShader::RenderInstanced(deviceContext,
											vertexCount,
											instanceCount,
											worldMatrix, 
											viewMatrix,
											projectionMatrix);
	if(FAILED(result)) { return result; }
	return result;
}

HRESULT MultitextureShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
								int indexCount,
								D3DXMATRIX worldMatrix, 
								D3DXMATRIX viewMatrix,
								D3DXMATRIX projectionMatrix,
								vector<ID3D11ShaderResourceView*>& textureArray)
{
	HRESULT result = S_OK;
	// Set the shader parameters that it will use for rendering.

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, false);
	if(FAILED(result)) { return result; }

	// Now render the prepared buffers with the shader.
	TextureShader::SetTextureArray(deviceContext, textureArray);
	result = TextureShader::RenderOrdinary(deviceContext,
											indexCount, 
											worldMatrix, 
											viewMatrix,
											projectionMatrix);
	if(FAILED(result)) { return result; }
	return result;
}

HRESULT MultitextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
	vector<ID3D11ShaderResourceView*>& textureArray, bool isInstanced)
{
	HRESULT result;
	result = BasicShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);
	if(FAILED(result)) { return result;	}

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, textureArray.size(), &textureArray[0]);
	return result;
}