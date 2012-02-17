// TODO: Fit sky dome shader into existing architecture

#pragma once
#include "LightShader.h"

class SkyDomeShader : public LightShader
{
	public:
		SkyDomeShader(void);
		~SkyDomeShader(void);

		virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		HRESULT InitializeShader(LightClass* lightSource, ID3D11Device*, HWND, WCHAR*, LPCSTR, LPCSTR, vector<char *>& layouts);
		virtual void Shutdown();

	private:
		ID3D11Buffer* cbPerObjectBuffer;
};
