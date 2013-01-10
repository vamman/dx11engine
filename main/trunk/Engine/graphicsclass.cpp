#include "graphicsclass.h"
#include "ResourceMgr.h"
#include "MacroHelper.h"
#include "FileSystemHelper.h"
#include "systemclass.h"

GraphicsClass::GraphicsClass() 
	: mIsAllowToBBRender(true)
	, mIsAllowToCameraDisplayRender(true)
	, mDirUp(true)
	, mDirDown(false)
	, mIsWireFrameModeOn(true)
	, mSkyShape((SkyShape)0)
	, mSkyPixelShaderType((SkyPixelShaderType)0)
	, mTerrain(0)
	, mQuadTree(0)
	, m_MiniMap(0)
	, mD3D(0)
	, mCamera(0)
	, mCameraMovement(0)
	, mBitmap(0)
	, mCursor(0)
	, m_RenderTexture(0)
	, m_Text(0)
	, m_Frustum(0)
	, mSkyDome(0)
	, m_SkyPlane(0)
	, mNumObjectsRendered(0)
	, m_ParticleSystem(0)
{
	for (int i = 0; i < 4; ++i)
	{
		mPointLights[i] = 0;
	}
	mTerrainRenderType = Terrain::TerrainRenderType::RENDER_QUAD_TREE; // RENDER_MATERIAL RENDER_QUAD_TREE
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


HRESULT GraphicsClass::Init(int screenWidth, int screenHeight, HWND hwnd)
{
	HRESULT result = S_FALSE;
	ModelObject* object = new ModelObject();
	mDrawFuncTime = -1;
	DWORD funcTime = -1;

	float cameraX, cameraY, cameraZ;
//	char videoCard[128];
//	int videoMemory;
	int terrainWidth, terrainHeight;

	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	mTimer = Timer::GetInstance();
	mTimer->Initialize();
	Timer::GetInstance()->SetTimeA();

	// Create the Direct3D object.
	mD3D = D3DClass::GetInstance();
	if(!mD3D)
	{
		return result;
	}

	// Initialize the Direct3D object.
	
	ACTION_THROUGH_ASSERT(mD3D->Initialize(mScreenWidth, mScreenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR),
		   L"Could not initialize Direct3D");
	ID3D11Device* device = mD3D->GetDevice();

	// Load ALL resources
	ResourceMgr* resourceManager = ResourceMgr::GetInstance();
	resourceManager->LoadResources();

	// Create the camera object.
	mCamera = new CameraClass;
	if(!mCamera)
	{
		return result;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	mCamera->SetPosition(0.0f, 0.0f, -1.0f);
	mCamera->Render();
	mCamera->GetViewMatrix(mBaseViewMatrix);
	

	// Set the initial position of the camera.
	cameraX = 130.0f;
	cameraY = 20.0f;
	cameraZ = 110.0f;

	mCamera->SetPosition(cameraX, cameraY, cameraZ);

	// Create the position object.
	mCameraMovement = new CameraMovement;
	if(!mCameraMovement)
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	mCameraMovement->SetPosition(cameraX, cameraY, cameraZ);

	// Create the text object.
	m_Text = new TextClass;
	if(!m_Text) { return result; }

	V_RETURN(m_Text->Initialize(mD3D->GetDevice(), mD3D->GetDeviceContext(), hwnd, mScreenWidth, mScreenHeight, mBaseViewMatrix),
			 L"Error",	L"Could not initialize the text object");

	// Create point light object
	PointLightInfo* pointLightInfos[4];
	for (int i = 0; i < 4; ++i)
	{
		pointLightInfos[i] = new PointLightInfo();
	}
	pointLightInfos[0]->color	= D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	pointLightInfos[0]->position = D3DXVECTOR3(-2.0f, 1.0f, 2.0f);

	pointLightInfos[1]->color	= D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	pointLightInfos[1]->position = D3DXVECTOR3(2.0f, 1.0f, 2.0f);

	pointLightInfos[2]->color	= D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	pointLightInfos[2]->position = D3DXVECTOR3(-2.0f, 1.0f, -2.0f);

	pointLightInfos[3]->color	= D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	pointLightInfos[3]->position = D3DXVECTOR3(2.0f, 1.0f, -2.0f);

	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if(!m_Frustum)
	{
		return result;
	}

	InitLights();
//	InitShaders(hwnd);
	InitMaterials();
	
	for (int i = 0; i < 4; ++i)
	{
		char* prefix = "light_";
		char resultName[20];
		char number[5];
		_itoa_s(i, number, 10);
		sprintf_s(resultName, "%s%s", prefix, number);

		mPointLights[i] = new LightClass;
		if(!mPointLights[i])
		{
			return result;
		}
		D3DXVECTOR4 color	 = pointLightInfos[i]->color;
		D3DXVECTOR3 position = pointLightInfos[i]->position;
		mPointLights[i]->SetDiffuseColor(color.x, color.y, color.z, color.w);
		mPointLights[i]->SetPosition(position.x, position.y, position.z);

		object = ModelFactory::GetInstance()->CreateOrdinaryModel(device, hwnd, resultName,
																  FileSystemHelper::GetResourcePath(L"/models/sphere.txt"),
																  ModelClass::NormalMapVertexType);
		object->SetMaterial(MaterialFactory::GetInstance()->GetMaterialByName("BlueFloor"));
		object->SetPosition(position);
	}

	InitObjects(hwnd);

	// Create the terrain object.
	mTerrain = new Terrain;
	if(!mTerrain) { return result; }

	if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_MATERIAL)
	{
		// heightmap01.bmp / heightmap513; materialmap01 / materialmap02; colorm01.bmp / colorm513
		V_RETURN(mTerrain->InitializeWithMaterials( device,	FileSystemHelper::GetResourcePath(L"/textures/terrain/simple_hmap_512_24.bmp").c_str(), // heightmap513.bmp simple_hmap_512_24.bmp
													FileSystemHelper::GetResourcePath(L"/textures/terrain/legend.txt").c_str(),
													FileSystemHelper::GetResourcePath(L"/textures/terrain/materialmap02.bmp").c_str(),
													FileSystemHelper::GetResourcePath(L"/textures/terrain/colorm513.bmp").c_str(), L"detail001"),	L"Error",
													L"Could not initialize the terrain object" );
	}
	else if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_QUAD_TREE)
	{
		// simple_hmap_512_24
		V_RETURN
		(
			mTerrain->InitializeWithQuadTree
			(
				device, FileSystemHelper::GetResourcePath(L"/textures/terrain/simple_hmap_512_24.bmp").c_str(),
				L"dirt01", FileSystemHelper::GetResourcePath(L"/textures/terrain/colorm513.bmp").c_str() 
			),
			L"Error", L"Could not initialize the terrain object"
		);
		// Create the quad tree object.
		mQuadTree = new QuadTree;
		if(!mQuadTree) { return false; }

		V_RETURN(mQuadTree->Initialize(mTerrain, device), L"Error",	L"Could not initialize the quad tree object");
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture) { return false; }
	V_RETURN(m_RenderTexture->Initialize(device, mScreenWidth, mScreenHeight), L"Error", L"Could not initialize the render texture object.");
	
	// Create the bitmap object.
	mBitmap = new BitmapClass;
	if(!mBitmap)
	{
		return result;
	}

	// Initialize the bitmap object.
	V_RETURN(mBitmap->Initialize(device, mScreenWidth, mScreenHeight, L"texture2" , 300, 225),
		L"Error", L"Could not initialize the bitmap object.");

	// Create cursor bitmap
	mCursorWidth = 32 * 2;
	mCursorHeight = 32 * 2;
	mCursor = new BitmapClass;
	if(!mCursor)
	{
		return result;
	}

	// Initialize the cursor bitmap object.
	V_RETURN(mCursor->Initialize(device, mScreenWidth, mScreenHeight, L"SC2Cursor1" , mCursorWidth / 2, mCursorHeight / 2),
		L"Error", L"Could not initialize cursor object.");

	// Probably need to initialize another instance of Texture shader for minimap
	// Get the size of the terrain as the minimap will require this information.
	mTerrain->GetTerrainSize(terrainWidth, terrainHeight);

	// Create the mini map object.
	m_MiniMap = new MiniMap;
	if(!m_MiniMap) { return result; }
	V_RETURN(m_MiniMap->Initialize(device, hwnd, screenWidth, screenHeight, mBaseViewMatrix, (float)(terrainWidth - 1), (float)(terrainHeight - 1)),
			 L"Error", L"Could not initialize the mini map object.");

	InputClass::GetInstance()->CenterMouseLocation();

	// Create the sky dome object.
	mSkyDome = new SkyDome;
	if(!mSkyDome) { return false; }
	V_RETURN(mSkyDome->Initialize(device, hwnd), L"Error",	L"Could not initialize the sky dome object");
	V_RETURN(mSkyDome->CreateCubeTexture(device), L"Error",	L"Could not initialize the sky dome cube map object");

	// Create the sky plane object.
	m_SkyPlane = new SkyPlane;
	if(!m_SkyPlane)	{ return false;	}
	V_RETURN(m_SkyPlane->Initialize(mD3D->GetDevice(), L"cloud001", L"perturb001"), L"Error", L"Could not initialize the sky plane object");
	
	// Create the particle system object.
	m_ParticleSystem = new ParticleSystem;

	// Initialize the particle system object.
	V_RETURN(m_ParticleSystem->Initialize(mD3D->GetDevice(), L"star"), L"Error", L"Could not initialize the sky dome cube map object");
	m_ParticleSystem->SetPosition(D3DXVECTOR3(128.0f, 8.0f, 129.0f));

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "GraphicsClass::Initialize time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "GraphicsClass::Initialize time: ");
	}
	
	return result;
}

