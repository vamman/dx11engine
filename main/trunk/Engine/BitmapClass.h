////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "BufferManager.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: BitmapClass
////////////////////////////////////////////////////////////////////////////////
class BitmapClass
{
	private:
		struct VertexType
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};

	public:
		BitmapClass();
		BitmapClass(const BitmapClass&);
		~BitmapClass();

		bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
						char* textureFilename, int width = 0, int heigh = 0);
		void Shutdown();
		HRESULT Render(ID3D11DeviceContext*, int, int);

		int GetIndexCount();
		int GetVertexCount();
		ID3D11ShaderResourceView* GetTexture();

	private:
		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();
		HRESULT UpdateBuffers(ID3D11DeviceContext*, int, int);
		void RenderBuffers(ID3D11DeviceContext*);

		bool LoadTexture(ID3D11Device*, WCHAR*);
		void ReleaseTexture();

	private:
		ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		int m_vertexCount, m_indexCount;
		Texture* m_Texture;
		int m_screenWidth, m_screenHeight;
		int m_bitmapWidth, m_bitmapHeight;
		int m_previousPosX, m_previousPosY;
};

#endif