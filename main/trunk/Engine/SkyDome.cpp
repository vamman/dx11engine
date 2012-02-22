#include "SkyDome.h"

SkyDome::SkyDome(void)
{
	mModelFactory = 0;
	mSkyDomeObject = 0;
}

SkyDome::~SkyDome(void)
{

}

bool SkyDome::Initialize(ID3D11Device* device, HWND hwnd)
{
	mModelFactory = new ModelFactory;
	mSkyDomeObject = new ModelObject;

	// Load in the sky dome model.
	mSkyDomeObject = mModelFactory->CreateOrdinaryModel(device, hwnd, "skyDome", "Engine/data/models/sphere.txt");

	// Set the color at the top of the sky dome.
	m_apexColor = D3DXVECTOR4(0.15f, 0.26f, 0.78f, 1.0f); // 0.0f, 0.15f, 0.66f, 1.0f

	// Set the color at the center of the sky dome.
	m_centerColor =  D3DXVECTOR4(0.62f, 0.6f, 0.91f, 1.0f); // 0.81f, 0.38f, 0.66f, 1.0f

	return true;
}

void SkyDome::Shutdown()
{
	// Release the sky dome model.
	ReleaseSkyDomeModel();
	return;
}

void SkyDome::Render(ID3D11DeviceContext* deviceContext)
{
	// Render the sky dome.
	mSkyDomeObject->GetModel()->RenderOrdinary(deviceContext);
	return;
}

int SkyDome::GetIndexCount()
{
	return mSkyDomeObject->GetModel()->GetIndexCount();
}

D3DXVECTOR4 SkyDome::GetApexColor()
{
	return m_apexColor;
}

D3DXVECTOR4 SkyDome::GetCenterColor()
{
	return m_centerColor;
}

void SkyDome::ReleaseSkyDomeModel()
{
	mSkyDomeObject->GetModel()->Shutdown();
	return;
}

HRESULT SkyDome::SetFillMode(D3DClass* d3d, D3D11_FILL_MODE fillMode)
{
	HRESULT result = S_OK;
	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState* m_rasterStateNoCulling = 0;

	// Setup a raster description which turns off back face culling.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = fillMode;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the no culling rasterizer state.
	result = d3d->GetDevice()->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCulling);
	if(FAILED(result))
	{
		return result;
	}

	d3d->GetDeviceContext()->RSSetState(m_rasterStateNoCulling);
	return result;
}