bool GraphicsClass::InitLights()
{
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Create directional specular light object.
	mDirSpecLight = new LightClass;
	if(!mDirSpecLight)
	{
		return false;
	}

	mDirSpecLight->SetLightType(LightClass::DIRECTIONAL_SPECULAR_LIGHT);
	// Initialize the light object.
	mDirSpecLight->SetAmbientColor(0.4f, 0.4f, 0.4f, 1.0f);
	mDirSpecLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	mDirSpecLight->SetDirection(0.0f, 0.0f, 0.75f);
	mDirSpecLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	mDirSpecLight->SetSpecularPower(16.0f);

	// Create directional ambient light object.
	mDirAmbLight = new LightClass;
	if(!mDirAmbLight)
	{
		return false;
	}

	mDirAmbLight->SetLightType(LightClass::DIRECTIONAL_AMBIENT_LIGHT);
	// Initialize the light object.
	mDirAmbLight->SetAmbientColor(0.5f, 0.5f, 0.5f, 1.0f); // 0.5f, 0.5f, 0.5f, 1.0f
	mDirAmbLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	mDirAmbLight->SetDirection(0.0f, 0.0f, 0.75f); //  -0.5f, -1.0f, 0.0f

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "	GraphicsClass::InitLights time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "	GraphicsClass::InitLights time: ");
	}

	return true;
}

bool GraphicsClass::InitMaterials()
{
	Material* material = new Material("defaultMaterial");
	bool result = true;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Create material "NormalWithSpec"
	material = MaterialFactory::GetInstance()->CreateMaterial("NormalWithSpec");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"stone02");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"bump02");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"spec02");
	if(!result) { return false; }

	material->SetMaterialShader((BasicShader*) ResourceMgr::GetInstance()->GetResourceByName(L"SpecMapShader", ResourceMgr::ResourceType::ResourceTypeShader)); // m_SpecMapShader

	// Create material "BlueFloor"
	material = MaterialFactory::GetInstance()->CreateMaterial("BlueFloor");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"blue01");
	if(!result) { return false; }

	material->SetMaterialShader((BasicShader*) ResourceMgr::GetInstance()->GetResourceByName(L"SpecularLight", ResourceMgr::ResourceType::ResourceTypeShader)); // mDirSpecLightShaders

	// Create material "TexturedFloor"
	material = MaterialFactory::GetInstance()->CreateMaterial("TexturedFloor");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"stone02");
	if(!result) { return false; }
	
	material->SetMaterialShader((BasicShader*) ResourceMgr::GetInstance()->GetResourceByName(L"AmbientLight", ResourceMgr::ResourceType::ResourceTypeShader)); // mDirAmbLightShader

	// Create normal map material for space compound
	material = MaterialFactory::GetInstance()->CreateMaterial("spaceCompoundMaterial");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"stone01");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"bump01");
	if(!result) { return false; }
	
	material->SetMaterialShader((BasicShader*) ResourceMgr::GetInstance()->GetResourceByName(L"NormalMapShader", ResourceMgr::ResourceType::ResourceTypeShader)); // m_BumpMapShader

	// Create fire material
	material = MaterialFactory::GetInstance()->CreateMaterial("fireMaterial");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"fire01");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"noise01");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"alpha01");
	if(!result) { return false; }
	
	material->SetMaterialShader((BasicShader*) ResourceMgr::GetInstance()->GetResourceByName(L"FireShader", ResourceMgr::ResourceType::ResourceTypeShader)); // m_FireShader

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "	GraphicsClass::InitMaterials time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "	GraphicsClass::InitMaterials time: ");
	}

	return true;
}

