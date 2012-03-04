#include "SkyDome.h"

SkyDome::SkyDome(void)
{
	mModelFactory = 0;
	mSkyDomeSphere = 0;
	mSkyDomeCube = 0;
	smrv = 0;
}

SkyDome::~SkyDome(void)
{

}

bool SkyDome::Initialize(ID3D11Device* device, HWND hwnd)
{
	// mModelFactory = new ModelFactory;

	mModelFactory =  ModelFactory::GetInstance();
	mSkyDomeSphere = new ModelObject;

	// Load in the sky dome model.
	mSkyDomeSphere = mModelFactory->CreateOrdinaryModel(device, hwnd, "skyDomeSphere", wstring(L"Engine/data/models/sphere.txt"));
	mSkyDomeCube = mModelFactory->CreateOrdinaryModel(device, hwnd, "skyDomeCube", wstring(L"Engine/data/models/cube.txt"));

	// Set the color at the top of the sky dome.
	m_apexColor = D3DXVECTOR4(0.15f, 0.26f, 0.78f, 1.0f); // 0.0f, 0.15f, 0.66f, 1.0f

	// Set the color at the center of the sky dome.
	m_centerColor =  D3DXVECTOR4(0.62f, 0.6f, 0.91f, 1.0f); // 0.81f, 0.38f, 0.66f, 1.0f

	return true;
}

HRESULT SkyDome::CreateCubeTexture(ID3D11Device* device)
{
	HRESULT result = S_OK;
	//Tell D3D we will be loading a cube texture
	D3DX11_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	//Load the texture
	ID3D11Texture2D* SMTexture = 0;
	result = D3DX11CreateTextureFromFile(device, L"Engine/data/textures/CubeMapAutumn.dds", &loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);
	if (FAILED(result))
	{
		return result;
	}

	//Create the textures description
	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	//Tell D3D We have a cube texture, which is an array of 2D textures
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	//Create the Resource view
	result = device->CreateShaderResourceView(SMTexture, &SMViewDesc, &smrv);
	if (FAILED(result))
	{
		return result;
	}

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	result = device->CreateSamplerState( &sampDesc, &CubesTexSamplerState );
	if (FAILED(result))
	{
		return result;
	}
	return result;
}

void SkyDome::Shutdown()
{
	// Release the sky dome model.
	ReleaseSkyDomeModel();
	return;
}

void SkyDome::Render(ID3D11DeviceContext* deviceContext, int shapeType)
{
	// Render the sky dome.
	deviceContext->PSSetShaderResources( 0, 1, &smrv );
	deviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState);
	if (shapeType == 0)
	{
		mSkyDomeSphere->GetModel()->RenderOrdinary(deviceContext);
	}
	else
	{
		mSkyDomeCube->GetModel()->RenderOrdinary(deviceContext);
	}
	return;
}

int SkyDome::GetIndexCount(int shapeType)
{
	int indexCount = shapeType == 0 ? mSkyDomeSphere->GetModel()->GetIndexCount() : mSkyDomeCube->GetModel()->GetIndexCount();
	return indexCount;
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
//	mSkyDomeSphere->GetModel()->Shutdown();
//	mSkyDomeCube->GetModel()->Shutdown();
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



