#include "graphicsclass.h"


GraphicsClass::GraphicsClass() : mIsAllowToBBRender(true), mIsAllowToCameraDisplayRender(true), mDirUp(true), 
								 mDirDown(false), mIsWireFrameModeOn(true), mSkyShape((SkyShape)0), mSkyPixelShaderType((SkyPixelShaderType)0)
{
	mObjectFactory = 0;
	mMaterialFactory = 0;
	mTerrain = 0;
	mQuadTree = 0;
	m_MiniMap = 0;

	mD3D = 0;
	mCamera = 0;
	mCameraMovement = 0;

	mBitmap = 0;
	mCursor = 0;
	m_RenderTexture = 0;

	m_Text = 0;

	m_Frustum = 0;

	// Shaders
	m_MultiTextureShader = 0;
	m_BumpMapShader = 0;
	m_SpecMapShader = 0;
	m_SpecMapShaderNonInstanced = 0;
	m_FogShader = 0;
	m_ReflectionShader = 0;
	mCursorShader = 0;
	mBasicShader = 0;
	mTextureShaderMiniMap = 0;
	mTextureShaderCamDisplay = 0;
	mDirSpecLightShader = 0;
	mDirAmbLightShader = 0;
	mPointLightShader = 0;
	mDirSpecLight = 0;
	mTerrainShader = 0;
	mSkyDomeShader = 0;

	mSkyDome = 0;

	for (int i = 0; i < 4; ++i)
	{
		mPointLights[i] = 0;
	}
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


HRESULT GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
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
	mD3D = new D3DClass;
	if(!mD3D)
	{
		return result;
	}

	// Initialize the Direct3D object.
	V_RETURN(mD3D->Initialize(mScreenWidth, mScreenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR), L"Error",
			 L"Could not initialize Direct3D");

	ID3D11Device* device = mD3D->GetDevice();

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
	cameraY = 2.0f;
	cameraZ = 110.0f;

	mCamera->SetPosition(cameraX, cameraY, cameraZ);

	// Create the terrain object.
	mTerrain = new Terrain;
	if(!mTerrain) { return result; }
	// heightmap01.bmp / heightmap513; dirt01.dds; colorm01.bmp / colorm513
	V_RETURN(mTerrain->Initialize(device, "Engine/data/textures/terrain/heightmap01.bmp", L"Engine/data/textures/terrain/dirt01.dds", "Engine/data/textures/terrain/colorm01.bmp"),
			 L"Error", L"Could not initialize the terrain object");
	
	// Create the quad tree object.
	mQuadTree = new QuadTree;
	if(!mQuadTree) { return false; }

	V_RETURN(mQuadTree->Initialize(mTerrain, device), L"Error",	L"Could not initialize the quad tree object");

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

	InitLights();
	InitializeShaders(hwnd);
	InitMaterials();
	InitObjects(hwnd);

	// Create the frustum object.
	m_Frustum = new FrustumClass;
	if(!m_Frustum)
	{
		return result;
	}

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

		object = mObjectFactory->CreateOrdinaryModel(device, hwnd, resultName, wstring(L"Engine/data/models/sphere.txt"));
		object->SetMaterial(mMaterialFactory->GetMaterialByName("BlueFloor"));
		object->SetPosition(position);
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
	V_RETURN(mBitmap->Initialize(device, mScreenWidth, mScreenHeight, L"Engine/data/textures/texture2.dds", 300, 225),
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
	V_RETURN(mCursor->Initialize(device, mScreenWidth, mScreenHeight, L"Engine/data/textures/SC2Cursor1.bmp", mCursorWidth / 2, mCursorHeight / 2),
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

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "GraphicsClass::Initialize time: ");
		Log::GetInstance()->WriteToOutput(funcTime, "GraphicsClass::Initialize time: ");
	}

	// Create the sky dome object.
	mSkyDome = new SkyDome;
	if(!mSkyDome) { return false; }
	V_RETURN(mSkyDome->Initialize(device, hwnd), L"Error",	L"Could not initialize the sky dome object");
	V_RETURN(mSkyDome->CreateCubeTexture(device), L"Error",	L"Could not initialize the sky dome cube map object");
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
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitLights time: ");
		Log::GetInstance()->WriteToOutput(funcTime, "	GraphicsClass::InitLights time: ");
	}

	return true;
}

