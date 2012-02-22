#include "SkyDomeBrayn.h"

SkyDomeBrayn::SkyDomeBrayn(void) : NumSphereVertices(0), NumSphereFaces(0)
{
	sphereVertBuffer = 0;
	sphereIndexBuffer = 0;
	cubeVertBuffer = 0;
	cubeIndexBuffer = 0;
	smrv = 0;
}

SkyDomeBrayn::~SkyDomeBrayn(void)
{
}

HRESULT SkyDomeBrayn::CreateCube(ID3D11Device* device)
{
	HRESULT result = S_OK;
	NumSphereVertices = 8;
	Vertex* vertices;
	vertices = new Vertex[NumSphereVertices];

	vertices[0].pos.x = -0.5f;
	vertices[0].pos.y = -0.5f;
	vertices[0].pos.z = -0.5f;

	vertices[1].pos.x = -0.5f;
	vertices[1].pos.y = 0.5f;
	vertices[1].pos.z = -0.5f;

	vertices[2].pos.x = 0.5f;
	vertices[2].pos.y = 0.5f;
	vertices[2].pos.z = -0.5f;

	vertices[3].pos.x = 0.5f;
	vertices[3].pos.y = -0.5f;
	vertices[3].pos.z = -0.5f;

	vertices[4].pos.x = -0.5f;
	vertices[4].pos.y = -0.5f;
	vertices[4].pos.z = 0.5f;

	vertices[5].pos.x = -0.5f;
	vertices[5].pos.y = 0.5f;
	vertices[5].pos.z = 0.5f;

	vertices[6].pos.x = 0.5f;
	vertices[6].pos.y = 0.5f;
	vertices[6].pos.z = 0.5f;

	vertices[7].pos.x = 0.5f;
	vertices[7].pos.y = -0.5f;
	vertices[7].pos.z = 0.5f;

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = vertices;
	result = device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &cubeVertBuffer);
	if (FAILED(result))
	{
		return result;
	}
	
	unsigned long indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		4, 5, 1,
		4, 1, 0,
		// right face
		3, 2, 6,
		3, 6, 7,
		// top face
		1, 5, 6,
		1, 6, 2,
		// bottom face
		4, 0, 3, 
		4, 3, 7
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 36;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;

	result = device->CreateBuffer(&indexBufferDesc, &iinitData, &cubeIndexBuffer); 
	if (FAILED(result))
	{
		return result;
	}

	return result;
}

HRESULT SkyDomeBrayn::CreateSphere(ID3D11Device* device, int LatLines, int LongLines)
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
	vertexBufferData.pSysMem = &vertices[0]; // vertices
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

HRESULT SkyDomeBrayn::InitializeSkyDome(D3DClass* d3d)
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
	ID3D11Device* device = d3d->GetDevice();
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	result = device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

	result = D3DX11CompileFromFile(L"Engine/data/shaders/SkyDomeBraynShader.fx", 0, 0, "SKYMAP_VS", "vs_4_0", 0, 0, 0, &SKYMAP_VS_Buffer, 0, 0);
	if (FAILED(result))
	{
		return result;
	}
	result = D3DX11CompileFromFile(L"Engine/data/shaders/SkyDomeBraynShader.fx", 0, 0, "SKYMAP_PS", "ps_4_0", 0, 0, 0, &SKYMAP_PS_Buffer, 0, 0);
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

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	//Create the Input Layout
	result = device->CreateInputLayout( layout, numElements, SKYMAP_VS_Buffer->GetBufferPointer(), 
		SKYMAP_VS_Buffer->GetBufferSize(), &vertLayout );

	//Set the Input Layout
	deviceContext->IASetInputLayout( vertLayout );

	//Set Primitive Topology
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

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

	SetFillMode(device, D3D11_FILL_SOLID);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	return result;
}

