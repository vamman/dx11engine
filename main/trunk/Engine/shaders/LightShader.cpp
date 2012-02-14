#include "LightShader.h"


LightShader::LightShader(void) : TextureShader()
{
	m_cameraBuffer = 0;
	m_lightBuffer = 0;

	m_lightColorBuffer = 0;
	m_lightPositionBuffer = 0;
}


LightShader::~LightShader(void)
{
}

void LightShader::Shutdown()
{
	TextureShader::Shutdown();
	// Release the light constant buffer.
	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
	// Release the camera constant buffer.
	if(m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}
	// Release the light constant buffers.
	if(m_lightColorBuffer)
	{
		m_lightColorBuffer->Release();
		m_lightColorBuffer = 0;
	}
	if(m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = 0;
	}
}

HRESULT LightShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	vector<char *> layouts;

	layouts.push_back("POSITION");
	layouts.push_back("TEXCOORD");
	layouts.push_back("NORMAL");

	result = InitializeShader(lightSource, device, hwnd, filename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}

	return result;
}

HRESULT LightShader::InitializeShader(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
	LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	result = InitializeDirectionalLightShader(lightSource, device, hwnd, FXfilename, VSname, PSname, layouts);
	return result;
}

HRESULT LightShader::InitializeDirectionalLightShader(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	D3D11_BUFFER_DESC cameraBufferDesc, lightBufferDesc;

	result = TextureShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	if (lightSource->GetLightType() == LightClass::DIRECTIONAL_SPECULAR_LIGHT)
	{
		cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
		cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cameraBufferDesc.MiscFlags = 0;
		cameraBufferDesc.StructureByteStride = 0;

		// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
		result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
		if(FAILED(result))
		{
			return result;
		}
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	
	int sizeOfBuffer = (lightSource->GetLightType() == LightClass::DIRECTIONAL_SPECULAR_LIGHT ? sizeof(SpecLightBufferType) : sizeof(AmbLightBufferType));
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeOfBuffer;
	
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return result;
	}
	
	return result;
}

HRESULT LightShader::InitializePointLightShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	D3D11_BUFFER_DESC lightColorBufferDesc, lightPositionBufferDesc;

	result = TextureShader::InitializeShader(device, hwnd, FXfilename, VSname, PSname, layouts);
	if (FAILED(result))
	{
		return result;
	}

	// Setup the description of the dynamic constant buffer that is in the pixel shader.
	lightColorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightColorBufferDesc.ByteWidth = sizeof(LightColorBufferType);
	lightColorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColorBufferDesc.MiscFlags = 0;
	lightColorBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&lightColorBufferDesc, NULL, &m_lightColorBuffer);
	if(FAILED(result))
	{
		return result;
	}

	// Setup the description of the dynamic constant buffer that is in the vertex shader.
	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
	if(FAILED(result))
	{
		return result;
	}

	return result;
}

void LightShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	// Set shader texture resource in the pixel shader.
	TextureShader::SetTextureArray(deviceContext, textureArray);
}

HRESULT LightShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	HRESULT result = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	CameraBufferType* CameraDataPtr;

	if (lightType == LightClass::DIRECTIONAL_SPECULAR_LIGHT)
	{
		// Lock the camera constant buffer so it can be written to.
		result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return result;
		}

		// Get a pointer to the data in the constant buffer.
		CameraDataPtr = (CameraBufferType*)mappedResource.pData;

		// Copy the camera position into the constant buffer.
		CameraDataPtr->cameraPosition = cameraPosition;
		CameraDataPtr->padding = 0.0f;

		// Unlock the camera constant buffer.
		deviceContext->Unmap(m_cameraBuffer, 0);

		// Set the position of the camera constant buffer in the vertex shader.
		bufferNumber = 1;

		// Now set the camera constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	}
	return result;
}

bool LightShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	HRESULT result = S_FALSE;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	SpecLightBufferType* SpecLightDataPtr;
	AmbLightBufferType* AmbLightDataPtr;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the lighting variables into the constant buffer.
	switch (lightSource->GetLightType())
	{
		case LightClass::DIRECTIONAL_SPECULAR_LIGHT:
		{
			SpecLightDataPtr = (SpecLightBufferType*)mappedResource.pData;

			SpecLightDataPtr->ambientColor = lightSource->GetAmbientColor();
			SpecLightDataPtr->diffuseColor = lightSource->GetDiffuseColor();
			SpecLightDataPtr->lightDirection = lightSource->GetDirection();
			SpecLightDataPtr->specularColor = lightSource->GetSpecularColor();
			SpecLightDataPtr->specularPower = lightSource->GetSpecularPower();
			break;
		}
		case LightClass::DIRECTIONAL_AMBIENT_LIGHT:
		{
			// Get a pointer to the data in the constant buffer.
			AmbLightDataPtr = (AmbLightBufferType*)mappedResource.pData;

			AmbLightDataPtr->ambientColor = lightSource->GetAmbientColor();
			AmbLightDataPtr->diffuseColor = lightSource->GetDiffuseColor();
			AmbLightDataPtr->lightDirection = lightSource->GetDirection();
			AmbLightDataPtr->padding = 0.0f;
			break;
		}
	}

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 2, &m_lightBuffer);

	return true;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix,
	bool isInstanced)
{
	bool result;
//	TextureShader::SetTextureArray(deviceContext, textureArray);
	result = TextureShader::SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, isInstanced);