HRESULT GraphicsClass::InitializeShaders(HWND hwnd)
{
	HRESULT result = S_FALSE;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Create basic shader
	mBasicShader = new BasicShader;
	if(!mBasicShader) { return result; }
	V_RETURN(mBasicShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/BasicShader.fx", "BasicVertexShader", "BasicPixelShader"),
			 L"Error", L"Could not initialize the basic shader object.");

	// Create the texture shader object for mini map.
	mTextureShaderMiniMap = new TextureShader;
	if(!mTextureShaderMiniMap) { return result; }
	V_RETURN(mTextureShaderMiniMap->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/TextureShaderNonInstanced.fx", "TextureVertexShader", "TexturePixelShader"),
			 L"Error", L"Could not initialize the texture shader object.");

	// Create the texture shader object for camera display.
	mTextureShaderCamDisplay = new TextureShader;
	if(!mTextureShaderCamDisplay) { return result; }
	V_RETURN(mTextureShaderCamDisplay->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/TextureShaderNonInstanced.fx", "TextureVertexShader", "TexturePixelShader"),
		L"Error", L"Could not initialize the texture shader object.");

	// Create directional specular light shader object.
	mDirSpecLightShader = new LightShader;
	if(!mDirSpecLightShader) { return result; }
	V_RETURN(mDirSpecLightShader->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SpecularLight.fx","LightVertexShader", "LightPixelShader"),
		L"Error", L"Could not initialize the light shader object.");

	// Create directional ambient light shader object.
	mDirAmbLightShader = new LightShader;
	if(!mDirAmbLightShader) { return result; }
	V_RETURN(mDirAmbLightShader->Initialize(mDirAmbLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/AmbientLight.fx", "LightVertexShader", "LightPixelShader"),
		L"Error", L"Could not initialize the light shader object.");

	// Create terrain shader
	mTerrainShader = new TerrainShader;
	if(!mTerrainShader)	{ return result; }
	V_RETURN(mTerrainShader->Initialize(mDirAmbLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/Terrain.fx", "TerrainVertexShader", "TerrainPixelShader"),
		L"Error", L"Could not initialize the terrain shader object.");

	// Create point light shader object.
	/*
	mPointLightShader = new LightShader;
	if(!mDirSpecLightShader) { return result; }
	V_RETURN(mPointLightShader->Initialize(mPointLights[0], mD3D->GetDevice(), hwnd, L"Engine/data/shaders/PointLight.fx", "LightVertexShader", "LightPixelShader"),
		L"Error", L"Could not initialize the point light shader object.");
	*/

	// Create the multitexture shader object.
	m_MultiTextureShader = new MultitextureShader;
	if(!m_MultiTextureShader) { return result; }
	V_RETURN(m_MultiTextureShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/LightmapShader.fx", "LightMapVertexShader", "LightMapPixelShader"),
		L"Error", L"Could not initialize the multitexture shader object.");

	// Create the bump map shader object.
	m_BumpMapShader = new NormalMapShader;
	if(!m_BumpMapShader) { return result; }
	V_RETURN(m_BumpMapShader->Initialize(mDirAmbLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/NormalMapShader.fx", "BumpMapVertexShader", "BumpMapPixelShader"),
		L"Error", L"Could not initialize the bump map shader object.");

	// Create the specular map shader object.
	m_SpecMapShader = new SpecMapShader;
	if(!m_SpecMapShader) { return result; }
	V_RETURN(m_SpecMapShader->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SpecMapShader.fx", "SpecMapVertexShader", "SpecMapPixelShader"),
		L"Error", L"Could not initialize the specular map instanced shader object.");

	// Create the specular map shader object.
	m_SpecMapShaderNonInstanced = new SpecMapShader();
	if(!m_SpecMapShader) { return result; }
	V_RETURN(m_SpecMapShaderNonInstanced->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SpecMapShaderNonInstanced.fx", "SpecMapVertexShader", "SpecMapPixelShader"),
		L"Error", L"Could not initialize the specular map non inctansed shader object.");
	
	// Create the fog shader object.
	m_FogShader = new FogShader;
	if(!m_FogShader) { return result; }
	V_RETURN(m_FogShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/FogShader.fx", "FogVertexShader", "FogPixelShader"),
		L"Error", L"Could not initialize the fog shader object.");

	// Create the reflection shader object.
	m_ReflectionShader = new ReflectionShader;
	if(!m_ReflectionShader) { return result; }
	V_RETURN(m_ReflectionShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/ReflectionShader.fx", "ReflectionVertexShader", "ReflectionPixelShader"),
		L"Error", L"Could not initialize the reflection shader object.");

	// Create cursor shader object
	mCursorShader = new FontShader;
	if(!mCursorShader) { return result; }
	V_RETURN(mCursorShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/CursorShader.fx",  "CursorVertexShader", "CursorPixelShader"),
		L"Error", L"Could not initialize the cursor shader object.");

	// Create the sky dome shader object.
	mSkyDomeShader = new SkyDomeShader;
	if(!mSkyDomeShader) { return false; }
	V_RETURN(mSkyDomeShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SkyDomeShader.fx",  "SkyDomeVertexShader", "SkyDomePixelShader"),
		L"Error", L"Could not initialize the sky dome shader object.");


	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitializeShaders time: ");
		Log::GetInstance()->WriteToOutput(funcTime, "	GraphicsClass::InitializeShaders time: ");
	}

	return result;
}

bool GraphicsClass::InitMaterials()
{
	Material* material = new Material("defaultMaterial");
	bool result = true;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Create material "NormalWithSpec"
	mMaterialFactory = new MaterialFactory();
	material = mMaterialFactory->CreateMaterial("NormalWithSpec");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/stone02.dds");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/bump02.dds");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/spec02.dds");
	if(!result) { return false; }

	material->SetMaterialShader(m_SpecMapShader);

	// Create material "BlueFloor"
	material = mMaterialFactory->CreateMaterial("BlueFloor");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/blue01.dds");
	if(!result) { return false; }

	material->SetMaterialShader(mDirSpecLightShader);

	// Create material "TexturedFloor"
	material = mMaterialFactory->CreateMaterial("TexturedFloor");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/stone02.dds");
	if(!result) { return false; }

	material->SetMaterialShader(mDirAmbLightShader);

	// Create normal map material for space compound
	material = mMaterialFactory->CreateMaterial("spaceCompoundMaterial");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/stone01.dds");
	if(!result) { return false; }

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/bump01.dds");
	if(!result) { return false; }

	material->SetMaterialShader(m_BumpMapShader);

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitMaterials time: ");
		Log::GetInstance()->WriteToOutput(funcTime, "	GraphicsClass::InitMaterials time: ");
	}

	return true;
}