HRESULT SkyDomeBrayn::SetFillMode(ID3D11Device* device, D3D11_FILL_MODE fillMode)
{
	D3D11_RASTERIZER_DESC cmdesc;
	HRESULT result = S_OK;
	
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = false;
	cmdesc.CullMode = D3D11_CULL_NONE;
	cmdesc.FillMode = fillMode;

	result = device->CreateRasterizerState(&cmdesc, &RSCullNone);
	if (FAILED(result))
	{
		return result;
	}
	return result;
}

void SkyDomeBrayn::UpdateSkyDome(D3DXVECTOR3 cameraPosition, float rotationX, float rotationY, float rotationZ)
{
	D3DXMATRIX scale, translation, rotationMatrix;
	float yaw, pitch, roll;

	//Reset sphereWorld
		// sphereWorld = XMMatrixIdentity();
	D3DXMatrixIdentity(&sphereWorld);

	//Define sphereWorld's world space matrix
		// Scale = XMMatrixScaling( 5.0f, 5.0f, 5.0f );
	D3DXMatrixScaling(&scale, 5.0f, 5.0f, 5.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = rotationX * 0.0174532925f;
	yaw   = rotationY * 0.0174532925f;
	roll  = rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	//Make sure the sphere is always centered around camera
		// Translation = XMMatrixTranslation( XMVectorGetX(camPosition), XMVectorGetY(camPosition), XMVectorGetZ(camPosition) );
	D3DXMatrixTranslation(&translation, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	D3DXMatrixMultiply(&sphereWorld, &sphereWorld, &rotationMatrix);

	//Set sphereWorld's world space using the transformations
	//D3DXMatrixMultiply(&sphereWorld, &scale, &translation);
	//sphereWorld = scale * translation;
}

void SkyDomeBrayn::RenderSkyDome(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX view, D3DXMATRIX projection, int shapeType)
{
	UINT stride = sizeof( Vertex );
	UINT offset = 0;

	D3DXMATRIX WVP;

	// Set primitive topology
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // D3D11_PRIMITIVE_TOPOLOGY_LINELIST

	if (shapeType == 0)
	{
		deviceContext->IASetVertexBuffers( 0, 1, &sphereVertBuffer, &stride, &offset );
		deviceContext->IASetIndexBuffer( sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}
	else
	{
		deviceContext->IASetVertexBuffers( 0, 1, &cubeVertBuffer, &stride, &offset );
		deviceContext->IASetIndexBuffer( cubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}
	

	//WVP = sphereWorld * view * projection;
	//D3DXMatrixMultiply(&view, &view, &projection);
	//D3DXMatrixMultiply(&WVP, &sphereWorld, &view);

	// cbPerObj.WVP = XMMatrixTranspose(WVP);
	// cbPerObj.World = XMMatrixTranspose(sphereWorld);

	//D3DXMatrixTranspose(&WVP, &WVP);
	// D3DXMatrixTranspose(&view, &view);

	// D3DXMatrixTranspose(&sphereWorld, &sphereWorld);
	D3DXMatrixTranspose(&projection, &projection);

	//cbPerObj.WVP = WVP;
	cbPerObj.worldMatrix = worldMatrix; // sphereWorld
	cbPerObj.viewMatrix = view;
	cbPerObj.projectionMatix = projection;

	deviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
	deviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
	deviceContext->PSSetShaderResources( 0, 1, &smrv );
	deviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );

	deviceContext->VSSetShader(SKYMAP_VS, 0, 0);
	deviceContext->PSSetShader(SKYMAP_PS, 0, 0);
	deviceContext->OMSetDepthStencilState(DSLessEqual, 0);
	deviceContext->RSSetState(RSCullNone);


	if (shapeType == 0)
	{
		deviceContext->DrawIndexed(NumSphereFaces * 3, 0, 0 ); // Sphere
	}
	else
	{
		deviceContext->DrawIndexed(36, 0, 0 ); // Cube
	}
	deviceContext->OMSetDepthStencilState(NULL, 0);
}

