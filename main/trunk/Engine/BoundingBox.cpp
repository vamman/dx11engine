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
	BufferManager::GetInstance()->CreateVertexBuffer(device, sizeof(VertexTypeLine) * NUM_VERTEXES, vertices, &mVertexBuffer);

	// Set up the description of the index buffer.
	BufferManager::GetInstance()->CreateVertexBuffer(device, sizeof(unsigned long) * 24, indices, &mIndexBuffer);

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
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->DrawIndexed(24, 0, 0);
}