bool GraphicsClass::InitObjects(HWND hwnd)
{
	ModelObject* object = new ModelObject();
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	//  Create instanced sphere
	CREATE_INSTANCED_OBJ_WITH_MAT("instancedSphere", FileSystemHelper::GetResourcePath(L"/models/sphere.txt"), "NormalWithSpec", 5)

	// Create floor
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "floor", FileSystemHelper::GetResourcePath(L"/models/floor.txt"), "TexturedFloor", ModelClass::NormalMapVertexType);
	object->SetPosition(D3DXVECTOR3(130.0f, 1.0f, 132.0f)); 

	// Create 10 ordinary spheres
	for (int i = 0; i < 5; ++i)
	{
		char* prefix = "sphere_";
		char resultName[20];
		char number[5];
		_itoa_s(i, number, 10);
		sprintf_s(resultName, "%s%s", prefix, number);

		CREATE_ORDINARY_OBJ_WITH_MAT(object, resultName, FileSystemHelper::GetResourcePath(L"/models/sphere.txt"), "NormalWithSpec", ModelClass::NormalMapVertexType);

		// Generate a random position in front of the viewer for the mode.
		float positionX = 130.0f; // 130.0f;
		float positionY = 0.0f; // 0.0f;
		float positionZ = 130.0f; // 130.0f;

		// Generate a random position in front of the viewer for the mode.
		positionX += (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		positionY += (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		positionZ += ((((float)rand()-(float)rand())/RAND_MAX) * 10.0f) + 5.0f;

		// Load the instance array with data.
		D3DXVECTOR3 posVector = D3DXVECTOR3(positionX, positionY, positionZ);

		object->SetPosition(posVector);
	}

	// Create cube
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "cube", FileSystemHelper::GetResourcePath(L"/models/cube.txt"), "NormalWithSpec", ModelClass::NormalMapVertexType);
	object->SetPosition(D3DXVECTOR3(130.0f, 1.0f, 130.0f));

	// Create space compound from TXT
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "spaceCompound", FileSystemHelper::GetResourcePath(L"/models/spaceCompound.txt"), "spaceCompoundMaterial", ModelClass::NormalMapVertexType);
	object->SetPosition(D3DXVECTOR3(130.0f, 0.0f, 132.0f));

	// Create space compound from OBJ
	/*
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "spaceCompound", string("data/models/spaceCompound/spaceCompound.obj"), "spaceCompoundMaterial");
	object->SetPosition(D3DXVECTOR3(130.0f, 0.0f, 132.0f));
	*/

	CREATE_ORDINARY_OBJ_WITH_MAT(object, "cubeObj", FileSystemHelper::GetResourcePath(L"/models/cube.obj"), "NormalWithSpec", ModelClass::NormalMapVertexType);
	object->SetPosition(D3DXVECTOR3(130.0f, 0.0f, 125.0f));

	// Create fire object
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "fireObj", FileSystemHelper::GetResourcePath(L"/models/square.txt"), "fireMaterial", ModelClass::TextureVertexType);
	object->SetPosition(D3DXVECTOR3(135.0f, 1.0f, 125.0f));

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "	GraphicsClass::InitObjects time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "	GraphicsClass::InitObjects time: ");
	}
	return true;
}


void GraphicsClass::Shutdown()
{
	ResourceMgr::GetInstance()->Shutdown();

	BufferManager::GetInstance()->Shutdown();

	// Release the text object.
	SHUTDOWN_OBJ(m_Text);
	// Release the render to texture object.
	SHUTDOWN_OBJ(m_RenderTexture);
	// Release the bitmap object.
	SHUTDOWN_OBJ(mBitmap);
	// Release cursor object.
	SHUTDOWN_OBJ(mCursor);
	// Release the mini map object.
	SHUTDOWN_OBJ(m_MiniMap);
	// Shutdown particle system
	SHUTDOWN_OBJ(m_ParticleSystem);

	// Release the light object.
	if(mDirSpecLight)
	{
		delete mDirSpecLight;
		mDirSpecLight = 0;
	}

	if(mDirAmbLight)
	{
		delete mDirAmbLight;
		mDirAmbLight = 0;
	}

	// Release the frustum object.
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// Release the quad tree object.
	SHUTDOWN_OBJ(mQuadTree);

	// Release the terrain object.
	SHUTDOWN_OBJ(mTerrain);

	// Release the camera object.
	if(mCamera)
	{
		delete mCamera;
		mCamera = 0;
	}

	// Release the position object.
	if(mCameraMovement)
	{
		delete mCameraMovement;
		mCameraMovement = 0;
	}
	// Release the sky dome object.
	SHUTDOWN_OBJ(mSkyDome);

	ModelFactory::GetInstance()->Shutdown();

	SHUTDOWN_OBJ(mD3D);

	SHUTDOWN_OBJ(m_SkyPlane);

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	mTimer->Frame();
	FpsClass::GetInstance()->Frame();
	CpuClass::GetInstance()->Frame();

	// Do the frame input processing.
	result = InputClass::GetInstance()->Frame();
	if(!result) { return false; }

	if (InputClass::GetInstance()->IsEscapePressed() == true) { return false; }

	result = HandleInput(mTimer->GetTime());
	if(!result) { return false; }

	// Do the sky plane frame processing.
	m_SkyPlane->Frame();

	// Run the frame processing for the particle system.
	m_ParticleSystem->Frame(mTimer->GetTime(), mD3D->GetDeviceContext());

	// Render the graphics scene.
	result = Render();
	if(!result) { return false; }

	return true;
}