bool GraphicsClass::InitObjects(HWND hwnd)
{
	ModelObject* object = new ModelObject();
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Create the model factory object.
	mObjectFactory = new ModelFactory;
	if(!mObjectFactory) { return false; }

	//  Create instanced sphere
	CREATE_INSTANCED_OBJ_WITH_MAT("instancedSphere", wstring(L"Engine/data/models/sphere.txt"), "NormalWithSpec", 5)

	// Create floor
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "floor", wstring(L"Engine/data/models/floor.txt"), "TexturedFloor");
	object->SetPosition(D3DXVECTOR3(130.0f, 0.0f, 132.0f)); 

	// Create 10 ordinary spheres
	for (int i = 0; i < 5; ++i)
	{
		char* prefix = "sphere_";
		char resultName[20];
		char number[5];
		_itoa_s(i, number, 10);
		sprintf_s(resultName, "%s%s", prefix, number);

		CREATE_ORDINARY_OBJ_WITH_MAT(object, resultName, wstring(L"Engine/data/models/sphere.txt"), "NormalWithSpec");

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
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "cube", wstring(L"Engine/data/models/cube.txt"), "NormalWithSpec");
	object->SetPosition(D3DXVECTOR3(130.0f, 1.0f, 130.0f)); // 130.0f, 1.0f, 130.0f
	

	// Create space compound
	// /spaceCompound.obj
	CREATE_ORDINARY_OBJ_WITH_MAT(object, "spaceCompound", wstring(L"Engine/data/models/spaceCompound.txt"), "spaceCompoundMaterial"); // spaceCompoundMaterial // NormalWithSpec
	object->SetPosition(D3DXVECTOR3(130.0f, 0.0f, 132.0f)); 

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitObjects time: ");
		Log::GetInstance()->WriteToOutput(funcTime, "	GraphicsClass::InitObjects time: ");
	}
	return true;
}


void GraphicsClass::Shutdown()
{
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

	ShutdownShaders();

	// Release the frustum object.
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	SHUTDOWN_OBJ(mObjectFactory);
	SHUTDOWN_OBJ(mMaterialFactory);
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

	SHUTDOWN_OBJ(mD3D);
	return;
}

