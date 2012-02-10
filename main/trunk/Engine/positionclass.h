////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: PositionClass
////////////////////////////////////////////////////////////////////////////////
class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetFrameTime(float);
	void GetRotation(float&);
	void GetTransformation(float&);

	void TurnLeft(bool);
	void TurnRight(bool);
	void WalkForward(bool);
	void WalkBack(bool);

private:

	float UpdatePositioning(bool keydown, float state, float acceleration, float maxSpeed);

	float m_frameTime;
	float m_rotationY;
	float m_Transformation;
	float m_leftTurnSpeed, m_rightTurnSpeed, m_walkSpeed;
};

#endif