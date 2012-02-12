
#ifndef _QUADTREECLASS_H_
#define _QUADTREECLASS_H_

const int MAX_TRIANGLES = 5000;

#include "Terrain.h"
#include "frustumclass.h"
#include "Shaders/TerrainShader.h"
#include "timerclass.h"
#include "Log.h"

const float LINE_HEIGHT = 50.0f;

class QuadTree
{
	private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct VertexTypeLine
	{
		D3DXVECTOR3 position;
		// D3DXVECTOR4 color;
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
		ID3D11Buffer *vertexBuffer, *indexBuffer, *linesVertixes, *lineInixes;
		VectorType* vertexArray;
		NodeType* nodes[4];
		int depthLevel;
	};

	public:
		QuadTree(void);
		~QuadTree(void);
		bool Initialize(Terrain*, ID3D11Device*);
		void Shutdown();
		void Render(FrustumClass*, ID3D11DeviceContext*, TerrainShader*);

		int GetDrawCount();
		bool GetHeightAtPosition(float, float, float&);

	private:
		void CalculateMeshDimensions(int, float&, float&, float&);
		void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
		void CreateBoxBufferForNode(NodeType &node, ID3D11Device* device);
		int CountTriangles(float, float, float);
		bool IsTriangleContained(int, float, float, float);
		void ReleaseNode(NodeType*);
		void RenderNode(NodeType* node, FrustumClass* frustum, ID3D11DeviceContext* deviceContext, TerrainShader* shader);
		void RenderDebugBoxForNode(NodeType* node, ID3D11DeviceContext* deviceContext);
		void DrawLine(ID3D11Buffer *vertexBuffer, ID3D11Buffer *indexBuffer, ID3D11DeviceContext* deviceContext);
		void FindNode(NodeType*, float, float, float&);
		bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

	private:
		int m_triangleCount, m_drawCount;
		VertexType* m_vertexList;
		NodeType* m_parentNode;
		int mDepth;
		ID3D11Device* mDevice;
};

#endif