//	D3DXVECTOR4 pointDiffuseColors[5]; // TODO: Add mulilight functionality
//	D3DXVECTOR4 pointLightPositions[5]; // TODO: Add mulilight functionality
	return result;
}

bool LightShader::SetDirLightShaderParams(ID3D11DeviceContext* deviceContext,
	D3DXVECTOR3 cameraPosition,
	LightClass* lightSource)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	SpecLightBufferType* SpecLightDataPtr;
	AmbLightBufferType* AmbLightDataPtr;
	CameraBufferType* CameraDataPtr;

	if (lightSource->GetLightType() == LightClass::DIRECTIONAL_SPECULAR_LIGHT)
	{
		// Lock the camera constant buffer so it can be written to.
		result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		CameraDataPtr = (CameraBufferType*)mappedResource.pData;

		// Copy the camera position into the constant buffer.
		CameraDataPtr->cameraPosition = cameraPosition;
		CameraDataPtr->padding = 0.0f;

		// Unlock the camera constant buffer.
		deviceContext->Unmap(m_cameraBuffer, 0);

		// Set the position of the camera constant buffer in the vertex shader.
		bufferNumber = 1;

		// Now set the camera constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	}

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the lighting variables into the constant buffer.
	if (lightSource->GetLightType() == LightClass::DIRECTIONAL_SPECULAR_LIGHT)
	{
		// Get a pointer to the data in the constant buffer.
		SpecLightDataPtr = (SpecLightBufferType*)mappedResource.pData;

		SpecLightDataPtr->ambientColor = lightSource->GetAmbientColor();
		SpecLightDataPtr->diffuseColor = lightSource->GetDiffuseColor();
		SpecLightDataPtr->lightDirection = lightSource->GetDirection();
		SpecLightDataPtr->specularColor = lightSource->GetSpecularColor();
		SpecLightDataPtr->specularPower = lightSource->GetSpecularPower();
	}

	if (lightSource->GetLightType() == LightClass::DIRECTIONAL_AMBIENT_LIGHT)
	{
		// Get a pointer to the data in the constant buffer.
		AmbLightDataPtr = (AmbLightBufferType*)mappedResource.pData;

		AmbLightDataPtr->ambientColor = lightSource->GetAmbientColor();
		AmbLightDataPtr->diffuseColor = lightSource->GetDiffuseColor();
		AmbLightDataPtr->lightDirection = lightSource->GetDirection();
		AmbLightDataPtr->padding = 0.0f;
	}

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

bool LightShader::SetPointLightShaderParameters(ID3D11DeviceContext* deviceContext,
												D3DXVECTOR4 pointDiffuseColors[],
												D3DXVECTOR4 pointLightPositions[])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightPositionBufferType* dataPtr2;
	LightColorBufferType* dataPtr3;
	unsigned int bufferNumber;

	// Lock the light position constant buffer so it can be written to.
	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightPositionBufferType*)mappedResource.pData;

	// Copy the light position variables into the constant buffer.
	dataPtr2->lightPosition[0] = pointLightPositions[0];
	dataPtr2->lightPosition[1] = pointLightPositions[1];
	dataPtr2->lightPosition[2] = pointLightPositions[2];
	dataPtr2->lightPosition[3] = pointLightPositions[3];

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightPositionBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightPositionBuffer);

	// Lock the light color constant buffer so it can be written to.
	result = deviceContext->Map(m_lightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightColorBufferType*)mappedResource.pData;

	// Copy the light color variables into the constant buffer.
	dataPtr3->diffuseColor[0] = pointDiffuseColors[0];
	dataPtr3->diffuseColor[1] = pointDiffuseColors[1];
	dataPtr3->diffuseColor[2] = pointDiffuseColors[2];
	dataPtr3->diffuseColor[3] = pointDiffuseColors[3];

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightColorBuffer, 0);

	// Set the position of the constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightColorBuffer);

	return true;
}

bool LightShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
						 int vertexCount,
						 int instanceCount,
						 D3DXMATRIX worldMatrix,
						 D3DXMATRIX viewMatrix, 
						 D3DXMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext,
		worldMatrix,
		viewMatrix, 
		projectionMatrix,
		true);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	result = TextureShader::RenderInstanced(deviceContext,
											vertexCount,
											instanceCount,
											worldMatrix,
											viewMatrix, 
											projectionMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}

bool LightShader::RenderOrdinary(ID3D11DeviceContext* deviceContext,
						 int indexCount,
						 D3DXMATRIX worldMatrix,
						 D3DXMATRIX viewMatrix, 
						 D3DXMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext,
		worldMatrix,
		viewMatrix, 
		projectionMatrix,
		false);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	result = TextureShader::RenderOrdinary(deviceContext,
											indexCount,
											worldMatrix,
											viewMatrix, 
											projectionMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}
