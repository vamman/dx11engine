///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;
	mSentence = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


HRESULT TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, 
	D3DXMATRIX baseViewMatrix)
{
	HRESULT result = S_FALSE;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object.
	m_Font = new FontClass;
	if(!m_Font)
	{
		return result;
	}

	// Initialize the font object.
	result = m_Font->Initialize(device, "Engine/data/fonts/fontdata.txt", L"Engine/data/fonts/font.dds");
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return result;
	}

	// Create the font shader object.
	m_FontShader = new FontShader;
	if(!m_FontShader)
	{
		return result;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd, L"Engine/data/shaders/FontShader.fx",
		"FontVertexShader", "FontPixelShader");
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return result;
	}

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "	TextClass::Initialize time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "	TextClass::Initialize time: ");
	}

	return result;
}

bool TextClass::AddSentence(D3DClass* d3d, char *text, int posX, int posY, float colorR, float colorG, float colorB, int sentenceID)
{
	bool result;
	SentenceType* sentence = mSentence;
	vector<int>::iterator it;

	it = find(mSentenceIDs.begin(), mSentenceIDs.end(), sentenceID);
	if ( it ==  mSentenceIDs.end() )
	{
		result = InitializeSentence(&sentence, MAX_LENGTH, d3d->GetDevice());
		if(!result)
		{
			return false;
		}
		mSentenceIDs.push_back(sentenceID);
		mSentenceVector.push_back(sentence);
	}

	result = UpdateSentence(mSentenceVector[sentenceID], text, posX, posY, colorR, colorG, colorB, d3d->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&mSentence);

	// Release the font shader object.
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	return;
}

HRESULT TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	HRESULT result;

	result = RenderText(deviceContext, worldMatrix, orthoMatrix);
	if(FAILED(result)) { return result; }
	return result;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result;
	int i;

	// Create a new sentence object.
	*sentence = new SentenceType;
	if(!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for(i=0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	
	
	//result = BufferManager::GetInstance()->CreateVertexBuffer(device, sizeof(VertexType) * (*sentence)->vertexCount, vertices, &(*sentence)->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	result = BufferManager::GetInstance()->CreateIndexBuffer(device, sizeof(unsigned long) * (*sentence)->indexCount, indices, &(*sentence)->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	SentenceType* foundSentence;

	vector<SentenceType*>::iterator it;
  
	it = find(mSentenceVector.begin(), mSentenceVector.end(), sentence);

	foundSentence = (*it);

	if (!foundSentence)
	{
		return false;
	}

	// Store the color of the sentence.
	foundSentence->red = red; // sentence
	foundSentence->green = green; // sentence
	foundSentence->blue = blue; // sentence

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if(numLetters > foundSentence->maxLength) // sentence
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[foundSentence->vertexCount]; // sentence
	if(!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * foundSentence->vertexCount)); // sentence

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(foundSentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); // sentence
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * foundSentence->vertexCount)); // sentence

	// Unlock the vertex buffer.
	deviceContext->Unmap(foundSentence->vertexBuffer, 0); // sentence

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if(*sentence)
	{
		// Release the sentence vertex buffer.
		if((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}

HRESULT TextClass::RenderText(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	HRESULT result;

	for (int i = 0; i < mSentenceVector.size(); ++i)
	{
		result = RenderSentence(deviceContext, mSentenceVector[i], worldMatrix, orthoMatrix);
		if (FAILED(result)) { return false;	}
	}
	return result;
}

HRESULT TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix, 
	D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	HRESULT result;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	vector<ID3D11ShaderResourceView*> textureArray;
	textureArray.push_back(m_Font->GetTexture());
	m_FontShader->SetTextureArray(deviceContext, textureArray);
	m_FontShader->SetPixelBufferColor(deviceContext, pixelColor);
	result = m_FontShader->RenderOrdinary(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix);
	if(FAILED(result)) { false; }

	return result;
}


