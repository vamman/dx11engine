// TODO: Fit sky dome shader into existing architecture

#pragma once
#include "BasicShader.h"

class SkyDomeShader : public BasicShader
{
	private:
		struct GradientBufferType
		{
			D3DXVECTOR4 apexColor;
			D3DXVECTOR4 centerColor;
			D3DXVECTOR4 pixelShaderType;
		};

	public:
		SkyDomeShader(void);
		~SkyDomeShader(void);
		virtual HRESULT Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		virtual void Shutdown();
		bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
					D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor, float pixelShaderMode);

	private:
		HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
								 D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor, float pixelShaderMode);
		void RenderShader(ID3D11DeviceContext*, int);

	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11Buffer* m_gradientBuffer;
};
