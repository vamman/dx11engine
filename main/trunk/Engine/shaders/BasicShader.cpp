#include "BasicShader.h"    

BasicShader::BasicShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

BasicShader::BasicShader(const BasicShader& other)
{
}

BasicShader::~BasicShader()
{
}

HRESULT BasicShader::Initialize(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result;
	vector<char *> layouts;
	layouts.push_back("POSITION");
	layouts.push_back("COLOR");
	layouts.push_back("TEXCOORD_INST");

	result = InitializeShader(device, hwnd, FXfilename, VSname, PSname,  layouts);

	if (!result)
	{
		return result;
	}
	return result;
}

HRESULT BasicShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
	LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts)
{
	HRESULT result;
	ID3D10Blob* errorMessage = NULL;
	ID3D10Blob* vertexShaderBuffer = NULL;
	ID3D10Blob* pixelShaderBuffer = NULL;
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(FXfilename, NULL, NULL, VSname, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, FXfilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, FXfilename, L"Missing Shader File", MB_OK);
		}

		return result;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(FXfilename, NULL, NULL, PSname, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, FXfilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, FXfilename, L"Missing Shader File", MB_OK);
		}

		return result;
	}

	if (FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, FXfilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, FXfilename, L"Missing Shader File", MB_OK);
		}
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		return result;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
	{
		return result;
	}

	vector<D3D11_INPUT_ELEMENT_DESC> polygonLayoutVector;

	polygonLayoutVector = CreateInputLayout(layouts);

	D3D11_INPUT_ELEMENT_DESC* polygonLayoutArray = &polygonLayoutVector[0];

	// Get a count of the elements in the layout.
	numElements = polygonLayoutVector.size();

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayoutArray, numElements, vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return result;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType) + 12;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return result;
	}

	return result;
}

vector<D3D11_INPUT_ELEMENT_DESC> BasicShader::CreateInputLayout(vector<char *>& layouts)
{
	vector<D3D11_INPUT_ELEMENT_DESC> polygonLayout;

	vector<char *>::iterator it;

	for (it = layouts.begin(); it != layouts.end(); ++it)
	{
		D3D11_INPUT_ELEMENT_DESC currentLayout;

		if ( *it == LAYOUT_POSITION)
		{
			currentLayout.SemanticName = "POSITION";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = 0;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}
		else if ( *it == LAYOUT_COLOR)
		{
			currentLayout.SemanticName = "COLOR";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}
		else if ( *it == LAYOUT_TEXCOORD)
		{
			currentLayout.SemanticName = "TEXCOORD";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}
		else if ( *it == LAYOUT_TEXCOORD_NORMAL_DETAIL)
		{
			currentLayout.SemanticName = "TEXCOORD";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}
		else if ( *it == LAYOUT_TEXCOORD_INST)
		{
			currentLayout.SemanticName = "TEXCOORD";
			currentLayout.SemanticIndex = 1;
			currentLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			currentLayout.InputSlot = 1;
			currentLayout.AlignedByteOffset = 0;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			currentLayout.InstanceDataStepRate = 1;
		}
		else if ( *it == LAYOUT_NORMAL)
		{
			currentLayout.SemanticName = "NORMAL";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}
		else if ( *it == LAYOUT_TANGENT)
		{
			currentLayout.SemanticName = "TANGENT";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}
		else if ( *it == LAYOUT_BINORMAL)
		{
			currentLayout.SemanticName = "BINORMAL";
			currentLayout.SemanticIndex = 0;
			currentLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			currentLayout.InputSlot = 0;
			currentLayout.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			currentLayout.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			currentLayout.InstanceDataStepRate = 0;
		}

		polygonLayout.push_back(currentLayout);
	}
	return polygonLayout;
}

void BasicShader::Shutdown()
{
	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
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

HRESULT BasicShader::RenderInstanced(ID3D11DeviceContext* deviceContext,
									 int vertexCount,
									 int instanceCount,
									 D3DXMATRIX worldMatrix, 
									 D3DXMATRIX viewMatrix,
									 D3DXMATRIX projectionMatrix) const
{
	HRESULT result = S_OK;
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, true);
	if(FAILED(result)) { return result; }
	// Now render the prepared buffers with the shader.
	RenderShaderInstanced(deviceContext,
						  vertexCount,
						  instanceCount);
	return result;
}

HRESULT BasicShader::RenderOrdinary(ID3D11DeviceContext* deviceContext, 
									 int indexCount,
									 D3DXMATRIX worldMatrix, 
									 D3DXMATRIX viewMatrix,
									 D3DXMATRIX projectionMatrix) const
{
	HRESULT result = S_OK;
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(FAILED(result)) { return result; }
	// Now render the prepared buffers with the shader.
	RenderShaderOrdinary(deviceContext, indexCount);
	return result;
}

void BasicShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


void BasicShader::SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray)
{
	
}
HRESULT BasicShader::SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType)
{
	return S_OK;
}
bool BasicShader::SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource)
{
	return true;
}


HRESULT BasicShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool isInstanced) const
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return result;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->isInstanced = isInstanced;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return result;
}

void BasicShader::RenderShaderInstanced(ID3D11DeviceContext* deviceContext,
							   // int indexCount,
							   int vertexCount,
							   int instanceCount) const
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	//deviceContext->DrawIndexed(indexCount, 0, 0);
	deviceContext->DrawInstanced(vertexCount, instanceCount, 0, 0);

	return;
}

void BasicShader::RenderShaderOrdinary(ID3D11DeviceContext* deviceContext, int indexCount) const
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

