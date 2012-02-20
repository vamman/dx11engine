#include "BoundingBox.h"


BoundingBox::BoundingBox(void)
{
	mVertexBuffer = 0;
	mIndexBuffer = 0;
}


BoundingBox::~BoundingBox(void)
{
}

void BoundingBox::CreateBBBuffers(D3DXVECTOR3 centerPos, float sizeX, float sizeY, float sizeZ, ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	VertexTypeLine* vertices;
	const int NUM_VERTEXES = 8;
	vertices = new VertexTypeLine[NUM_VERTEXES];

	// Front face
	vertices[0].position = D3DXVECTOR3(centerPos.x - sizeX / 2, sizeY, centerPos.z - sizeZ / 2);
	vertices[1].position = D3DXVECTOR3(centerPos.x + sizeX / 2, sizeY, centerPos.z - sizeZ / 2);
	vertices[2].position = D3DXVECTOR3(centerPos.x - sizeX / 2, 0.0f,  centerPos.z - sizeZ / 2);
	vertices[3].position = D3DXVECTOR3(centerPos.x + sizeX / 2, 0.0f,  centerPos.z - sizeZ / 2);
	// Back face
	vertices[4].position = D3DXVECTOR3(centerPos.x - sizeX / 2, sizeY, centerPos.z + sizeZ / 2);
	vertices[5].position = D3DXVECTOR3(centerPos.x + sizeX / 2, sizeY, centerPos.z + sizeZ / 2);
	vertices[6].position = D3DXVECTOR3(centerPos.x - sizeX / 2, 0.0f,  centerPos.z + sizeZ / 2);
	vertices[7].position = D3DXVECTOR3(centerPos.x + sizeX / 2, 0.0f,  centerPos.z + sizeZ / 2);

	// Create the index array.
	unsigned long indices[] = {
		// front
		0, 1,
		0, 2,
		1, 3,
		2, 3,
		// left
		0, 4,
		2, 6,
		4, 6,
		// far
		4, 5,
		5, 7,
		6, 7,
		// right
		5, 1,
		7, 3
	};

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeLine) * NUM_VERTEXES;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = sizeof(VertexTypeLine) * NUM_VERTEXES;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 24;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);

	delete [] vertices;
	vertices = 0;
}

void BoundingBox::RenderBBBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride, offset;

	// Create buffers for line
	stride = sizeof(VertexTypeLine); 
	offset = 0;

	// Set buffers for line
	/*
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->DrawIndexed(24, 0, 0);
	*/
}
