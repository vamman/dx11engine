#pragma once
#include <vector>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

class SkyDome
{
	public:
		SkyDome(void);
		~SkyDome(void);
		HRESULT CreateSphere(ID3D11Device* device, int LatLines, int LongLines);
		HRESULT InitializeSkyDome(ID3D11Device* device);
		void UpdateSkyDome(D3DXVECTOR3 cameraPosition);
		void RenderSkyDome(ID3D11DeviceContext* deviceContext, D3DXMATRIX view, D3DXMATRIX projection);

	private:
		struct Vertex	//Overloaded Vertex Structure
		{
			Vertex(){}
			Vertex(float x, float y, float z,
				float u, float v,
				float nx, float ny, float nz)
				: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

			D3DXVECTOR3 pos;
			D3DXVECTOR2 texCoord;
			D3DXVECTOR3 normal;
		};

		//Create effects constant buffer's structure//
		struct cbPerObject
		{
			D3DXMATRIX  WVP;
			D3DXMATRIX World;
		};

		cbPerObject cbPerObj;

		ID3D11Buffer* sphereIndexBuffer;
		ID3D11Buffer* sphereVertBuffer;

		ID3D11VertexShader* SKYMAP_VS;
		ID3D11PixelShader* SKYMAP_PS;
		ID3D10Blob* SKYMAP_VS_Buffer;
		ID3D10Blob* SKYMAP_PS_Buffer;

		ID3D11ShaderResourceView* smrv;

		ID3D11DepthStencilState* DSLessEqual;

		ID3D11RasterizerState* CCWcullMode;
		ID3D11RasterizerState* CWcullMode;
		ID3D11RasterizerState* RSCullNone;

		int NumSphereVertices;
		int NumSphereFaces;

		D3DXMATRIX Rotationx;
		D3DXMATRIX Rotationy;
		D3DXMATRIX Rotationz;
		D3DXMATRIX sphereWorld;

		ID3D11Buffer* cbPerObjectBuffer;
		ID3D11SamplerState* CubesTexSamplerState;
		//ID3D11VertexShader* VS;
};
