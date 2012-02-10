#include "ModelObject.h"


ModelObject::ModelObject() : mScale(1.0f, 1.0f, 1.0f), mRotationAngle(0)
{
	mModel = 0; // Set default material instead
	mMaterial = 0;
}

ModelObject::ModelObject(D3DXVECTOR3 position, ModelClass* model, const char* modelName,
	bool isInstanced, vector<InstanceType*>& instancesVector) : mScale(1.0f, 1.0f, 1.0f), mRotationAngle(0)
{
	mMaterial = 0; // Set default material instead
	mPosition = position;
	mModel = model;
	strcpy(mModelName, modelName);
	mIsInstanced = isInstanced;
	mInstancesVector = instancesVector;
}

ModelObject::ModelObject(D3DXVECTOR3 position, ModelClass* model, const char* modelName) : mScale(1.0f, 1.0f, 1.0f), mRotationAngle(0)
{
	mMaterial = 0; // Set default material instead
	mPosition = position;
	mModel = model;
	strcpy(mModelName, modelName);
	mIsInstanced = false;
}

Material* ModelObject::GetMaterial()
{
	return mMaterial;
}

void ModelObject::SetMaterial(Material* material)
{
	mMaterial = material;
}

vector<InstanceType*>& ModelObject::GetInstances()
{
	return mInstancesVector;
}

ModelClass* ModelObject::GetModel()
{
	return mModel;
}

char* ModelObject::GetModelName()
{
	return mModelName;
}

bool ModelObject::IsInstanced()
{
	return mIsInstanced;
}