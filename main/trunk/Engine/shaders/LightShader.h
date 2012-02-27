#pragma once
#include "TextureShader.h"
#include "../lightclass.h"
const int NUM_LIGHTS = 4; // 4 point lights

class LightShader : public TextureShader
{
	public:
		enum LightTypes
		{
			DIRECTIONAL_AMBIENT_LIGHT = 0,
			DIRECTIONAL_SPECULAR_LIGHT,
			POINT_LIGHT
		};

		LightShader(void);
		~LightShader(void);

		virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		HRESULT InitializeShader(LightClass* lightSource, ID3D11Device*, HWND, WCHAR*, LPCSTR, LPCSTR, vector<char *>& layouts);
		virtual void Shutdown();

		virtual HRESULT SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType);
		virtual bool SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource);
		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray);
		virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool isInstanced);

		virtual HRESULT RenderInstanced(ID3D11DeviceContext* deviceContext,
									 int vertexCount,
									 int instanceCount,
									 D3DXMATRIX worldMatrix,
									 D3DXMATRIX viewMatrix,
									 D3DXMATRIX projectionMatrix);

		virtual HRESULT RenderOrdinary(ID3D11DeviceContext* deviceContext, 
							int indexCount,
							D3DXMATRIX worldMatrix,
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix);

	private:
		HRESULT InitializeDirectionalLightShader(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* FXfilename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);
		HRESULT InitializePointLightShader(ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname, vector<char *>& layouts);

		bool SetDirLightShaderParams(ID3D11DeviceContext* deviceContext, 
			D3DXVECTOR3 cameraPosition,
			LightClass* lightSource
			);
		bool SetPointLightShaderParameters(ID3D11DeviceContext* deviceContext, 
			D3DXVECTOR4 pointDiffuseColors[],
			D3DXVECTOR4 pointLightPositions[]
		);
	private:

		struct CameraBufferType
		{
			D3DXVECTOR3 cameraPosition;
			float padding;
		};

		struct SpecLightBufferType
		{
			D3DXVECTOR4 ambientColor;
			D3DXVECTOR4 diffuseColor;
			D3DXVECTOR4 specularColor;
			float specularPower;
			D3DXVECTOR3 lightDirection;
		};

		struct AmbLightBufferType
		{
			D3DXVECTOR4 ambientColor;
			D3DXVECTOR4 diffuseColor;
			D3DXVECTOR3 lightDirection;
			float padding;
		};

		ID3D11Buffer* m_lightBuffer;
		ID3D11Buffer* m_cameraBuffer;

		// For point lights
		struct LightColorBufferType
		{
			D3DXVECTOR4 diffuseColor[NUM_LIGHTS];
		};

		struct LightPositionBufferType
		{
			D3DXVECTOR4 lightPosition[NUM_LIGHTS];
		};

		ID3D11Buffer* m_lightColorBuffer;
		ID3D11Buffer* m_lightPositionBuffer;
		int mLightType;
};

