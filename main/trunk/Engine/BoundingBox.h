#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include "BufferManager.h"

class BoundingBox
{
	struct VertexTypeLine
	{
		D3DXVECTOR3 position;
	};

	public:
		void CreateBBBuffers(D3DXVECTOR3 centerPos, float sizeX, float sizeY, float sizeZ, ID3D11Device* device);
		void RenderBBBuffers(ID3D11DeviceContext* deviceContext);
		BoundingBox(void);
		~BoundingBox(void);
	private:
		ID3D11Buffer *mVertexBuffer, *mIndexBuffer;
};

