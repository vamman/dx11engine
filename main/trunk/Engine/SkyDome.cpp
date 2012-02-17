#include "SkyDome.h"

SkyDome::SkyDome(void) : NumSphereVertices(0), NumSphereFaces(0)
{
	sphereIndexBuffer = 0;
	sphereVertBuffer = 0;
	smrv = 0;
}

SkyDome::~SkyDome(void)
{
}

HRESULT SkyDome::CreateSphere(ID3D11Device* device, int LatLines, int LongLines)
{
	HRESULT result = S_OK;
	NumSphereVertices = ((LatLines-2) * LongLines) + 2;
	NumSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<Vertex> vertices(NumSphereVertices);

	D3DXVECTOR3 currVertPos = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f;

	for(int i = 0; i < LatLines-2; ++i)
	{
		spherePitch = ( (float) i + 1.0f) * (3.14f / ( (float) LatLines - 1.0f) );

		D3DXMatrixRotationX(&Rotationx, spherePitch);
			// Rotationx = XMMatrixRotationX(spherePitch);

		for(int j = 0; j < LongLines; ++j)
		{
			sphereYaw = (float) j * (6.28f / (float)LongLines);

			D3DXMatrixRotationX(&Rotationy, sphereYaw);
				// Rotationy = XMMatrixRotationZ(sphereYaw);

			D3DXVec3TransformNormal(&currVertPos, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), & (Rotationx * Rotationy) );
				// currVertPos = XMVector3TransformNormal( XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy) );
			
			D3DXVec3Normalize(&currVertPos, &currVertPos);
				// currVertPos = XMVector3Normalize( currVertPos );

			vertices[i*LongLines+j+1].pos.x = currVertPos.x; // XMVectorGetX(currVertPos);
			vertices[i*LongLines+j+1].pos.y = currVertPos.y; // XMVectorGetY(currVertPos);
			vertices[i*LongLines+j+1].pos.z = currVertPos.z; // XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices-1].pos.x =  0.0f;
	vertices[NumSphereVertices-1].pos.y =  0.0f;
	vertices[NumSphereVertices-1].pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &vertices[0];
	result = device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);
	if (FAILED(result))
	{
		return result;
	}

	std::vector<DWORD> indices(NumSphereFaces * 3);

	int k = 0;
	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = 0;
		indices[k+1] = l+1;
		indices[k+2] = l+2;
		k += 3;
	}

	indices[k] = 0;
	indices[k+1] = LongLines;
	indices[k+2] = 1;
	k += 3;

	for(int i = 0; i < LatLines-3; ++i)
	{
		for(int j = 0; j < LongLines-1; ++j)
		{
			indices[k]   = i*LongLines+j+1;
			indices[k+1] = i*LongLines+j+2;
			indices[k+2] = (i+1)*LongLines+j+1;

			indices[k+3] = (i+1)*LongLines+j+1;
			indices[k+4] = i*LongLines+j+2;
			indices[k+5] = (i+1)*LongLines+j+2;

			k += 6; // next quad
		}

		indices[k]   = (i*LongLines)+LongLines;
		indices[k+1] = (i*LongLines)+1;
		indices[k+2] = ((i+1)*LongLines)+LongLines;

		indices[k+3] = ((i+1)*LongLines)+LongLines;
		indices[k+4] = (i*LongLines)+1;
		indices[k+5] = ((i+1)*LongLines)+1;

		k += 6;
	}

	for(int l = 0; l < LongLines-1; ++l)
	{
		indices[k] = NumSphereVertices-1;
		indices[k+1] = (NumSphereVertices-1)-(l+1);
		indices[k+2] = (NumSphereVertices-1)-(l+2);
		k += 3;
	}

	indices[k] = NumSphereVertices-1;
	indices[k+1] = (NumSphereVertices-1)-LongLines;
	indices[k+2] = NumSphereVertices-2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];

	result = device->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);
	if (FAILED(result))
	{
		return result;
	}
	return result;
}

