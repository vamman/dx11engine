///////////////////////////////////////////////////////////////////////////////
// Class name: ModelObject
///////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "modelclass.h"
#include "Material.h"

// TODO: Add hierarchy relations (parent/child) to store relational hierarchy like single ModelObject
class ModelObject
{
public:
	ModelObject();
	ModelObject(D3DXVECTOR3 position, ModelClass* model, const char* modelName,
				bool isInstanced, vector<InstanceType*>& instancesVector);
	ModelObject(D3DXVECTOR3 position, ModelClass* model, const char* modelName);
	~ModelObject();

	Material* GetMaterial();
	void SetMaterial(Material* material);
	vector<InstanceType*>& GetInstances();
	ModelClass* GetModel();
	char* GetModelName();
	bool IsInstanced();

	inline D3DXVECTOR3 GetScale()
	{
		return mScale;
	}
	inline void SetScale(D3DXVECTOR3 scale)
	{
		mScale = scale;
	}

	inline D3DXVECTOR3 GetPosition()
	{
		return mPosition;
	}
	inline void SetPosition(D3DXVECTOR3 position)
	{
		mPosition = position;
	}

	inline float GetRotation()
	{
		return mRotationAngle;
	}
	inline void SetRotation(float angle)
	{
		mRotationAngle = angle;
	}

	inline D3DXMATRIX GetWorldMatrix()
	{
		D3DXMATRIX scaleMatrix, transitionMatrix, rotationMatrix;

		// Do rotation
		D3DXMatrixRotationY(&rotationMatrix, mRotationAngle);

		// Do scale
		D3DXMatrixScaling(&scaleMatrix, mScale.x, mScale.y, mScale.z);

		// Do transition
		D3DXMatrixTranslation(&transitionMatrix, mPosition.x, mPosition.y, mPosition.z);

		// Create world matrix
		mWorldMatrix = rotationMatrix * scaleMatrix * transitionMatrix;

		return mWorldMatrix;
	}
	inline void SetWorldMatrix(D3DXMATRIX worldMatrix)
	{
		mWorldMatrix = worldMatrix;
	}


private:
	D3DXVECTOR4* mColor;
	ModelClass* mModel;
	char mModelName[20];
	bool mIsInstanced;
	Material* mMaterial;
	vector<InstanceType*> mInstancesVector;

	D3DXVECTOR3 mScale;
	D3DXVECTOR3	mPosition;
	float		mRotationAngle;
	D3DXMATRIX	mWorldMatrix;
	
};