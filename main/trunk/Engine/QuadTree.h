
#ifndef _QUADTREECLASS_H_
#define _QUADTREECLASS_H_

const int MAX_TRIANGLES = 15000;

#include "Terrain.h"
#include "frustumclass.h"
#include "Shaders/TerrainShader.h"
#include "timerclass.h"
#include "Log.h"
#include "BoundingBox.h"
#include "InputClass.h"
#include "BufferManager.h"

const float LINE_HEIGHT = 50.0f;

class QuadTree
{
	private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 color;
	};

	struct VectorType
	{
		float x, y, z;
	};

	// Each node in the quad tree will be defined as follows with position, size, triangle count, buffers, and four child nodes: 
	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		VectorType* vertexArray;
		NodeType* nodes[4];
		BoundingBox* bBox;
		int depthLevel;
	};

	public:
		QuadTree(void);
		~QuadTree(void);
		bool Initialize(Terrain*, ID3D11Device*);
		void Shutdown();
		void Render(FrustumClass*, ID3D11DeviceContext*, TerrainShader*, bool isAllowToBBRender);

		int GetDrawCount();
		bool GetHeightAtPosition(float, float, float&);

	private:
		void CalculateMeshDimensions(int, float&, float&, float&);
		void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
		int CountTriangles(float, float, float);
		bool IsTriangleContained(int, float, float, float);
		void ReleaseNode(NodeType*);
		void RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShader* shader);
		void FindNode(NodeType*, float, float, float&);
		bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

	private:
		int m_triangleCount, m_drawCount;
		VertexType* m_vertexList;
		NodeType* m_parentNode;
		bool mIsAllowToBBRender;
		int mDepth;
};

#endif

