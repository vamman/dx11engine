////////////////////////////////////////////////////////////////////////////////
// Filename: CameraMovement.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CameraMovement.h"

CameraMovement::CameraMovement()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_frameTime = 0.0f;

	mForwardSpeed   = 0.0f;
	mBackwardSpeed  = 0.0f;
	mStrafeLeftSpeed  = 0.0f;
	mStrafeRightSpeed = 0.0f;
	m_upwardSpeed    = 0.0f;
	m_downwardSpeed  = 0.0f;
	m_leftTurnSpeed  = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_lookUpSpeed    = 0.0f;
	m_lookDownSpeed  = 0.0f;
}

CameraMovement::CameraMovement(const CameraMovement& other)
{
}

CameraMovement::~CameraMovement()
{
}

void CameraMovement::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraMovement::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

void CameraMovement::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
	return;
}


void CameraMovement::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
	return;
}

void CameraMovement::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}

void CameraMovement::MoveForward(bool keydown, D3DXVECTOR3 normalCameraDirection)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		mForwardSpeed += m_frameTime * 0.001f;

		if(mForwardSpeed > (m_frameTime * 0.03f))
		{
			mForwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		mForwardSpeed -= m_frameTime * 0.0007f;

		if(mForwardSpeed < 0.0f)
		{
			mForwardSpeed = 0.0f;
		}
	}

	if (mForwardSpeed > 0.0f )
	{
		m_positionX += normalCameraDirection.x * mForwardSpeed;
		m_positionZ += normalCameraDirection.z * mForwardSpeed;
	}
	return;
}

void CameraMovement::MoveBackward(bool keydown, D3DXVECTOR3 normalCameraDirection)
{
	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		mBackwardSpeed += m_frameTime * 0.001f;

		if(mBackwardSpeed > (m_frameTime * 0.03f))
		{
			mBackwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		mBackwardSpeed -= m_frameTime * 0.0007f;

		if(mBackwardSpeed < 0.0f)
		{
			mBackwardSpeed = 0.0f;
		}
	}

	if (mBackwardSpeed > 0.0f )
	{
		m_positionX -= normalCameraDirection.x * mBackwardSpeed;
		m_positionZ -= normalCameraDirection.z * mBackwardSpeed;
	}
	return;
}

void CameraMovement::StrafeLeft(bool keydown, D3DXVECTOR3 normalCameraRightVector)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		mStrafeLeftSpeed += m_frameTime * 0.001f;

		if(mStrafeLeftSpeed > (m_frameTime * 0.03f))
		{
			mStrafeLeftSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		mStrafeLeftSpeed -= m_frameTime * 0.0007f;

		if(mStrafeLeftSpeed < 0.0f)
		{
			mStrafeLeftSpeed = 0.0f;
		}
	}

	if (mStrafeLeftSpeed > 0.0f )
	{
		m_positionX += normalCameraRightVector.x * mStrafeLeftSpeed;
		m_positionZ += normalCameraRightVector.z * mStrafeLeftSpeed;
	}

	return;
}

void CameraMovement::StrafeRight(bool keydown, D3DXVECTOR3 normalCameraRightVector)
{
	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		mStrafeRightSpeed += m_frameTime * 0.001f;

		if(mStrafeRightSpeed > (m_frameTime * 0.03f))
		{
			mStrafeRightSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		mStrafeRightSpeed -= m_frameTime * 0.0007f;

		if(mStrafeRightSpeed < 0.0f)
		{
			mStrafeRightSpeed = 0.0f;
		}
	}

	if (mStrafeRightSpeed > 0.0f )
	{
		m_positionX -= normalCameraRightVector.x * mStrafeRightSpeed;
		m_positionZ -= normalCameraRightVector.z * mStrafeRightSpeed;
	}

	return;
}

void CameraMovement::MouseMoveHorizontal(int mouseDeltaX)
{
	float rotationSpeed = 0.03f;
	if(abs(mouseDeltaX) > 0 && abs(mouseDeltaX) < 300)
	{
		m_rotationY += rotationSpeed * mouseDeltaX * m_frameTime;
	}

	// Keep the rotation in the 0 to 360 range.
	if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}
	else if(m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}
}

void CameraMovement::MouseMoveVertical(int mouseDeltaY)
{
	float rotationSpeed = 0.03f;
	if(abs(mouseDeltaY) > 0 && abs(mouseDeltaY) < 300)
	{
		m_rotationX += rotationSpeed * mouseDeltaY * m_frameTime;
	}

	// Keep the rotation in the 0 to 360 range.
	if(m_rotationX > 90.0f)
	{
		m_rotationX += 360.0f;
	}
	else if(m_rotationX < -90.0f)
	{
		m_rotationX -= 360.0f;
	}
}

void CameraMovement::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * 0.01f;

		if(m_lookUpSpeed > (m_frameTime * 0.15f))
		{
			m_lookUpSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime* 0.005f;

		if(m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotationX > 90.0f)
	{
		m_rotationX = 90.0f;
	}

	return;
}

void CameraMovement::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * 0.01f;

		if(m_lookDownSpeed > (m_frameTime * 0.15f))
		{
			m_lookDownSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime* 0.005f;

		if(m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_rotationX += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_rotationX < -90.0f)
	{
		m_rotationX = -90.0f;
	}

	return;
}