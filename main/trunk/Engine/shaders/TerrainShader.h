#pragma once
#include "lightshader.h"
class TerrainShader : public LightShader
{
	struct TextureInfoBufferType
	{
		bool useAlplha;
		D3DXVECTOR3 padding2;
	};

	public:
		TerrainShader(void);
		~TerrainShader(void);
		virtual HRESULT Initialize(LightClass* lightSource, ID3D11Device* device, HWND hwnd, WCHAR* filename, LPCSTR VSname, LPCSTR PSname);
		virtual void Shutdown();

		virtual HRESULT SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType);
		virtual bool SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource);

		virtual bool SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray, bool useAlpha);

		virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,  D3DXMATRIX projectionMatrix, bool isInstanced);
		void RenderShader(ID3D11DeviceContext*, int);

	private:
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_layout;
		ID3D11Buffer* m_textureInfoBuffer;
};

