#pragma once
#include <vector>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include "d3dclass.h"

class SkyDome
{
	public:
		SkyDome(void);
		~SkyDome(void);
		HRESULT CreateCube(ID3D11Device* device);
		HRESULT CreateSphere(ID3D11Device* device, int LatLines, int LongLines);
		HRESULT InitializeSkyDome(D3DClass* d3d);
		void UpdateSkyDome(D3DXVECTOR3 cameraPosition);
		HRESULT SetFillMode(ID3D11Device* device, D3D11_FILL_MODE fillMode);
		void RenderSkyDome(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX view, D3DXMATRIX projection, int shapeType);

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
			// D3DXMATRIX  WVP;
			D3DXMATRIX worldMatrix;
			D3DXMATRIX viewMatrix;
			D3DXMATRIX projectionMatix;
		};

		cbPerObject cbPerObj;

		ID3D11Buffer *sphereVertBuffer, *sphereIndexBuffer, *cubeVertBuffer, *cubeIndexBuffer;

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
		ID3D11InputLayout* vertLayout;
		//ID3D11VertexShader* VS;
};
