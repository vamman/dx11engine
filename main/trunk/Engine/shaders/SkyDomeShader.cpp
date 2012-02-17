// TODO: Fit sky dome shader into existing architecture

#include "SkyDomeShader.h"

SkyDomeShader::SkyDomeShader(void)
{
	cbPerObjectBuffer = 0;
}

SkyDomeShader::~SkyDomeShader(void)
{
}

HRESULT SkyDomeShader::Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname)
{
	HRESULT result = S_OK;
	result = LightShader::Initialize(lightSource, device, hwnd, filename, VSname, PSname);
	return result;
}

HRESULT SkyDomeShader::InitializeShader(LightClass* lightSource, ID3D11Device*, HWND, WCHAR*, LPCSTR, LPCSTR, vector<char *>& layouts)
{
	HRESULT result = S_OK;
	return result;
}

void SkyDomeShader::Shutdown()
{

}