bool GraphicsClass::HandleInput(float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;
	D3DXVECTOR3 normalCameraDirectionVector, normalCameraRightVector;

	if (InputClass::GetInstance()->IsTAB_PressedAndUnpressed(mIsWireFrameModeOn))
	{
		SetFillMode(D3D11_FILL_WIREFRAME);
		mSkyDome->SetFillMode(mD3D, D3D11_FILL_WIREFRAME);
	}
	else
	{
		SetFillMode(D3D11_FILL_SOLID);
		mSkyDome->SetFillMode(mD3D, D3D11_FILL_SOLID);
	}
	
	normalCameraDirectionVector = mCamera->GetNormalDirectionVector();
	normalCameraRightVector = mCamera->GetNormalRightVector();

	// Set the frame time for calculating the updated position.
	mCameraMovement->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = InputClass::GetInstance()->IsWPressed();
	mCameraMovement->MoveForward(keyDown, normalCameraDirectionVector);
	keyDown = InputClass::GetInstance()->IsSPressed();
	mCameraMovement->MoveBackward(keyDown, normalCameraDirectionVector);

	keyDown = InputClass::GetInstance()->IsAPressed();
	mCameraMovement->StrafeLeft(keyDown, normalCameraRightVector);
	keyDown = InputClass::GetInstance()->IsDPressed();
	mCameraMovement->StrafeRight(keyDown, normalCameraRightVector);

	keyDown = InputClass::GetInstance()->IsPgUpPressed();
	mCameraMovement->LookUpward(keyDown);

	keyDown = InputClass::GetInstance()->IsPgDownPressed();
	mCameraMovement->LookDownward(keyDown);

	// Get the view point position/rotation.
	mCameraMovement->GetPosition(posX, posY, posZ);
	mCameraMovement->GetRotation(rotX, rotY, rotZ);

	// Get mouse delta and center mouse location
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	InputClass::GetInstance()->GetMouseDelta(mouseDeltaX, mouseDeltaY);
	mCameraMovement->MouseMoveHorizontal(mouseDeltaX);
	mCameraMovement->MouseMoveVertical(mouseDeltaY);

	mCameraMovement->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	mCamera->SetPosition(posX, posY, posZ);
	mCamera->SetRotation(rotX, rotY, rotZ);

	// Update the location of the camera on the mini map.
	m_MiniMap->PositionUpdate(posX, posZ);
	
	if (InputClass::GetInstance()->Is1Pressed() && mSkyShape != SKY_SPHERE)
	{
		mSkyShape = SKY_SPHERE;
	}
	else if (InputClass::GetInstance()->Is2Pressed() && mSkyShape != SKY_CUBE)
	{
		mSkyShape = SKY_CUBE;
	}

	if (InputClass::GetInstance()->IsNumPad0Pressed() && mSkyPixelShaderType != SKY_PIXEL_GRADIENT)
	{
		mSkyPixelShaderType = SKY_PIXEL_GRADIENT;
	}
	else if (InputClass::GetInstance()->IsNumPad1Pressed() && mSkyPixelShaderType != SKY_PIXEL_CUBE_TEXTURE)
	{
		mSkyPixelShaderType = SKY_PIXEL_CUBE_TEXTURE;
	}

	return true;
}

bool GraphicsClass::SetFillMode(D3D11_FILL_MODE mode)
{
	D3D11_RASTERIZER_DESC rasterDesc;
	ID3D11RasterizerState* m_rasterState;
	HRESULT result;

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = mode; // D3D11_FILL_SOLID // D3D11_FILL_WIREFRAME
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = mD3D->GetDevice()->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result)) { return false; }

	// Now set the rasterizer state.
	mD3D->GetDeviceContext()->RSSetState(m_rasterState);
	return true;
}

bool GraphicsClass::Render()
{
	bool result;
	float  fogColor, fogStart, fogEnd;
	mIsAllowToBBRender = InputClass::GetInstance()->IsHOME_PressedAndUnpressed();

	// Set the color of the fog.
	fogColor = 0.5f;
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// For camera positioning above terrain
	bool foundHeight = false;
	D3DXVECTOR3 position;
	float height;

	Timer::GetInstance()->SetTimeA();

	// Generate the view matrix based on the camera's position.
	mCamera->Render();

	// Get the current position of the camera.
	position = mCamera->GetPosition();

	// Get the height of the triangle that is directly underneath the given camera position.
	if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_QUAD_TREE)
	{
		foundHeight =  mQuadTree->GetHeightAtPosition(position.x, position.z, height);
		if(foundHeight)
		{
			// If there was a triangle under the camera then position the camera just above it by two units.
			mCamera->SetPosition(position.x, height + 2.0f, position.z);
		}
	}

	//result = RenderToTextureFromReflectionView(); // TODO

	// Render the entire scene to the texture first.
	mIsAllowToCameraDisplayRender = InputClass::GetInstance()->IsEND_PressedAndUnpressed();
	if (mIsAllowToCameraDisplayRender)
	{
		result = RenderToTextureFromCameraView();
		if(!result) { return false; }
	}

	// Clear the buffers to begin the scene.
	mD3D->BeginScene(fogColor, fogColor, fogColor, 1.0);

	// Render the scene as normal to the back buffer.
	result = RenderScene();
	if(!result) { return false; }

	Render2D();

	// Present the rendered scene to the screen.
	mD3D->EndScene();
	Timer::GetInstance()->SetTimeB();
	mDrawFuncTime = Timer::GetInstance()->GetDeltaTime();

	return true;
}