HRESULT SkyDome::InitializeSkyDome(ID3D11Device* device)
{
	HRESULT result = S_OK;

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;	
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	result = device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	result = D3DX11CompileFromFile(L"Engine/data/shaders/SkyDomeShader.fx", 0, 0, "SKYMAP_VS", "vs_4_0", 0, 0, 0, &SKYMAP_VS_Buffer, 0, 0);
	if (FAILED(result))
	{
		return result;
	}
	result = D3DX11CompileFromFile(L"Engine/data/shaders/SkyDomeShader.fx", 0, 0, "SKYMAP_PS", "ps_4_0", 0, 0, 0, &SKYMAP_PS_Buffer, 0, 0);
	if (FAILED(result))
	{
		return result;
	}

	result = device->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(), SKYMAP_VS_Buffer->GetBufferSize(), NULL, &SKYMAP_VS);
	if (FAILED(result))
	{
		return result;
	}
	result = device->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(), SKYMAP_PS_Buffer->GetBufferSize(), NULL, &SKYMAP_PS);
	if (FAILED(result))
	{
		return result;
	}

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

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	result = device->CreateRasterizerState(&cmdesc, &CCWcullMode);
	if (FAILED(result))
	{
		return result;
	}

	cmdesc.FrontCounterClockwise = false;

	result = device->CreateRasterizerState(&cmdesc, &CWcullMode);
	if (FAILED(result))
	{
		return result;
	}

	cmdesc.CullMode = D3D11_CULL_NONE;
	result = device->CreateRasterizerState(&cmdesc, &RSCullNone);
	if (FAILED(result))
	{
		return result;
	}

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	return result;
}

void SkyDome::UpdateSkyDome(D3DXVECTOR3 cameraPosition)
{
	D3DXMATRIX scale, translation;

	//Reset sphereWorld
		// sphereWorld = XMMatrixIdentity();
	D3DXMatrixIdentity(&sphereWorld);

	//Define sphereWorld's world space matrix
		// Scale = XMMatrixScaling( 5.0f, 5.0f, 5.0f );
	D3DXMatrixScaling(&scale, 5.0f, 5.0f, 5.0f);
	

	//Make sure the sphere is always centered around camera
		// Translation = XMMatrixTranslation( XMVectorGetX(camPosition), XMVectorGetY(camPosition), XMVectorGetZ(camPosition) );
	D3DXMatrixTranslation(&translation, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	//Set sphereWorld's world space using the transformations
	D3DXMatrixMultiply(&sphereWorld, &scale, &translation);
	//sphereWorld = scale * translation;
}

void SkyDome::RenderSkyDome(ID3D11DeviceContext* deviceContext, D3DXMATRIX view, D3DXMATRIX projection)
{
	UINT stride = sizeof( Vertex );
	UINT offset = 0;

	D3DXMATRIX WVP, viewProj;
	deviceContext->IASetIndexBuffer( sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetVertexBuffers( 0, 1, &sphereVertBuffer, &stride, &offset );

	
	// WVP = sphereWorld * view * projection;

	D3DXMatrixMultiply(&viewProj, &view, &projection);
	D3DXMatrixMultiply(&WVP, &sphereWorld, &viewProj);

		// cbPerObj.WVP = XMMatrixTranspose(WVP);
	D3DXMatrixTranspose(&cbPerObj.WVP, &WVP);

		// cbPerObj.World = XMMatrixTranspose(sphereWorld);
	D3DXMatrixTranspose(&cbPerObj.World, &sphereWorld);

	deviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	deviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	deviceContext->PSSetShaderResources( 0, 1, &smrv );
	deviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	deviceContext->VSSetShader(SKYMAP_VS, 0, 0);
	deviceContext->PSSetShader(SKYMAP_PS, 0, 0);
	deviceContext->OMSetDepthStencilState(DSLessEqual, 0);
	deviceContext->RSSetState(RSCullNone);
	deviceContext->DrawIndexed( NumSphereFaces * 3, 0, 0 );

	//deviceContext->VSSetShader(VS, 0, 0);
	//deviceContext->OMSetDepthStencilState(NULL, 0);
}

