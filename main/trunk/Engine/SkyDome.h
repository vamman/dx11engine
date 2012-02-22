#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "ModelFactory.h"
#include "d3dclass.h"

using namespace std;

class SkyDome
	{
	private:
		struct VertexType
		{
			D3DXVECTOR3 position;
		};

	public:
		SkyDome();
		SkyDome(const SkyDome&);
		~SkyDome();

		bool Initialize(ID3D11Device* device, HWND hwnd);
		void Shutdown();
		void Render(ID3D11DeviceContext*);
		int GetIndexCount();
		D3DXVECTOR4 GetApexColor();
		D3DXVECTOR4 GetCenterColor();
		HRESULT SetFillMode(D3DClass* d3d, D3D11_FILL_MODE fillMode);

	private:
		void ReleaseSkyDomeModel();

	private:
		ModelFactory* mModelFactory;
		ModelObject* mSkyDomeObject;
		D3DXVECTOR4 m_apexColor, m_centerColor;
};
