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
#include "shaders/TerrainShader.h"
#include "Log.h"
#include "BufferManager.h"

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
			D3DXVECTOR4 texture;
			D3DXVECTOR3 normal;
			D3DXVECTOR4 color;
		};

		struct HeightMapType 
		{ 
			float x, y, z;
			float tu, tv;
			float nx, ny, nz;
			float r, g, b;
			int rIndex, gIndex, bIndex;
		};

		struct VectorType 
		{ 
			float x, y, z;
		};

		struct MaterialGroupType 
		{ 
			int textureIndex1, textureIndex2, alphaIndex;
			int red, green, blue;
			ID3D11Buffer *vertexBuffer, *indexBuffer;
			int vertexCount, indexCount;
			VertexType* vertices;
			unsigned long* indices;

			MaterialGroupType() : textureIndex1(-1), textureIndex2(-1), alphaIndex(-1), red(-1), green(-1), blue(-1),
					vertexCount(-1), indexCount(-1)
			{
				vertexBuffer = 0;
				indexBuffer = 0;
				vertices = 0;
				indices = 0;
			}
		};

	public:
		Terrain();
		Terrain(const Terrain&);
		~Terrain();

		bool InitializeWithQuadTree(ID3D11Device* device, const wchar_t* heightMapFileName, wchar_t* textureFilename,
									const wchar_t* colorMapFilename);

		bool InitializeWithMaterials(ID3D11Device* device, wchar_t* heightMapFileName, char* materialsFilename,
									char* materialMapFilename, wchar_t* colorMapFilename, wchar_t* detailMapFilename);

		bool Render(ID3D11DeviceContext* deviceContext, TerrainShader* shader, D3DXMATRIX worldMatrix, 
			D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

		void						Shutdown();
		ID3D11ShaderResourceView*	GetTexture();
		ID3D11ShaderResourceView*	GetDetailMapTexture();
		int							GetVertexCount();
		void						CopyVertexArray(void*);
		void						GetTerrainSize(int&, int&);

	private:
		bool LoadHeightMap(ID3D11Device* device, const wchar_t* heightMapFileName);
		void NormalizeHeightMap();
		bool CalculateNormals();
		void ShutdownHeightMap();

		void CalculateTextureCoordinates();
		bool LoadTexture(ID3D11Device*, WCHAR*);
		HRESULT LoadColorMap(const wchar_t*);
		void ReleaseTexture();

		bool InitializeBuffers(ID3D11Device*);
		void ShutdownBuffers();

		bool LoadMaterialFile(char* filename, char* materialMapFilename, ID3D11Device* device);
		bool LoadMaterialMap(char*);
		bool LoadMaterialBuffers(ID3D11Device*);
		void ReleaseMaterials();

	private:
		int m_terrainWidth, m_terrainHeight;
		int m_vertexCount;
		HeightMapType* m_heightMap;
		Texture* m_Texture, *m_DetailTexture;
		VertexType* m_vertices;

		int m_textureCount, m_materialCount;
		Texture* m_Textures;
		MaterialGroupType* m_Materials;
	};

#endif