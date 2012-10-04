#ifndef _BASICSHADERCLASS_H_
#define _BASICSHADERCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>
#include "../lightclass.h"

using namespace std;

class BaseShaderClass
{
	public:
		BaseShaderClass(void);
		~BaseShaderClass(void);
		virtual HRESULT RenderInstanced(ID3D11DeviceContext* deviceContext,
			int vertexCount,
			int instanceCount,
			D3DXMATRIX worldMatrix, 
			D3DXMATRIX viewMatrix,
			D3DXMATRIX projectionMatrix) const = 0;

		virtual HRESULT RenderOrdinary(ID3D11DeviceContext* deviceContext, 
			int indexCount,
			D3DXMATRIX worldMatrix, 
			D3DXMATRIX viewMatrix,
			D3DXMATRIX projectionMatrix) const = 0;

		virtual void SetTextureArray(ID3D11DeviceContext* deviceContext, vector<ID3D11ShaderResourceView*>& textureArray) = 0;
		virtual HRESULT SetCameraPosition(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition, int lightType) = 0;
		virtual bool SetLightSource(ID3D11DeviceContext* deviceContext, LightClass* lightSource) = 0;

		virtual HRESULT SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
			D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool isInstanced) const = 0;
};

#endif