void GraphicsClass::ShutdownShaders()
{
	// Release the reflection shader object.
	SHUTDOWN_OBJ(m_ReflectionShader);
	// Release the fog shader object.
	SHUTDOWN_OBJ(m_FogShader);
	// Release the terrain shader object.
	SHUTDOWN_OBJ(mTerrainShader);
	// Release the multitexture shader object.
	SHUTDOWN_OBJ(m_MultiTextureShader);
	// Release the directional specular light shader object.
	SHUTDOWN_OBJ(mDirSpecLightShader);
	// Release the directional ambient light shader object.
	SHUTDOWN_OBJ(mDirAmbLightShader);	
	// Release the texture shader object for mini map.
	SHUTDOWN_OBJ(mTextureShaderMiniMap);
	// Release the texture shader object for camera display.
	SHUTDOWN_OBJ(mTextureShaderCamDisplay);
	// Release the color shader object.
	SHUTDOWN_OBJ(mBasicShader);
	// Release the bump map shader object.
	SHUTDOWN_OBJ(m_BumpMapShader);
	// Release the specular map shader object.
	SHUTDOWN_OBJ(m_SpecMapShader);
	// Release the cursor shader object.
	SHUTDOWN_OBJ(mCursorShader);
	// Release the sky dome shader object.
	SHUTDOWN_OBJ(mSkyDomeShader);
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

	if (InputClass::GetInstance()->IsWireframeModeOn(mIsWireFrameModeOn))
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
	mIsAllowToBBRender = InputClass::GetInstance()->IsAllowToBBRender();

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
	foundHeight =  mQuadTree->GetHeightAtPosition(position.x, position.z, height);
	if(foundHeight)
	{
		// If there was a triangle under the camera then position the camera just above it by two units.
		mCamera->SetPosition(position.x, height + 2.0f, position.z);
	}

	//result = RenderToTextureFromReflectionView(); // TODO

	// Render the entire scene to the texture first.
	mIsAllowToCameraDisplayRender = InputClass::GetInstance()->IsAllowToCameraDisplayRender();
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
	modelCount = mObjectFactory->GetModelCount();

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
	mSkyDomeShader->Render(deviceContext, mSkyDome->GetIndexCount(mSkyShape), worldMatrix, viewMatrix, projectionMatrix,
						   mSkyDome->GetApexColor(), mSkyDome->GetCenterColor(), (float) mSkyPixelShaderType);

	// Turn back face culling back on.
	mD3D->TurnOnCulling();

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
	
	RenderTerrain(worldMatrix, viewMatrix, projectionMatrix);
	RenderModel(worldMatrix, viewMatrix, projectionMatrix, fogStart, fogEnd);
	
	ModelObject* modelObj = mObjectFactory->GetObjectByName("floor");
	SetPositionAboveTerrain(modelObj, 0.1f);
	RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirAmbLight, LightClass::DIRECTIONAL_AMBIENT_LIGHT, false);

	return true;
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
		mTextureShaderCamDisplay->SetTextureArray(deviceContext, textureArray);
		result = mTextureShaderCamDisplay->RenderOrdinary(deviceContext,
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
	result = m_MiniMap->Render(deviceContext, worldMatrix, orthoMatrix, mTextureShaderMiniMap, cameraRotY);
	if(FAILED(result)) { return result; }

	// Render cursor object
	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = mCursor->Render(deviceContext, mScreenWidth / 2 - mCursorWidth / 4, mScreenHeight / 2 - mCursorHeight / 4 );
	if(FAILED(result)) { return result; }

	D3DXVECTOR4 pixelColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	vector<ID3D11ShaderResourceView*> textureArray;
	textureArray.push_back(mCursor->GetTexture());

	// Render the bitmap with the texture shader.
	mCursorShader->SetTextureArray(mD3D->GetDeviceContext(), textureArray);
	mCursorShader->SetPixelBufferColor(deviceContext, pixelColor);
	result = mCursorShader->RenderOrdinary(deviceContext,
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

	//RenderModel(worldMatrix, viewMatrix, projectionMatrix, fogStart, fogEnd);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	return true;
}

HRESULT GraphicsClass::RenderTerrain(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result = true;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

	// Render the terrain buffers.
	vector<ID3D11ShaderResourceView*> texArr;
	texArr.push_back(mTerrain->GetTexture());

	// Render the terrain using the terrain shader.
	mTerrainShader->SetTextureArray(deviceContext, texArr);
	mTerrainShader->SetLightSource(deviceContext, mDirAmbLight);
	result = mTerrainShader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(FAILED(result)) { return result; }
	// Render the terrain using the quad tree and terrain shader.
	mQuadTree->Render(m_Frustum, deviceContext, mTerrainShader, mIsAllowToBBRender);
	return result;
}

bool GraphicsClass::RenderModel(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, float fogStart, float fogEnd)
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

	vector<ModelObject*> listOfModels = mObjectFactory->GetVectorOfObjects();
	vector<ModelObject*>::iterator modelIt;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

	float dirDeltaStep = 0.01f;
	if (mDirUp) { dirDelta += dirDeltaStep; }
	if (mDirDown) { dirDelta -= dirDeltaStep; }

	if (dirDelta > 1.0f) { mDirUp   = false; mDirDown = true; }
	if (dirDelta < -1.0f) { mDirUp   = true; mDirDown = false; }
	
	mDirSpecLight->SetDirection(0.0f, dirDelta, 1.0f);
	mDirAmbLight->SetDirection(0.0f, dirDelta, 1.0f);

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.00099f;
	if(rotation > 360.0f) { rotation -= 360.0f; }

	for (modelIt = listOfModels.begin(); modelIt != listOfModels.end(); ++modelIt)
	{
		modelObj = (*modelIt);
		model = modelObj->GetModel();
		Material* material = modelObj->GetMaterial();
		vector<ID3D11ShaderResourceView*> textureVector = material->GetTextureVector();
		ID3D11ShaderResourceView** textureArray = &textureVector[0];

		// Render instanced objects
		if (modelObj->IsInstanced())
		{
			SetPositionAboveTerrain(modelObj, 1.0f);
			RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirSpecLight, LightClass::DIRECTIONAL_SPECULAR_LIGHT, true);
		}
		// Render non-instanced objects
		else if (!modelObj->IsInstanced())
		{		
			if (strcmp(modelObj->GetModelName(), "cube") == 0)
			{
				modelObj->SetRotation(rotation);
				SetPositionAboveTerrain(modelObj, 1.0f);
				RenderObject(modelObj, deviceContext, viewMatrix, projectionMatrix, mDirSpecLight, LightClass::DIRECTIONAL_AMBIENT_LIGHT, false);
			}
			// Draw spaceCompound
			else if (strcmp(modelObj->GetModelName(), "spaceCompound") == 0)
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
						SetPositionAboveTerrain(modelObj, radius);
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
		model->RenderOrdinary(deviceContext);
		result = materialShader->RenderOrdinary(deviceContext,
			model->GetIndexCount(),
			worldMatrix,
			viewMatrix,
			projectionMatrix);
		if(FAILED(result)) { return result; }
	}
	else
	{
		model->RenderInstanced(deviceContext);
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

	// SetFillMode(D3D11_FILL_SOLID);

	// Generate the view matrix based on the camera's position.
	mCamera->Render();
	mD3D->GetOrthoMatrix(orthoMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	staticWorldMatrix = worldMatrix;

	// Convert the fps integer to string format.
	WRITE_SENTENCE(mD3D, FpsClass::GetInstance()->GetFps(), "FPS: ", 800, 100, 0.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	// Convert the cpu integer to string format.
	_itoa_s(CpuClass::GetInstance()->GetCpuPercentage(), tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	result = m_Text->AddSentence(mD3D, cpuString, 800, 100 + 20, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	// Convert the count integer to string format.
	WRITE_SENTENCE(mD3D, mNumObjectsRendered, "Render Count: ", 800, 100 + 20 * 5, 1.0f, 0.0f, 0.0f, sentenceNumber);
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
	terrinDrawCount = mQuadTree->GetDrawCount();

	// Truncate the position if it exceeds either 9999 or -9999.
	if(posX > 9999) { posX = 9999; }
	if(posY > 9999) { posY = 9999; }
	if(posZ > 9999) { posZ = 9999; }

	if(posX < -9999) { posX = -9999; }
	if(posY < -9999) { posY = -9999; }
	if(posZ < -9999) { posZ = -9999; }

	WRITE_SENTENCE(mD3D, posX, "X: ", 800, 240, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, posY, "Y: ", 800, 260, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, posZ, "Z: ", 800, 280, 1.0f, 0.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, rotX, "rX: ", 800, 320, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, rotY, "rY: ", 800, 340, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, rotZ, "rZ: ", 800, 360, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	WRITE_SENTENCE(mD3D, terrinDrawCount, "terrain draw count: ", 800, 400, 0.0f, 1.0f, 1.0f, sentenceNumber);
	++sentenceNumber;

	if (mDrawFuncTime != -1)
	{
		WRITE_SENTENCE(mD3D, mDrawFuncTime, "draw time: ", 800, 440, 0.0f, 1.0f, 1.0f, sentenceNumber);
		++sentenceNumber;
	}

	// Render the text strings.
	result = m_Text->Render(mD3D->GetDeviceContext(), staticWorldMatrix, orthoMatrix);
	if(FAILED(result)) { return false; }
	return result;
}