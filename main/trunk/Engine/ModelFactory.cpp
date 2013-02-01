#include "ModelFactory.h"
#include "FileSystemHelper.h"

ModelFactory* ModelFactory::m_pInstance = NULL;  

ModelFactory* ModelFactory::GetInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new ModelFactory;
	}
	return m_pInstance;
}

ModelFactory::ModelFactory() : m_modelCount(0)
{
	Log::GetInstance()->WriteTimedMessageToOutput(100, "ModelFactory::ModelFactory");
	m_instanceBuffer = 0;
	mVectorOfObjects.clear();
}

ModelFactory::ModelFactory(const ModelFactory& other)
{
}


ModelFactory::~ModelFactory()
{
}

ModelObject* ModelFactory::CreateInstancedModel(ID3D11Device* device, HWND hwnd, wstring modelName, wstring fileName, int numberOfModels)
{
	bool result;
	m_modelCount = numberOfModels;
	vector<InstanceType*> instancesVector;

	ModelClass* model = new ModelClass();
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

	ModelObject* modelObject = new ModelObject(D3DXVECTOR3(positionX, positionY, positionZ), model, modelName, true, instancesVector);
	mVectorOfObjects[modelName] = modelObject;
	return modelObject;
}

ModelObject* ModelFactory::CreateOrdinaryModel(ID3D11Device* device, HWND hwnd, wstring modelName, wstring fileName, ModelClass::VertexTypes vertexType)
{
	bool result;
	ModelClass* model = new ModelClass;

	FileSystemHelper::FileExtensions fileFormat = FileSystemHelper::GetFileExtension(fileName);

	if (mVectorOfObjects[modelName])
	{
		return mVectorOfObjects[modelName];
	}
	else
	{
		// Initialize the model.
		result = model->InitializeOrdinary(device, fileName, fileFormat, vertexType);
		if(!result)
		{
			MessageBox(hwnd, L"Could not initialize an ordinary model object.", L"Error", MB_OK);
			return false;
		}
	}

	// Set default position
	ModelObject* modelObject = new ModelObject(D3DXVECTOR3(0.0f, 0.0f, 0.0f), model, modelName);
	mVectorOfObjects[modelName] = modelObject;
	return modelObject;
}

void ModelFactory::SetPositionForObject(D3DXVECTOR3 positionVector, wstring modelName)
{
	ModelObject* modelObj = mVectorOfObjects[modelName];
	modelObj->SetPosition(positionVector);
}

void ModelFactory::Shutdown()
{
	map<wstring, ModelObject* >::iterator iter;
	for (iter = mVectorOfObjects.begin(); iter != mVectorOfObjects.end(); iter++)
	{
		iter->second->GetModel()->Shutdown();
	}
}

int ModelFactory::GetModelCount()
{
	return m_modelCount;
}

map<wstring, ModelObject*>& ModelFactory::GetVectorOfObjects()
{
	return mVectorOfObjects;
}

ModelObject* ModelFactory::GetObjectByName(wstring modelName)
{
	return mVectorOfObjects[modelName];
}

ID3D11Buffer* ModelFactory::GetInstanceBuffer()
{
	return m_instanceBuffer;
}