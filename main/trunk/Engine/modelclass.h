////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <time.h>
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "texturearrayclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
struct InstanceType
{
	D3DXVECTOR3 position;
};

struct ModelType
{
	float x, y, z;
	float tu, tv;
	float nx, ny, nz;
	float tx, ty, tz;
	float bx, by, bz;
};

class ModelClass
{
	private:
		struct VertexTypeColor
		{
			D3DXVECTOR3 position;
			D3DXVECTOR4 color;
		};

		struct VertexTypeTexture
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};

		struct VertexTypeLight
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;
		};

		struct VertexTypeNormalMap
		{
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;
			D3DXVECTOR3 tangent;
			D3DXVECTOR3 binormal;
		};

		struct TempVertexType
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
		ModelClass();
		ModelClass(const ModelClass&);
		~ModelClass();

		bool InitializeInstanced(ID3D11Device* device, char* filename, InstanceType* instances /* vector<InstanceType*> instancesVector */, int numModels);
		bool InitializeOrdinary(ID3D11Device*, char*);

		void Shutdown();
		void RenderInstanced(ID3D11DeviceContext* deviceContext);
		void RenderOrdinary(ID3D11DeviceContext* deviceContext);

		void SetPosition(D3DXVECTOR3 posVector);
		D3DXVECTOR3* GetPosition();

		int GetIndexCount() const;
		int GetVertexCount() const;
		int GetInstanceCount() const;
		char* GetModelFileName() const;
		vector<ID3D11ShaderResourceView*>& GetTextureVector() const;
		TextureArrayClass* GetTextureArray() const;

		ModelType* GetModelTypeData() const;
		ID3D11Buffer* GetVertexBuffer() const;
		ID3D11Buffer* GetIndexBuffer() const;
		ID3D11Buffer* GetInstanceBuffer() const;

	private:
		bool InitializeBuffersInstanced(ID3D11Device* device, InstanceType* instances /* vector<InstanceType*> instancesVector */, int numModels);
		bool InitializeBuffersOrdinary(ID3D11Device*);
		void ShutdownBuffers();
		void RenderBuffersInstanced(ID3D11DeviceContext* deviceContext);
		void RenderBuffersOrdinary(ID3D11DeviceContext* deviceContext);

		bool LoadModel(char*);
		void ReleaseModel();

		void CalculateModelVectors();
		void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
		void CalculateNormal(VectorType, VectorType, VectorType&);

	private:
		ID3D11Buffer *mVertexBuffer, *mInstanceBuffer, *mIndexBuffer;
		int mVertexCount, mInstanceCount, mIndexCount;
		ModelType* mModel;
		char mModelFileName[50];
};

#endif