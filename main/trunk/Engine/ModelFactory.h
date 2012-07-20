#ifndef _MODELFACTORY_H_
#define _MODELFACTORY_H_

#include <time.h>
#include <vector>
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
		ModelObject* CreateInstancedModel(ID3D11Device* device, HWND hwnd, char* modelName, string fileName, int numberOfModels);
		ModelObject* CreateOrdinaryModel(ID3D11Device* device, HWND hwnd, const char* modelName, string fileName, ModelClass::VertexTypes vertexType);

		// TODO: Integrate in loading pipeline
		ModelObject* CreateSimpleModel(ID3D11Device* device, HWND hwnd, char* modelName, string fileName);

		vector<ModelObject*>& GetVectorOfObjects();
		void SetPositionForObject(D3DXVECTOR3 positionVector, char * modelName);

		ModelObject* GetObjectByName(const char * modelName);

		ID3D11Buffer* GetInstanceBuffer();

	private:
		ModelFactory();
		ModelFactory(const ModelFactory&);
		~ModelFactory();
		int GetExistingModelIndex(char* modelFileName, bool isInstanced);
		bool HasObjectBeenDeleted(ModelObject* currentDeletingObject, vector<DeletedModelInfo*>& vectorOfDeletedModelInfos);
		int m_modelCount;
		ID3D11Buffer *m_instanceBuffer;
		vector<ModelObject*> mVectorOfObjects;
		static ModelFactory* m_pInstance;
};

#endif