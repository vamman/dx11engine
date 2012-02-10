////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "positionclass.h"

float ROTATION_SPEED = 0.15f;
float WALK_SPEED = 0.015f;

float ROTATION_ACCELERATION = 0.15f;
float WALK_ACCELERATION = 0.015f;

PositionClass::PositionClass()
{
	m_frameTime = 0.0f;
	m_rotationY = 0.0f;
	m_leftTurnSpeed  = 0.0f;
	m_rightTurnSpeed = 0.0f;
	m_Transformation = 0.0f;
	m_walkSpeed = 0.0f;
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
	return;
}

void PositionClass::GetRotation(float& y)
{
	y = m_rotationY;
	return;
}

void PositionClass::TurnLeft(bool keydown)
{
	m_leftTurnSpeed = UpdatePositioning(keydown, m_leftTurnSpeed, ROTATION_ACCELERATION, ROTATION_SPEED);

	m_rotationY -= m_leftTurnSpeed;
	if(m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}

	return;
}


void PositionClass::TurnRight(bool keydown)
{
	m_rightTurnSpeed = UpdatePositioning(keydown, m_rightTurnSpeed, ROTATION_ACCELERATION, ROTATION_SPEED);

	m_rotationY += m_rightTurnSpeed;
	if(m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

	return;
}

void PositionClass::WalkForward(bool keydown)
{
	m_walkSpeed = UpdatePositioning(keydown, m_walkSpeed, WALK_ACCELERATION, WALK_SPEED);
	m_Transformation += m_walkSpeed;
}

void PositionClass::WalkBack(bool keydown)
{
	m_walkSpeed = UpdatePositioning(keydown, m_walkSpeed, WALK_ACCELERATION, WALK_SPEED);
	m_Transformation -= m_walkSpeed;
}

void PositionClass::GetTransformation(float& transformation)
{
	transformation = m_Transformation;
}

float PositionClass::UpdatePositioning(bool keydown, float state, float acceleration, float maxSpeed)
{
	if(keydown)
	{
		state += m_frameTime * acceleration;

		if(state > (m_frameTime * maxSpeed))
		{
			state = m_frameTime * maxSpeed;
		}
	}
	else
	{
		state -= m_frameTime* acceleration * 5;

		if(state < 0.0f)
		{
			state = 0.0f;
		}
	}
	return state;
}
