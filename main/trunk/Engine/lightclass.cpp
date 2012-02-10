////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass() : mLightType(0), m_specularColor(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)), m_specularPower(0), m_position(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f))
{
}


LightClass::LightClass(const LightClass& other) : mLightType(0), m_specularColor(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f)), m_specularPower(0), m_position(D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f))
{
}


LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}

D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR3 LightClass::GetDirection()
{
	return m_direction;
}

D3DXVECTOR4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}

float LightClass::GetSpecularPower()
{
	return m_specularPower;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = D3DXVECTOR4(x, y, z, 1.0f);
	return;
}

D3DXVECTOR4 LightClass::GetPosition()
{
	return m_position;
}

void LightClass::SetLightType(int lightType)
{
	mLightType = lightType;
}

int LightClass::GetLightType()
{
	return mLightType;
}