bool GraphicsClass::RenderScene()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, staticWorldMatrix, reflectionMatrix;
	int modelCount;
	float fogColor, fogStart, fogEnd;
	D3DXVECTOR4 color;
	static float rotation = 0.0f;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();
	D3DXVECTOR3 cameraPosition;

	// Set the color of the fog to grey.
	fogColor = 0.5f;
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// Generate the view matrix based on the camera's position.
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	staticWorldMatrix = worldMatrix;
	mD3D->GetProjectionMatrix(projectionMatrix);

	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// Get the number of models that will be rendered.
	modelCount = ModelFactory::GetInstance()->GetModelCount();

	// Initialize the count of models that have been rendered.
	mNumObjectsRendered = 0;

	D3DXVECTOR4 pointDiffuseColors[4];
	D3DXVECTOR4 pointLightPositions[4];

	for (int i = 0; i < 4; ++i)
	{
		pointDiffuseColors[i]  = mPointLights[i]->GetDiffuseColor();
		pointLightPositions[i] = mPointLights[i]->GetPosition();
	}

	////////////////// RENDER SKY DOME BEGIN //////////////////////////
	// Get the position of the camera.
	cameraPosition = mCamera->GetPosition();

	// Translate the sky dome to be centered around the camera position.
	D3DXMatrixTranslation(&worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	// Turn off back face culling.
	mD3D->TurnOffCulling();

	// Turn off the Z buffer.
	mD3D->TurnZBufferOff();
 
	if (mIsWireFrameModeOn)
	{
		mSkyDome->SetFillMode(mD3D, D3D11_FILL_WIREFRAME);
	}
	else
	{
		mSkyDome->SetFillMode(mD3D, D3D11_FILL_SOLID);
	}
	
	// Render the sky dome using the sky dome shader.
	mSkyDome->Render(deviceContext, mSkyShape);
	SkyDomeShader* shader = (SkyDomeShader*) ResourceMgr::GetInstance()->GetResourceByName(L"SkyDomeShader", ResourceMgr::ResourceType::ResourceTypeShader);
	shader->Render(deviceContext, mSkyDome->GetIndexCount(mSkyShape), worldMatrix, viewMatrix, projectionMatrix,
					mSkyDome->GetApexColor(), mSkyDome->GetCenterColor(), (float) mSkyPixelShaderType);

	// Turn back face culling back on.
	mD3D->TurnOnCulling();
	
	// RenderSkyPlane(worldMatrix, viewMatrix, projectionMatrix);

	// Turn the Z buffer back on.
	mD3D->TurnZBufferOn();

	// Reset the world matrix.
	mD3D->GetWorldMatrix(worldMatrix);
	////////////////// RENDER SKY DOME END //////////////////////////
	
	if (mIsWireFrameModeOn)
	{
		SetFillMode(D3D11_FILL_WIREFRAME);
	}
	else
	{
		SetFillMode(D3D11_FILL_SOLID);
	}
	
	RenderFire();

	RenderParticles();
	
	if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_MATERIAL)
	{
		RenderTerrainWithMaterials(worldMatrix, viewMatrix, projectionMatrix);
	}
	else if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_QUAD_TREE)
	{
		RenderTerrainWithQuadTree(worldMatrix, viewMatrix, projectionMatrix);
	}

	ModelObject* modelObj = ModelFactory::GetInstance()->GetObjectByName("floor");
	if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_QUAD_TREE)
	{
		SetPositionAboveTerrain(modelObj, 0.1f);
	}
	RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirAmbLight, LightClass::DIRECTIONAL_AMBIENT_LIGHT, false);

	RenderObjects(worldMatrix, viewMatrix, projectionMatrix, fogStart, fogEnd);

	return true;
}

void GraphicsClass::RenderSkyPlane(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	// Enable additive blending so the clouds blend with the sky dome color.
	mD3D->EnableSecondBlendState();

	// Render the sky plane using the sky plane shader.
	m_SkyPlane->Render(mD3D->GetDeviceContext());

	vector<ID3D11ShaderResourceView*> textureArray;
	textureArray.push_back(m_SkyPlane->GetCloudTexture());
	textureArray.push_back(m_SkyPlane->GetPerturbTexture());

	SkyPlaneShader* shader = (SkyPlaneShader*) ResourceMgr::GetInstance()->GetResourceByName(L"SkyPlaneShader", ResourceMgr::ResourceType::ResourceTypeShader);
	shader->SetTextureArray(mD3D->GetDeviceContext(), textureArray);
	shader->SetSkyBuffer(mD3D->GetDeviceContext(), m_SkyPlane->GetTranslation(0), m_SkyPlane->GetScale(), m_SkyPlane->GetBrightness());
	shader->Render(mD3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	// Turn off blending.
	mD3D->TurnOffAlphaBlending();
}

HRESULT GraphicsClass::Render2D()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	HRESULT result = S_OK;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();
	// Generate the view matrix based on the camera's position.
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mD3D->GetWorldMatrix(worldMatrix);
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetProjectionMatrix(projectionMatrix);

	// We now also get the ortho matrix from the D3DClass for 2D rendering. We will pass this in instead of the projection matrix.
	mD3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Render to camera display bitmap
	if (mIsAllowToCameraDisplayRender)
	{
		// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = mBitmap->Render(deviceContext, 0, 0);
		if(FAILED(result)) { return result; }

		vector<ID3D11ShaderResourceView*> textureArray;
		textureArray.push_back(m_RenderTexture->GetShaderResourceView());

		// Render the bitmap with the texture shader.
		TextureShader* shader = (TextureShader*) ResourceMgr::GetInstance()->GetResourceByName(L"TextureShaderNonInstanced", ResourceMgr::ResourceType::ResourceTypeShader);
		shader->SetTextureArray(deviceContext, textureArray);
		result = shader->RenderOrdinary(deviceContext,
										mBitmap->GetIndexCount(),
										worldMatrix,
										mBaseViewMatrix, 
										orthoMatrix);
		if(FAILED(result)) { return result; }
	}

	// Turn on the alpha blending before rendering the text.
	mD3D->TurnOnAlphaBlending();

	// Render the mini map.
	float cameraRotX, cameraRotY, cameraRotZ;
	mCameraMovement->GetRotation(cameraRotX, cameraRotY, cameraRotZ);
	TextureShader* miniMapShader = (TextureShader*) ResourceMgr::GetInstance()->GetResourceByName(L"TextureShaderNonInstanced", ResourceMgr::ResourceType::ResourceTypeShader);
	result = m_MiniMap->Render(deviceContext, worldMatrix, orthoMatrix, miniMapShader, cameraRotY);
	if(FAILED(result)) { return result; }

	// Render cursor object
	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = mCursor->Render(deviceContext, mScreenWidth / 2 - mCursorWidth / 4, mScreenHeight / 2 - mCursorHeight / 4 );
	if(FAILED(result)) { return result; }

	D3DXVECTOR4 pixelColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vector<ID3D11ShaderResourceView*> textureArray;
	textureArray.push_back(mCursor->GetTexture());

	// Render the bitmap with the texture shader.
	FontShader* shader = (FontShader*) ResourceMgr::GetInstance()->GetResourceByName(L"CursorShader", ResourceMgr::ResourceType::ResourceTypeShader);
	shader->SetTextureArray(mD3D->GetDeviceContext(), textureArray);
	shader->SetPixelBufferColor(deviceContext, pixelColor);
	result = shader->RenderOrdinary(deviceContext,
		mCursor->GetIndexCount(),
		worldMatrix,
		mBaseViewMatrix, 
		orthoMatrix);
	if(FAILED(result)) { return result; }

	// Render debug text
	RenderText();

	// Turn off alpha blending after rendering the text.
	mD3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();

	return result;
}

