////////////////////////////////////////////////////////////////////////////////
// Filename: CameraMovement.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>
#include <math.h>
#include "Log.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraMovement
////////////////////////////////////////////////////////////////////////////////
class CameraMovement
{
public:
	CameraMovement();
	CameraMovement(const CameraMovement&);
	~CameraMovement();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void SetFrameTime(float);

	void MoveForward(bool keyDown, D3DXVECTOR3 normalCameraDirection);
	void MoveBackward(bool keyDown, D3DXVECTOR3 normalCameraDirection);
	void StrafeLeft(bool keyDown, D3DXVECTOR3 normalCameraRightVector);
	void StrafeRight(bool keyDown, D3DXVECTOR3 normalCameraRightVector);

	//void MoveUpward(bool);
	//void MoveDownward(bool);

	void TurnLeft(bool);
	void TurnRight(bool);

	void MouseMoveHorizontal(int mouseDeltaX);
	void MouseMoveVertical(int mouseDeltaY);

	void LookUpward(bool);
	void LookDownward(bool);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	float m_frameTime;

	float mForwardSpeed, mBackwardSpeed, mStrafeLeftSpeed, mStrafeRightSpeed;
	float m_upwardSpeed, m_downwardSpeed;
	float m_leftTurnSpeed, m_rightTurnSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;
};

#endif