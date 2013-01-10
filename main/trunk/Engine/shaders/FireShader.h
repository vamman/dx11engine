#pragma once
#include "TextureShader.h"
#include "..\ModelObject.h"
#include "..\d3dclass.h"
#include "..\cameraclass.h"

class FireShader : public TextureShader
{
	private:
		struct NoiseBufferType
		{
			float frameTime;
			D3DXVECTOR3 scrollSpeeds;
			D3DXVECTOR3 scales;
			float padding;
		};

		struct DistortionBufferType
		{
			D3DXVECTOR2 distortion1;
			D3DXVECTOR2 distortion2;
			D3DXVECTOR2 distortion3;
			float distortionScale;
			float distortionBias;
		};

	public:
		FireShader(void);
		virtual ~FireShader(void);

		virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		HRESULT InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* FXfilename,
							  LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		virtual void Shutdown();

		virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
			D3DXMATRIX projectionMatrix, float frameTime, D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, D3DXVECTOR2 distortion1, 
			D3DXVECTOR2 distortion2, D3DXVECTOR2 distortion3, float distortionScale, 
			float distortionBias, bool isInstanced);
		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);

		virtual HRESULT RenderInstanced(ID3D11DeviceContext* deviceContext,
										int vertexCount,
										int instanceCount,
										D3DXMATRIX worldMatrix, 
										D3DXMATRIX viewMatrix, 
										D3DXMATRIX projectionMatrix);

		virtual HRESULT RenderOrdinary( ID3D11DeviceContext* deviceContext,
										ModelObject* modelObj, CameraClass* activeCamera, float frameTime,
										D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, 
										D3DXVECTOR2 distortion1, D3DXVECTOR2 distortion2,
										D3DXVECTOR2 distortion3, float distortionScale, float distortionBias);

	private:
		ID3D11Buffer* m_noiseBuffer;
		ID3D11SamplerState* m_sampleState2;
		ID3D11Buffer* m_distortionBuffer;
};

