#ifndef _MODELFACTORY_H_
#define _MODELFACTORY_H_

#include <time.h>
#include <vector>
#include <map>
#include "ModelObject.h"

using namespace std;

struct DeletedModelInfo 
{
	char* fileName;
	bool isInst;
};

class ModelFactory
{
	public:
		static ModelFactory* GetInstance();

		void Shutdown();
		int GetModelCount();
		ModelObject* CreateInstancedModel(ID3D11Device* device, HWND hwnd, wstring modelName, wstring fileName, int numberOfModels);
		ModelObject* CreateOrdinaryModel(ID3D11Device* device, HWND hwnd, wstring modelName, wstring fileName, ModelClass::VertexTypes vertexType);

		map<wstring, ModelObject*>& GetVectorOfObjects();
		void SetPositionForObject(D3DXVECTOR3 positionVector, wstring modelName);

		ModelObject* GetObjectByName(wstring modelName);

		ID3D11Buffer* GetInstanceBuffer();

	private:
		ModelFactory();
		ModelFactory(const ModelFactory&);
		~ModelFactory();
		int m_modelCount;
		ID3D11Buffer *m_instanceBuffer;
		map<wstring, ModelObject*> mVectorOfObjects;
		static ModelFactory* m_pInstance;
};

#endif