HRESULT GraphicsClass::RenderFire()
{
	HRESULT result;
	D3DXVECTOR3 scrollSpeeds, scales;
	D3DXVECTOR2 distortion1, distortion2, distortion3;
	float distortionScale, distortionBias;
	static float frameTime = 0.0f;

	// Increment the frame time counter.
	frameTime += 0.01f;
	if(frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// Set the three scrolling speeds for the three different noise textures.
	scrollSpeeds = D3DXVECTOR3(1.3f, 2.1f, 2.3f);

	// Set the three scales which will be used to create the three different noise octave textures.
	scales = D3DXVECTOR3(1.0f, 2.0f, 3.0f);

	// Set the three different x and y distortion factors for the three different noise textures.
	distortion1 = D3DXVECTOR2(0.1f, 0.2f);
	distortion2 = D3DXVECTOR2(0.1f, 0.3f);
	distortion3 = D3DXVECTOR2(0.1f, 0.1f);

	// The the scale and bias of the texture coordinate sampling perturbation.
	distortionScale = 0.8f;
	distortionBias = 0.5f;

	// Turn on alpha blending for the fire transparency.
	mD3D->TurnOnAlphaBlending();

	ModelObject* modelObj = ModelFactory::GetInstance()->GetObjectByName("fireObj");
	modelObj->GetModel()->RenderOrdinary(mD3D->GetDeviceContext(), ModelClass::TextureVertexType);

	vector<ID3D11ShaderResourceView*> textureArray;
	Material* material = MaterialFactory::GetInstance()->GetMaterialByName("fireMaterial");
	
	FireShader* shader = (FireShader*) ResourceMgr::GetInstance()->GetResourceByName(L"FireShader", ResourceMgr::ResourceType::ResourceTypeShader);
	shader->SetTextureArray(mD3D->GetDeviceContext(), material->GetTextureVector());

	result = shader->RenderOrdinary(mD3D->GetDeviceContext(), modelObj, mCamera, frameTime, scrollSpeeds, scales, distortion1, 
										  distortion2, distortion3, distortionScale, distortionBias);
	// Turn off alpha blending.
	mD3D->TurnOffAlphaBlending();

	return result;
}

HRESULT GraphicsClass::RenderParticles()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	HRESULT result;
	D3DXVECTOR3 cameraPosition, particlesPosition;
	double angle;
	float rotation;

	// Turn on alpha blending.
	mD3D->TurnOnAlphaBlending();

	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ParticleSystem->Render(mD3D->GetDeviceContext());

	vector<ID3D11ShaderResourceView*> textureArray;
	textureArray.push_back(m_ParticleSystem->GetTexture());

	//////////////////////////////////////////
	// Billboard the particles
	// ///////////////////////////////////////
	/*
	cameraPosition = mCamera->GetPosition();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(viewMatrix);
	D3DClass::GetInstance()->GetProjectionMatrix(projectionMatrix);

	particlesPosition = m_ParticleSystem->GetPosition();
	// Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
	angle = atan2(particlesPosition.x - cameraPosition.x, particlesPosition.z - cameraPosition.z) * (180.0 / D3DX_PI);

	// Convert rotation into radians.
	rotation = (float)angle * 0.0174532925f;

	// Setup the rotation the billboard at the origin using the world matrix.
	D3DXMatrixRotationY(&worldMatrix, rotation);

	// Setup the translation matrix from the billboard model.
	D3DXMatrixTranslation(&translateMatrix, particlesPosition.x, particlesPosition.y, particlesPosition.z);

	// Finally combine the rotation and translation matrices to create the final world matrix for the billboard model.
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translateMatrix); 
	*/
	//////////////////////////////////////////
	
	// Get the world, view, and projection matrices from the camera and d3d objects.
	mD3D->GetWorldMatrix(worldMatrix);
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetProjectionMatrix(projectionMatrix);

	// Render the model using the texture shader.
	ParticleShader* particleShader = (ParticleShader*) ResourceMgr::GetInstance()->GetResourceByName(L"ParticleShader", ResourceMgr::ResourceType::ResourceTypeShader);
	particleShader->SetTextureArray(mD3D->GetDeviceContext(), textureArray);
	result = particleShader->RenderOrdinary(mD3D->GetDeviceContext(), m_ParticleSystem->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	// Turn off alpha blending.
	mD3D->TurnOffAlphaBlending();
	
	return result;
}

bool GraphicsClass::RenderToTextureFromCameraView()
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(mD3D->GetDeviceContext(), mD3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(mD3D->GetDeviceContext(), mD3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Render the scene now and it will draw to the render to texture instead of the back buffer.
	result = RenderScene();
	if(!result) { return false; }

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	return true;
}

// TODO
bool GraphicsClass::RenderToTextureFromReflectionView()
{
	D3DXMATRIX worldMatrix, viewMatrix, reflectionViewMatrix, projectionMatrix;
	float fogStart, fogEnd;
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(mD3D->GetDeviceContext(), mD3D->GetDepthStencilView());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(mD3D->GetDeviceContext(), mD3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to calculate the reflection matrix.
	mCamera->RenderReflection(-1.5f);
 
	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = mCamera->GetReflectionViewMatrix();

	// Get the world and projection matrices.
	mD3D->GetWorldMatrix(worldMatrix);
	mD3D->GetProjectionMatrix(projectionMatrix);

	mCamera->GetViewMatrix(viewMatrix);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	return true;
}

HRESULT GraphicsClass::RenderTerrainWithMaterials(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result = true;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

	// Render the terrain buffers.
	vector<ID3D11ShaderResourceView*> texArr;
	TerrainShader* shader = (TerrainShader*) ResourceMgr::GetInstance()->GetResourceByName(L"TerrainWithMaterials", ResourceMgr::ResourceType::ResourceTypeShader);
	shader->SetLightSource(deviceContext, mDirAmbLight);
	
	result = shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(FAILED(result)) { return result; }
	
	result = mTerrain->Render(deviceContext, shader, worldMatrix, viewMatrix, projectionMatrix);
	return result;
}

HRESULT GraphicsClass::RenderTerrainWithQuadTree(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result = true;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

	// Render the terrain buffers.
	vector<ID3D11ShaderResourceView*> texArr;
	texArr.push_back(mTerrain->GetTexture());

	// Render the terrain using the terrain shader.
	TerrainShader* shader = (TerrainShader*) ResourceMgr::GetInstance()->GetResourceByName(L"TerrainWithQuadTree", ResourceMgr::ResourceType::ResourceTypeShader);
	shader->SetTextureArray(deviceContext, texArr, false);
	shader->SetLightSource(deviceContext, mDirAmbLight);
	result = shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(FAILED(result)) { return result; }

	// Render the terrain using the quad tree and terrain shader.
	mQuadTree->Render(m_Frustum, deviceContext, shader, mIsAllowToBBRender);
	return result;
}

bool GraphicsClass::RenderObjects(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, float fogStart, float fogEnd)
{
	bool renderModel;
	D3DXMATRIX reflectionMatrix, rotationMatrix;
	reflectionMatrix = mCamera->GetReflectionViewMatrix();
	static float rotation = 0.0f;
	static float dirDelta = 0.0f;
	mNumObjectsRendered = 0;
	int numIndex = 0;

	ModelClass* model;
	ModelObject* modelObj;
	D3DXVECTOR3 posVector;

	vector<ModelObject*> listOfModels = ModelFactory::GetInstance()->GetVectorOfObjects();
	vector<ModelObject*>::iterator modelIt;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

	float dirDeltaStep = 0.01f;
	if (mDirUp) { dirDelta += dirDeltaStep; }
	if (mDirDown) { dirDelta -= dirDeltaStep; }

	if (dirDelta > 1.0f) { mDirUp   = false; mDirDown = true; }
	if (dirDelta < -1.0f) { mDirUp   = true; mDirDown = false; }
	
	mDirSpecLight->SetDirection(0.0f, dirDelta, 1.0f);
	mDirAmbLight->SetDirection(0.0f, -0.5f, 1.0f); // dirDelta

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.00099f;
	if(rotation > 360.0f) { rotation -= 360.0f; }

	for (modelIt = listOfModels.begin(); modelIt != listOfModels.end(); ++modelIt)
	{
		modelObj = (*modelIt);
		model = modelObj->GetModel();

		// Exclusions. TODO: Create understandable way of excluding objects from main draw loop
		if ( strcmp(modelObj->GetModelName(), "skyDomeSphere") == 0 || strcmp(modelObj->GetModelName(), "skyDomeCube") == 0 )
		{
			continue;
		}

		Material* material = modelObj->GetMaterial();
		vector<ID3D11ShaderResourceView*> textureVector = material->GetTextureVector();
		ID3D11ShaderResourceView** textureArray = &textureVector[0];

		// Render instanced objects
		if (modelObj->IsInstanced())
		{
			if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_QUAD_TREE)
			{
				SetPositionAboveTerrain(modelObj, 1.0f);
			}
			RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirSpecLight, LightClass::DIRECTIONAL_SPECULAR_LIGHT, true);	
		}
		// Render non-instanced objects
		else if (!modelObj->IsInstanced())
		{
			
			// Draw cube
			if (strcmp(modelObj->GetModelName(), "cube") == 0)
			{
				modelObj->SetRotation(rotation);
			//	SetPositionAboveTerrain(modelObj, 1.0f);
				RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirSpecLight, LightClass::DIRECTIONAL_AMBIENT_LIGHT, false);
			}
			// Draw spaceCompound
			/*
			else if (strcmp(modelObj->GetModelName(), "spaceCompound") == 0)
			{
				// Turn back face culling back off.
				mD3D->TurnOffCulling();
				RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirAmbLight, LightClass::DIRECTIONAL_AMBIENT_LIGHT, false);
				// Turn back face culling back on.
				mD3D->TurnOnCulling();
			}
			*/
			// Draw cubeObj
			if (strcmp(modelObj->GetModelName(), "cubeObj") == 0)
			{
				// Turn back face culling back off.
				mD3D->TurnOffCulling();
				RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirAmbLight, LightClass::DIRECTIONAL_AMBIENT_LIGHT, false);
				// Turn back face culling back on.
				mD3D->TurnOnCulling();
			}

			else if (strcmp(modelObj->GetModelName(), "floor") != 0) // ordinary created spheras
			{
				string objName(modelObj->GetModelName());
				string sphereObjPrefix, lightObjPrefix;

				sphereObjPrefix = objName.substr(0, 7);
				lightObjPrefix  = objName.substr(0, 6);

				// Set the radius of the sphere to 1.0 since this is already known.
				float radius = 1.0f;

				// Check if the sphere model is in the view frustum.
				renderModel = m_Frustum->CheckSphere(modelObj->GetPosition(), radius);

				// If it can be seen then render it, if not skip this model and check the next sphere.
				if(renderModel)
				{
					if (strcmp(sphereObjPrefix.c_str(), "sphere_") == 0)
					{
						float rot = numIndex % 2 == 0 ? rotation : -rotation;
						modelObj->SetRotation(rot);
					//	SetPositionAboveTerrain(modelObj, radius);
						RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirSpecLight, LightClass::DIRECTIONAL_SPECULAR_LIGHT, false);
					}
					else if (strcmp(lightObjPrefix.c_str(), "light_") == 0)
					{
						modelObj->SetScale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
						RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirSpecLight, LightClass::DIRECTIONAL_SPECULAR_LIGHT, false);
					}

					// Since this model was rendered then increase the count for this frame.
					mNumObjectsRendered++;
				}
			}
		}
		numIndex++;
	}
	return true;
}

