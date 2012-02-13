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
			D3DXVECTOR4 color;
		};

		struct HeightMapType 
		{ 
			float x, y, z;
			float tu, tv;
			float nx, ny, nz;
			float r, g, b;
		};

		struct VectorType 
		{ 
			float x, y, z;
		};

	public:
		Terrain();
		Terrain(const Terrain&);
		~Terrain();

		bool Initialize(ID3D11Device* device, char* heightMapFileName, WCHAR* textureFilename, char* colorMapFilename);
		void Shutdown();
		ID3D11ShaderResourceView* GetTexture();
		int GetVertexCount();
		void CopyVertexArray(void*);
		void GetTerrainSize(int&, int&);

	private:
		bool LoadHeightMap(char* heightMapFileName);
		void NormalizeHeightMap();
		bool CalculateNormals();
		void ShutdownHeightMap();

		void CalculateTextureCoordinates();
		bool LoadTexture(ID3D11Device*, WCHAR*);
		bool LoadColorMap(char*);
		void ReleaseTexture();

		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();

	private:
		int m_terrainWidth, m_terrainHeight;
		int m_vertexCount;
		HeightMapType* m_heightMap;
		TextureClass* m_Texture;
		VertexType* m_vertices;
	};

#endif