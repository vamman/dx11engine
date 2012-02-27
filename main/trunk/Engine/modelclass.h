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
#include <vector>
#include <sstream>
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

enum ModelFileFormat
{
	MODEL_FILE_TXT = 0,
	MODEL_FILE_OBJ
};

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

// For .MLT material
struct SurfaceMaterial
{
	std::wstring matName;
	D3DXVECTOR4 difColor;
	int texArrayIndex;
	bool hasTexture;
	bool transparent;
};

class ModelClass
{
	private:
		struct VertexTypeColor
		{
			VertexTypeColor(){}
			VertexTypeColor(float x, float y, float z, float r, float g, float b, float a) : position(x, y, z), color(r, g, b, a){}
			D3DXVECTOR3 position;
			D3DXVECTOR4 color;
		};

		struct VertexTypeTexture
		{
			VertexTypeTexture(){}
			VertexTypeTexture(float x, float y, float z, float u, float v) : position(x, y, z), texture(u, v){}
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
		};

		struct VertexTypeLight
		{
			VertexTypeLight(){}
			VertexTypeLight(float x, float y, float z, float u, float v, float nx, float ny, float nz) : 
																							position(x, y, z), texture(u, v), normal(nx, ny, nz){}
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;
		};

		struct VertexTypeNormalMap
		{
			VertexTypeNormalMap(){}
			VertexTypeNormalMap(float x, float y, float z, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz, float bx, float by, float bz) :
																							position(x, y, z), texture(u, v), normal(nx, ny, nz), tangent(tx, ty, tz), binormal(bx, by, bz){}
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;
			D3DXVECTOR3 tangent;
			D3DXVECTOR3 binormal;
		};

		struct TempVertexType
		{
			TempVertexType(){}
			TempVertexType(float x, float y, float z, float u, float v, float nx, float ny, float nz) : 
																							position(x, y, z), texture(u, v), normal(nx, ny, nz){}
			D3DXVECTOR3 position;
			D3DXVECTOR2 texture;
			D3DXVECTOR3 normal;

			// float x, y, z;
			// float tu, tv;
			// float nx, ny, nz;
		};

		struct VectorType
		{
			float x, y, z;
		};

	public:
		ModelClass();
		ModelClass(const ModelClass&);
		~ModelClass();

		bool InitializeInstanced(ID3D11Device* device, wstring filename, InstanceType* instances /* vector<InstanceType*> instancesVector */, int numModels);
		bool InitializeOrdinary(ID3D11Device* device, wstring filename, ModelFileFormat fileFormat);

		void Shutdown();
		void RenderInstanced(ID3D11DeviceContext* deviceContext);
		void RenderOrdinary(ID3D11DeviceContext* deviceContext);

		void SetPosition(D3DXVECTOR3 posVector);
		D3DXVECTOR3* GetPosition();

		int GetIndexCount() const;
		int GetVertexCount() const;
		int GetInstanceCount() const;
		char* GetModelFileName() const;

		ModelType* GetModelTypeData() const;
		ID3D11Buffer* GetVertexBuffer() const;
		ID3D11Buffer* GetIndexBuffer() const;
		ID3D11Buffer* GetInstanceBuffer() const;

	private:
		bool InitializeBuffersInstanced(ID3D11Device* device, InstanceType* instances /* vector<InstanceType*> instancesVector */, int numModels);
		bool InitializeBuffersOrdinary(ID3D11Device* device);
		void ShutdownBuffers();
		void RenderBuffersInstanced(ID3D11DeviceContext* deviceContext);
		void RenderBuffersOrdinary(ID3D11DeviceContext* deviceContext);

		bool LoadModelFromTXT(char* modelFilename);
		bool LoadTXTModel(char* modelFilename);
		bool LoadModelFromOBJ(ID3D11Device* device, wstring filename);
		void ReleaseModel();

		void CalculateModelVectors();
		void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
		void CalculateNormal(VectorType, VectorType, VectorType&);

	private:
		ID3D11Buffer *mVertexBuffer, *mInstanceBuffer, *mIndexBuffer;
		int mVertexCount, mInstanceCount, mIndexCount;
		ModelType* mModel;
		char mModelFileName[50];

		int meshSubsets;
		vector<int> meshSubsetIndexStart;

		// TODO: Probably need this in material
		// For .MLT material
		vector<int> meshSubsetTexture;
		ID3D11BlendState* Transparency;
		vector<ID3D11ShaderResourceView*> meshSRV;

		vector<SurfaceMaterial> material;
		vector<std::wstring> textureNameArray;
};

#endif