HRESULT GraphicsClass::RenderObject(ModelObject* modelObj, ID3D11DeviceContext* deviceContext, D3DXMATRIX viewMatrix,
								 D3DXMATRIX projectionMatrix, LightClass* lightSource, LightClass::LightTypes lightType, bool isInstanced)
{
	HRESULT result = S_OK;
	D3DXMATRIX worldMatrix = modelObj->GetWorldMatrix();
	ModelClass* model = modelObj->GetModel();
	Material* material = modelObj->GetMaterial();
	BasicShader* materialShader = material->GetMaterialShader();
	vector<ID3D11ShaderResourceView*> textureVector = material->GetTextureVector();

	worldMatrix = modelObj->GetWorldMatrix();

	materialShader->SetTextureArray(deviceContext, textureVector);
	materialShader->SetCameraPosition(deviceContext, mCamera->GetPosition(), lightType);
	materialShader->SetLightSource(deviceContext, lightSource);

	if (!isInstanced)
	{
		model->RenderOrdinary(deviceContext, ModelClass::NormalMapVertexType);
		result = materialShader->RenderOrdinary(deviceContext,
			model->GetIndexCount(),
			worldMatrix,
			viewMatrix,
			projectionMatrix);
		if(FAILED(result)) { return result; }
	}
	else
	{
		model->RenderInstanced(deviceContext, ModelClass::NormalMapVertexType);
		result = materialShader->RenderInstanced(deviceContext,
			model->GetVertexCount(),
			model->GetInstanceCount(),
			worldMatrix,
			viewMatrix, 
			projectionMatrix);
		if(FAILED(result)) { return result; }
	}
	return result;
}

