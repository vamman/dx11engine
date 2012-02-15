// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

CameraClass::CameraClass() : mCamYaw(0.0f), mCamPitch(0.0f), mPositionX(0.0f),
							mPositionY(0.0f), mPositionZ(0.0f), mRotationX(0.0f), mRotationY(0.0f), mRotationZ(0.0f)
{
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	mPositionX = x;
	mPositionY = y;
	mPositionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	mRotationX = x;
	mRotationY = y;
	mRotationZ = z;
	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(mPositionX, mPositionY, mPositionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(mRotationX, mRotationY, mRotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	mCameraLookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mCameraRight = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	mCameraUp = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = mPositionX;
	position.y = mPositionY;
	position.z = mPositionZ;

	// Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = mRotationX * 0.0174532925f;
	yaw   = mRotationY * 0.0174532925f;
	roll  = mRotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	mCameraUp = up;

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;
	mCameraLookAt = lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&mViewMatrix, &position, &mCameraLookAt, &up);

	return;
}

D3DXVECTOR3 CameraClass::GetNormalDirectionVector()
{
	D3DXVECTOR3 cameraPosition = D3DXVECTOR3(mPositionX, mPositionY, mPositionZ);
	D3DXVECTOR3 cameraDirection = mCameraLookAt - cameraPosition;
	D3DXVECTOR3 normalCameraDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3Normalize(&normalCameraDirection, &cameraDirection);
	return normalCameraDirection;
}

D3DXVECTOR3 CameraClass::GetNormalRightVector()
{
	D3DXVECTOR3 normalCameraRight = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 normalCameraUp = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVec3Normalize(&normalCameraUp, &mCameraUp);
	D3DXVECTOR3 normalCameraDirection = GetNormalDirectionVector();

	D3DXVec3Cross(&normalCameraRight, &normalCameraDirection, &normalCameraUp);
	return normalCameraRight;
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = mViewMatrix;
	return;
}

void CameraClass::RenderReflection(float height)
{
	D3DXVECTOR3 up, position, lookAt;
	float radians;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	// For planar reflection invert the Y position of the camera.
	position.x = mPositionX;
	position.y = -mPositionY + (height * 2.0f);
	position.z = mPositionZ;

	// Calculate the rotation in radians.
	radians = mRotationY * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + mPositionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + mPositionZ;

	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&m_reflectionViewMatrix, &position, &lookAt, &up);

	return;
}

D3DXMATRIX CameraClass::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
}

