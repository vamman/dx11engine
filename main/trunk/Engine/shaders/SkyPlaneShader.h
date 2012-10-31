#ifndef _SKYPLANESHADER_H_
#define _SKYPLANESHADER_H_

#include "BasicShader.h"
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

class SkyPlaneShader : public BasicShader
{
	private:
		struct SkyBufferType
		{
			float translation;
			float scale;
			float brightness;
			float padding;
		};

	public:
		SkyPlaneShader(void);
		virtual ~SkyPlaneShader(void);
		HRESULT Initialize(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		void Shutdown();
		HRESULT Render(ID3D11DeviceContext* deviceContext,
				    int indexCount,
				    D3DXMATRIX worldMatrix, 
				    D3DXMATRIX viewMatrix, 
				    D3DXMATRIX projectionMatrix);
		void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);
		HRESULT SetSkyBuffer(ID3D11DeviceContext* deviceContext, float translation, float scale,
			float brightness);

	private:
		HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, 
							  vector<char *>& layouts);
		void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

		HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
								 D3DXMATRIX projectionMatrix);

	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_layout;
		ID3D11SamplerState* m_sampleState;
		ID3D11Buffer* m_skyBuffer;
};

#endif

