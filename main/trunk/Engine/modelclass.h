////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <vector>
#include <fstream>
#include <istream>
#include <time.h>
#include <vector>
#include <sstream>

#include "textureclass.h"
#include "Log.h"
#include "BufferManager.h"
#include "BasicResource.h"
#include "FileSystemHelper.h"

using namespace std;

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
	string matName;
	D3DXVECTOR4 difColor;
	int texArrayIndex;
	bool hasTexture;
	bool transparent;
};

class ModelClass : BasicResource
{
	private:
		struct BasicVertexType 
		{
			BasicVertexType(){}
			BasicVertexType(float x, float y, float z) : position(x, y, z){}
			D3DXVECTOR3 position;
		};

		struct VertexTypeColor : BasicVertexType
		{
			VertexTypeColor(){}
			VertexTypeColor(float x, float y, float z, float r, float g, float b, float a) : 
													BasicVertexType(x, y, z), color(r, g, b, a){}
			D3DXVECTOR4 color;
		};

		struct VertexTypeTexture : BasicVertexType
		{
			VertexTypeTexture(){}
			VertexTypeTexture(float x, float y, float z, float u, float v) :
													BasicVertexType(x, y, z), texture(u, v){}
			D3DXVECTOR2 texture;
		};

		struct VertexTypeLight : VertexTypeTexture
		{
			VertexTypeLight(){}
			VertexTypeLight(float x, float y, float z, float u, float v, float nx, float ny, float nz) : 
													VertexTypeTexture(x, y, z, u, v), normal(nx, ny, nz){}
			D3DXVECTOR3 normal;
		};

		struct VertexTypeNormalMap : VertexTypeLight
		{
			VertexTypeNormalMap(){}
			VertexTypeNormalMap(float x, float y, float z, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz, float bx, float by, float bz) :
						VertexTypeLight(x, y, z, u, v, nx, ny, nz), tangent(tx, ty, tz), binormal(bx, by, bz){}

			D3DXVECTOR3 tangent;
			D3DXVECTOR3 binormal;
		};

		struct TempVertexType : VertexTypeTexture
		{
			TempVertexType(){}
			TempVertexType(float x, float y, float z, float u, float v, float nx, float ny, float nz) : 
							VertexTypeTexture(x, y, z, u, v), normal(nx, ny, nz){}
			D3DXVECTOR3 normal;
		};

		struct VectorType
		{
			float x, y, z;
		};

	public:
		enum VertexTypes
		{
			ColorVertexType = 0,
			TextureVertexType,
			LightVertexType,
			NormalMapVertexType,
			VertexTypeTemp,

			VertexTypeCount
		};

		ModelClass();
		ModelClass(const ModelClass&);
		virtual ~ModelClass();

		bool InitializeInstanced(ID3D11Device* device, wstring filename, InstanceType* instances /* vector<InstanceType*> instancesVector */, int numModels);
		bool InitializeOrdinary(ID3D11Device* device, wstring filename, FileSystemHelper::FileExtensions fileExtension, VertexTypes vertexType);

		virtual void Shutdown();
		void RenderInstanced(ID3D11DeviceContext* deviceContext, VertexTypes vertexType);
		void RenderOrdinary(ID3D11DeviceContext* deviceContext, VertexTypes vertexType);

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
		bool CreateVertexBuffer(ID3D11Device* device, size_t size, void* vertices, ID3D11Buffer** buffer);

		void ShutdownBuffers();

		bool LoadModelFromTXT(wstring modelFilename);
		bool LoadTXTModel(wstring modelFilename);
		bool LoadModelFromOBJ(ID3D11Device* device, wstring modelFilename);

		bool CheckChar(ifstream& fileIn, wchar_t charToCheck);
		void ReleaseModel();

		void CalculateModelVectors();
		void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
		void CalculateNormal(VectorType, VectorType, VectorType&);

		void RenderBuffersInstanced(ID3D11DeviceContext* deviceContext, VertexTypes vertexType);
		void RenderBuffersOrdinaryForTXTFile(ID3D11DeviceContext* deviceContext, VertexTypes vertexType);
		void RenderBuffersOrdinaryForOBJFile(ID3D11DeviceContext* deviceContext, VertexTypes vertexType);
		size_t GetVertexTypeSize(VertexTypes vertxtype);

	private:
		ID3D11Buffer *mVertexBuffer, *mInstanceBuffer, *mIndexBuffer;
		int mVertexCount, mInstanceCount, mIndexCount;
		int meshSubsets;
		std::vector<int> meshSubsetIndexStart;
		std::vector<int> meshSubsetTexture;
		std::vector<ID3D11ShaderResourceView*> meshSRV;
		std::vector<std::wstring> textureNameArray;
		ModelType* mModel;
		char mModelFileName[50];
		VertexTypes mVertexType;

		//int meshSubsets;
		//vector<int> meshSubsetIndexStart;

		// TODO: Probably need this in material
		// For .MLT material
		//vector<int> meshSubsetTexture;
		ID3D11BlendState* Transparency;
		//vector<ID3D11ShaderResourceView*> meshSRV;

		vector<SurfaceMaterial> material;
		//vector<std::wstring> textureNameArray;

		FileSystemHelper::FileExtensions mFileExtension;
};

#endif