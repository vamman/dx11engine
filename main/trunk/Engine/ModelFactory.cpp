#include "ModelFactory.h"

ModelFactory::ModelFactory()
{
}

ModelFactory::ModelFactory(const ModelFactory& other)
{
}


ModelFactory::~ModelFactory()
{
}

ModelObject* ModelFactory::CreateInstancedModel(ID3D11Device* device, HWND hwnd, char* modelName, char* fileName, int numberOfModels)
{
	bool result;
	m_modelCount = numberOfModels;
	vector<InstanceType*> instancesVector;

	ModelClass* model = new ModelClass;
	// Create the model object.
	if(!model)
	{
		return false;
	}

	// Load the instance array with data.
	// Seed the random generator with the current time.
	srand((unsigned int)time(NULL));

	// Go through all the models and randomly generate the model color and position.
	float positionX = 60.0f; // 60.0f;
	float positionY = 0.0f; // 0.0f;
	float positionZ = 55.0f; // 55.0f;

	//InstanceType* instance = new InstanceType();
	InstanceType* instances = new InstanceType[numberOfModels];

	for(int i = 0; i < numberOfModels; ++i)
	{
		// Generate a random position in front of the viewer for the mode.
		positionX += (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		positionY += (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		positionZ += ((((float)rand()-(float)rand())/RAND_MAX) * 10.0f) + 5.0f;

		// Load the instance array with data.
		instances[i].position = D3DXVECTOR3(positionX, positionY, positionZ);

		// instance->position = D3DXVECTOR3(positionX, positionY, positionZ);
		// instancesVector.push_back(instance);
	}

	// Initialize the model object.
	result = model->InitializeInstanced(device, fileName, instances /* instancesVector */, numberOfModels);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize an instanced model object.", L"Error", MB_OK);
		return false;
	}

	D3DXVECTOR3 posVec = D3DXVECTOR3(positionX, positionY, positionZ);
	ModelObject* modelObject = new ModelObject(posVec, model, modelName, true, instancesVector);

	mVectorOfObjects.push_back(modelObject);

	return modelObject;
}

ModelObject* ModelFactory::CreateOrdinaryModel(ID3D11Device* device, HWND hwnd, const char* modelName, char* fileName)
{
	bool result;
	D3DXVECTOR3 posVec;
	ModelClass* model = new ModelClass;
	// Create the model object.
	if(!model)
	{
		return false;
	}

	int existingIndex = GetExistingModelIndex(fileName, false);
	// If this model was already loaded before
	if (existingIndex != -1)
	{
		model = mVectorOfObjects[existingIndex]->GetModel();
		// Copy the model
		model = new ModelClass(*model);
	}
	// Else load it
	else
	{
		// Initialize the model.
		result = model->InitializeOrdinary(device, fileName);
		if(!result)
		{
			MessageBox(hwnd, L"Could not initialize an instanced model object.", L"Error", MB_OK);
			return false;
		}
	}

	// Set default position
	posVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	ModelObject* modelObject = new ModelObject(posVec, model, modelName);

	mVectorOfObjects.push_back(modelObject);

	return modelObject;
}

int ModelFactory::GetExistingModelIndex(char* modelFileName, bool isInstanced)
{
	vector<ModelObject*>::iterator modelIt;
	int index = 0;
	for (modelIt = mVectorOfObjects.begin(); modelIt != mVectorOfObjects.end(); ++modelIt)
	{
		ModelObject* modelObject = (*modelIt);
		if ( (strcmp(modelObject->GetModel()->GetModelFileName(), modelFileName) == 0) && (modelObject->IsInstanced() == isInstanced) )
		{
			return index;
		}
		index++;
	}
	return -1;
}

void ModelFactory::SetPositionForObject(D3DXVECTOR3 positionVector, char * modelName)
{
	vector<ModelObject*>::iterator modelIt;
	for (modelIt = mVectorOfObjects.begin(); modelIt != mVectorOfObjects.end(); ++modelIt)
	{
		if (strcmp((*modelIt)->GetModelName(), modelName) == 0)
		{
			(*modelIt)->SetPosition(positionVector);
			break;
		}
	}
}

void ModelFactory::Shutdown()
{
	vector<DeletedModelInfo*> vectorOfDeletedModelInfos;
	vector<ModelObject*>::iterator modelIt;
	int deletedModes = 0;

	for (modelIt = mVectorOfObjects.begin(); modelIt != mVectorOfObjects.end(); ++modelIt)
	{
		ModelObject* object = (*modelIt);
		if (!HasObjectBeenDeleted(object, vectorOfDeletedModelInfos))
		{
			DeletedModelInfo* deletedModel = new DeletedModelInfo();
			deletedModel->fileName = object->GetModel()->GetModelFileName();
			deletedModel->isInst = object->IsInstanced();
			object->GetModel()->Shutdown();
			vectorOfDeletedModelInfos.push_back(deletedModel);
			deletedModes++;
		}
	}
}

bool ModelFactory::HasObjectBeenDeleted(ModelObject* currentDeletingObject, vector<DeletedModelInfo*>& vectorOfDeletedModelInfos)
{
	vector<DeletedModelInfo*>::iterator currDeletedObjInfo;
	for (currDeletedObjInfo = vectorOfDeletedModelInfos.begin(); currDeletedObjInfo != vectorOfDeletedModelInfos.end(); ++currDeletedObjInfo)
	{
		if (strcmp(currentDeletingObject->GetModel()->GetModelFileName() , (*currDeletedObjInfo)->fileName) == 0 &&
			currentDeletingObject->IsInstanced() == (*currDeletedObjInfo)->isInst)
		{
			return true;
		}
	}
	return false;
}

int ModelFactory::GetModelCount()
{
	return m_modelCount;
}

vector<ModelObject*>& ModelFactory::GetVectorOfObjects()
{
	return mVectorOfObjects;
}

ModelObject* ModelFactory::GetObjectByName(const char * modelName)
{
	vector<ModelObject*>::iterator modelIt;
	for (modelIt = mVectorOfObjects.begin(); modelIt != mVectorOfObjects.end(); ++modelIt)
	{
		if (strcmp((*modelIt)->GetModelName(), modelName) == 0)
		{
			return (*modelIt);
		}
	}
	return 0;
}

ID3D11Buffer* ModelFactory::GetInstanceBuffer()
{
	return m_instanceBuffer;
}