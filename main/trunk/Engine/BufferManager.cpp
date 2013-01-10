#include "BufferManager.h"

BufferManager* BufferManager::m_pInstance = NULL;

BufferManager::BufferManager(void)
{
	Log::GetInstance()->WriteTextMessageToFile("BufferManager created.");
}

BufferManager::~BufferManager(void)
{
}

BufferManager* BufferManager::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new BufferManager;
	}
	return m_pInstance;
}

HRESULT BufferManager::CreateVertexBuffer(ID3D11Device* device, int bufferSize, void * vertices,
										  ID3D11Buffer** vertexBuffer, D3D11_USAGE useage, UINT bindFlag, UINT cpuAccessFlag)
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	HRESULT result = S_FALSE;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = (useage == D3D11_USAGE_DEFAULT ? D3D11_USAGE_DEFAULT : useage);
	vertexBufferDesc.ByteWidth = bufferSize;
	vertexBufferDesc.BindFlags = (bindFlag == D3D11_BIND_VERTEX_BUFFER ? D3D11_BIND_VERTEX_BUFFER : bindFlag);
	vertexBufferDesc.CPUAccessFlags = (cpuAccessFlag == 0 ? 0 : cpuAccessFlag);
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer);
	if(FAILED(result))
	{
		return result;
	}

	//char* numStr;
	char numStr[10];
	sprintf(numStr,"%d",mVertexBufferList.size());
	string numString = string(numStr);
	string bufferName = string("vertexBuffer_") + numString;
	BufferInfo bufInfo = BufferInfo(bufferName, *vertexBuffer);
	mVertexBufferList.push_back(bufInfo);

	return result;
}

HRESULT BufferManager::CreateIndexBuffer(ID3D11Device* device, int bufferSize, void * indices, ID3D11Buffer** indexBuffer)
{
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result = S_FALSE;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = bufferSize;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//char* numStr;
	char numStr[10];
	sprintf(numStr,"%d", mIndexBufferList.size());
	string numString = string(numStr);
	string bufferName = string("indexBuffer_") + numString;
	BufferInfo bufInfo = BufferInfo(bufferName, *indexBuffer);
	mIndexBufferList.push_back(bufInfo);

	return result;
}

HRESULT BufferManager::CreateInstanceBuffer(ID3D11Device* device, int bufferSize, void * instances, ID3D11Buffer** instanceBuffer)
{
	D3D11_SUBRESOURCE_DATA instanceData;
	D3D11_BUFFER_DESC instanceBufferDesc;
	HRESULT result = S_FALSE;

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = bufferSize;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	//char* numStr;
	char numStr[10];
	sprintf(numStr,"%d",mInstanceBufferList.size());
	string numString = string(numStr);
	string bufferName = string("instanceBuffer_") + numString;
	BufferInfo bufInfo = BufferInfo(bufferName, *instanceBuffer);
	mInstanceBufferList.push_back(bufInfo);

	return result;
}

void BufferManager::Shutdown()
{
	vector<BufferInfo>::iterator iter;

	for (iter = mVertexBufferList.begin(); iter != mVertexBufferList.end(); iter++)
	{
		iter->buffer->Release();
	}

	for (iter = mIndexBufferList.begin(); iter != mIndexBufferList.end(); iter++)
	{
		iter->buffer->Release();
	}

	for (iter = mInstanceBufferList.begin(); iter != mInstanceBufferList.end(); iter++)
	{
		iter->buffer->Release();
	}
}