void GraphicsClass::SetPositionAboveTerrain(ModelObject* modelObject, float heightAbove)
{
	float height;

	// Set psition above terrain
	bool foundHeight =  mQuadTree->GetHeightAtPosition(modelObject->GetPosition().x, modelObject->GetPosition().z, height);
	if(foundHeight)
	{
		// If there was a triangle under the camera then position the camera just above it by two units.
		D3DXVECTOR3 newPosition = D3DXVECTOR3(modelObject->GetPosition().x, height + heightAbove, modelObject->GetPosition().z);
		modelObject->SetPosition(newPosition);
	}
}

HRESULT GraphicsClass::RenderText()
{
	D3DXMATRIX orthoMatrix, worldMatrix, staticWorldMatrix;
	int MAX_STRING_LENGTH = 30;
	char tempString[10];
	char cpuString[20];
	float posX, posY, posZ, rotX, rotY, rotZ;
	int terrinDrawCount, mouseX, mouseY;
	HRESULT result;

	int sentenceNumber = 0;

	int textPosX = RESOLUTION_X - RESOLUTION_X / 10;
	int textPosY = RESOLUTION_Y / 10;

	// SetFillMode(D3D11_FILL_SOLID);

	// Generate the view matrix based on the camera's position.
	mCamera->Render();
	mD3D->GetOrthoMatrix(orthoMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	staticWorldMatrix = worldMatrix;

	// Convert the fps integer to string format.
	WRITE_SENTENCE(mD3D, FpsClass::GetInstance()->GetFps(), "FPS: ", textPosX, textPosY, 0.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	// Convert the cpu integer to string format.
	_itoa_s(CpuClass::GetInstance()->GetCpuPercentage(), tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	result = m_Text->AddSentence(mD3D, cpuString, textPosX, textPosY + 20, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	// Convert the count integer to string format.
	WRITE_SENTENCE(mD3D, mNumObjectsRendered, "Render Count: ", textPosX, textPosY + 20 * 5, 1.0f, 0.0f, 0.0f, sentenceNumber);
	++sentenceNumber;

	// Convert the mouseX integer to string format.
	InputClass::GetInstance()->GetMouseLocation(mouseX, mouseY);
	WRITE_SENTENCE(mD3D, mouseX, "Mouse X: ", mouseX, mouseY, 1.0f, 0.0f, 0.0f, sentenceNumber);
	++sentenceNumber;

	// Convert the mouseY integer to string format.
	WRITE_SENTENCE(mD3D, mouseY, "Mouse Y: ", mouseX, mouseY + 20, 1.0f, 0.0f, 0.0f, sentenceNumber);
	++sentenceNumber;

	mCameraMovement->GetPosition(posX, posY, posZ);
	mCameraMovement->GetRotation(rotX, rotY, rotZ);
	if (mTerrainRenderType == Terrain::TerrainRenderType::RENDER_QUAD_TREE)
	{
		terrinDrawCount = mQuadTree->GetDrawCount();
	}

	// Truncate the position if it exceeds either 9999 or -9999.
	if(posX > 9999) { posX = 9999; }
	if(posY > 9999) { posY = 9999; }
	if(posZ > 9999) { posZ = 9999; }

	if(posX < -9999) { posX = -9999; }
	if(posY < -9999) { posY = -9999; }
	if(posZ < -9999) { posZ = -9999; }

	WRITE_SENTENCE(mD3D, posX, "X: ", textPosX, textPosY + 120, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, posY, "Y: ", textPosX, textPosY + 160, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, posZ, "Z: ", textPosX, textPosY + 200, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, rotX, "rX: ", textPosX, textPosY + 240, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, rotY, "rY: ", textPosX, textPosY + 280, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, rotZ, "rZ: ", textPosX, textPosY + 320, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

//	WRITE_SENTENCE(mD3D, terrinDrawCount, "terrain draw count: ", 800, 400, 0.0f, 1.0f, 1.0f, sentenceNumber);
//	++sentenceNumber;

	if (mDrawFuncTime != -1)
	{
		WRITE_SENTENCE(mD3D, mDrawFuncTime, "draw time: ", textPosX, textPosY + 360, 0.0f, 1.0f, 1.0f, sentenceNumber);
		++sentenceNumber;
	}

	// Render the text strings.
	result = m_Text->Render(mD3D->GetDeviceContext(), staticWorldMatrix, orthoMatrix);
	if(FAILED(result)) { return false; }
	return result;
}