////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include "textureclass.h"
#include "timerclass.h"
#include "Log.h"

const int TEXTURE_REPEAT = 16;

////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class Terrain
{
	private:

		struct VertexType
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;
		};

		struct HeightMapType 
		{ 
			float x, y, z;
			float tu, tv;
			float nx, ny, nz;
		};

		struct VectorType 
		{ 
			float x, y, z;
		};

	public:
		Terrain();
		Terrain(const Terrain&);
		~Terrain();

		bool Initialize(ID3D11Device* device, char* heightMapFileName, WCHAR* textureFilename);
		void Shutdown();
		// void Render(ID3D11DeviceContext*);

		// int GetIndexCount();
		ID3D11ShaderResourceView* GetTexture();
		int GetVertexCount();
		void CopyVertexArray(void*);

	private:
		bool LoadHeightMap(char* heightMapFileName);
		void NormalizeHeightMap();
		bool CalculateNormals();
		void ShutdownHeightMap();

		void CalculateTextureCoordinates();
		bool LoadTexture(ID3D11Device*, WCHAR*);
		void ReleaseTexture();

		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();
		// void RenderBuffers(ID3D11DeviceContext*);

	private:
		int m_terrainWidth, m_terrainHeight;
		int m_vertexCount;
		// ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
		HeightMapType* m_heightMap;
		TextureClass* m_Texture;
		VertexType* m_vertices;
	};

#endif