////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>
#include "Log.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
	public:
		CameraClass();
		CameraClass(const CameraClass&);
		~CameraClass();

		void SetPosition(float, float, float);
		void SetRotation(float, float, float);

		D3DXVECTOR3 GetPosition();
		D3DXVECTOR3 GetRotation();
		D3DXVECTOR3 GetNormalDirectionVector();
		D3DXVECTOR3 GetNormalRightVector();

		void Render();
		void GetViewMatrix(D3DXMATRIX&);

		void RenderReflection(float);
		D3DXMATRIX GetReflectionViewMatrix();

	private:
		float mPositionX, mPositionY, mPositionZ;
		float mRotationX, mRotationY, mRotationZ;
		float mCamYaw, mCamPitch;

		D3DXVECTOR3 mCameraLookAt, mCameraUp, mCameraRight;

		D3DXMATRIX mViewMatrix;

		D3DXMATRIX m_reflectionViewMatrix;
};

#endif