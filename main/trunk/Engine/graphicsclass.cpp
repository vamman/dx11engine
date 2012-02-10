#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	mObjectFactory = 0;
	mMaterialFactory = 0;
	mTerrain = 0;
	mQuadTree = 0;

	mD3D = 0;
	mCamera = 0;
	mCameraMovement = 0;
	mBasicShader = 0;
	m_TextureShader = 0;
	mDirSpecLightShader = 0;
	mDirAmbLightShader = 0;
	mPointLightShader = 0;
	mDirSpecLight = 0;
	mTerrainShader = 0;

	m_Bitmap = 0;
	m_RenderTexture = 0;

	m_Text = 0;

	m_Frustum = 0;
	m_MultiTextureShader = 0;
	m_BumpMapShader = 0;
	m_SpecMapShader = 0;
	m_SpecMapShaderNonInstanced = 0;
	m_FogShader = 0;
	m_ReflectionShader = 0;

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


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix; // !!!
	ModelObject* object = new ModelObject();
	mDrawFuncTime = -1;
	DWORD funcTime = -1;

	float cameraX, cameraY, cameraZ;
	char videoCard[128];
	int videoMemory;

	mTimer = Timer::GetInstance();
	mTimer->Initialize();
	Timer::GetInstance()->SetTimeA();

	// Create the Direct3D object.
	mD3D = new D3DClass;
	if(!mD3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = mD3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	mCamera = new CameraClass;
	if(!mCamera)
	{
		return false;
	}

	// Set the initial position of the camera.
	mCamera->Render();
	mCamera->GetViewMatrix(baseViewMatrix);

	// Set the initial position of the camera.
	// cameraX = 130.0f; // 0.0f
	// cameraY = 2.0f; // 2.0f
	// cameraZ = 115.0f; // -15.0f

	cameraX = 0.0f; // 0.0f
	cameraY = 2.0f; // 2.0f
	cameraZ = -15.0f; // -15.0f

	mCamera->SetPosition(cameraX, cameraY, cameraZ);

	// Create the terrain object.
	mTerrain = new Terrain;
	if(!mTerrain)
	{
		return false;
	}

	// Initialize the terrain object.
	result = mTerrain->Initialize(mD3D->GetDevice(), "Engine/data/textures/heightmap01.bmp", L"Engine/data/textures/dirt01.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	// Create the quad tree object.
	mQuadTree = new QuadTree;
	if(!mQuadTree)
	{
		return false;
	}

	// Initialize the quad tree object.
	result = mQuadTree->Initialize(mTerrain, mD3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the quad tree object.", L"Error", MB_OK);
		return false;
	}

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
	if(!m_Text)
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize(mD3D->GetDevice(), mD3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}


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
		return false;
	}


	for (int i = 0; i < 4; ++i)
	{
		char* prefix = "light_";
		char resultName[20];
		char number[5];
		itoa(i, number, 10);
		sprintf(resultName, "%s%s", prefix, number);

		mPointLights[i] = new LightClass;
		if(!mPointLights[i])
		{
			return false;
		}
		D3DXVECTOR4 color	 = pointLightInfos[i]->color;
		D3DXVECTOR3 position = pointLightInfos[i]->position;
		mPointLights[i]->SetDiffuseColor(color.x, color.y, color.z, color.w);
		mPointLights[i]->SetPosition(position.x, position.y, position.z);

		object = mObjectFactory->CreateOrdinaryModel(mD3D->GetDevice(), hwnd, resultName, "Engine/data/models/sphere.txt");
		object->SetMaterial(mMaterialFactory->GetMaterialByName("BlueFloor"));
		object->SetPosition(position);
	}

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(mD3D->GetDevice(), screenWidth, screenHeight);
	if(!result)
	{
		return false;
	}

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if(!m_Bitmap)
	{
		return false;
	}

	// Initialize the bitmap object.
	result = m_Bitmap->Initialize(mD3D->GetDevice(), screenWidth, screenHeight, L"Engine/data/textures/texture2.dds", 300, 225); // 256, 256
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "GraphicsClass::Initialize time: ");
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
	mMaterialFactory = new MaterialFactory();
	material = mMaterialFactory->CreateMaterial("NormalWithSpec");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/stone02.dds");
	if(!result)
	{
		return false;
	}

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/bump02.dds");
	if(!result)
	{
		return false;
	}

	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/spec02.dds");
	if(!result)
	{
		return false;
	}

	material->SetMaterialShader(m_SpecMapShader);

	// Create material "BlueFloor"

	material = mMaterialFactory->CreateMaterial("BlueFloor");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/blue01.dds");
	if(!result)
	{
		return false;
	}

	material->SetMaterialShader(mDirSpecLightShader);

	// Create material "TexturedFloor"

	material = mMaterialFactory->CreateMaterial("TexturedFloor");
	material->AppentTextureToMaterial(mD3D->GetDevice(), L"Engine/data/textures/stone02.dds");
	if(!result)
	{
		return false;
	}

	material->SetMaterialShader(mDirAmbLightShader);

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitMaterials time: ");
	}

	return true;
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
	mDirAmbLight->SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	mDirAmbLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	mDirAmbLight->SetDirection(0.0f, 0.0f, 0.75f); //  -0.5f, -1.0f, 0.0f

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitLights time: ");
	}

	return true;
}

bool GraphicsClass::InitializeShaders(HWND hwnd)
{
	bool result;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	mBasicShader = new BasicShader;
	if(!mBasicShader)
	{
		return false;
	}

	// Initialize the basic shader object.
	result = mBasicShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/BasicShader.fx",
		"BasicVertexShader", "BasicPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShader;
	if(!m_TextureShader)
	{
		return false;
	}
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/TextureShaderNonInstanced.fx",
		"TextureVertexShader", "TexturePixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create directional specular light shader object.
	mDirSpecLightShader = new LightShader;
	if(!mDirSpecLightShader)
	{
		return false;
	}

	// Initialize directional light shader object.
	result = mDirSpecLightShader->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SpecularLight.fx",
		"LightVertexShader", "LightPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create directional ambient light shader object.
	mDirAmbLightShader = new LightShader;
	if(!mDirAmbLightShader)
	{
		return false;
	}

	// Initialize directional light shader object.
	result = mDirAmbLightShader->Initialize(mDirAmbLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/AmbientLight.fx",
		"LightVertexShader", "LightPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create terrain shader
	mTerrainShader = new TerrainShader;
	if(!mTerrainShader)
	{
		return false;
	}
	result = mTerrainShader->Initialize(mDirAmbLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/Terrain.fx",
		"TerrainVertexShader", "TerrainPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create directional light shader object.
	mPointLightShader = new LightShader;
	if(!mDirSpecLightShader)
	{
		return false;
	}

	// Initialize directional light shader object.
	/*
	result = mPointLightShader->Initialize(mPointLights[0], mD3D->GetDevice(), hwnd, L"Engine/data/shaders/PointLight.fx",
		"LightVertexShader", "LightPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	*/
	// Create the multitexture shader object.
	m_MultiTextureShader = new MultitextureShader;
	if(!m_MultiTextureShader)
	{
		return false;
	}

	// Initialize the multitexture shader object.
	result = m_MultiTextureShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/LightmapShader.fx",
		"LightMapVertexShader", "LightMapPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the multitexture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the bump map shader object.
	m_BumpMapShader = new NormalMapShader;
	if(!m_BumpMapShader)
	{
		return false;
	}

	// Initialize the bump map shader object.
	result = m_BumpMapShader->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/NormalMapShader.fx",
		"BumpMapVertexShader", "BumpMapPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the specular map shader object.
	m_SpecMapShader = new SpecMapShader;
	if(!m_SpecMapShader)
	{
		return false;
	}

	// Initialize the specular map shader object.
	result = m_SpecMapShader->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SpecMapShader.fx",
		"SpecMapVertexShader", "SpecMapPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the specular map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the specular map shader object.
	
	m_SpecMapShaderNonInstanced = new SpecMapShader();
	if(!m_SpecMapShader)
	{
		return false;
	}

	// Initialize the specular map shader object.
	result = m_SpecMapShaderNonInstanced->Initialize(mDirSpecLight, mD3D->GetDevice(), hwnd, L"Engine/data/shaders/SpecMapShaderNonInstanced.fx",
		"SpecMapVertexShader", "SpecMapPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the specular map shader object.", L"Error", MB_OK);
		return false;
	}
	
	// Create the fog shader object.
	m_FogShader = new FogShader;
	if(!m_FogShader)
	{
		return false;
	}

	// Initialize the fog shader object.
	result = m_FogShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/FogShader.fx",
		"FogVertexShader", "FogPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the fog shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the reflection shader object.
	m_ReflectionShader = new ReflectionShader;
	if(!m_ReflectionShader)
	{
		return false;
	}

	// Initialize the reflection shader object.
	result = m_ReflectionShader->Initialize(mD3D->GetDevice(), hwnd, L"Engine/data/shaders/ReflectionShader.fx", 
											"ReflectionVertexShader", "ReflectionPixelShader");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
		return false;
	}

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitializeShaders time: ");
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
	if(!mObjectFactory)
	{
		return false;
	}

	//  Create instanced sphere

	object = mObjectFactory->CreateInstancedModel(mD3D->GetDevice(), hwnd, "instancedSphere", "Engine/data/models/sphere.txt", 5); // sphere cube
	object->SetMaterial(mMaterialFactory->GetMaterialByName("NormalWithSpec"));

	// Create floor

	object = mObjectFactory->CreateOrdinaryModel(mD3D->GetDevice(), hwnd, "floor", "Engine/data/models/floor.txt");
	object->SetMaterial(mMaterialFactory->GetMaterialByName("TexturedFloor"));
	mObjectFactory->SetPositionForObject(D3DXVECTOR3(130.0f, 0.0f, 132.0f), "floor");

	// Create 10 ordinary spheres
	for (int i = 0; i < 5; ++i)
	{
		char* prefix = "sphere_";
		char resultName[20];
		char number[5];
		itoa(i, number, 10);
		sprintf(resultName, "%s%s", prefix, number);

		object = mObjectFactory->CreateOrdinaryModel(mD3D->GetDevice(), hwnd, resultName, "Engine/data/models/sphere.txt");
		object->SetMaterial(mMaterialFactory->GetMaterialByName("NormalWithSpec"));

		// Generate a random position in front of the viewer for the mode.
		float positionX = 130.0f;
		float positionY = 0.0f;
		float positionZ = 130.0f;

		// Generate a random position in front of the viewer for the mode.
		positionX += (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		positionY += (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		positionZ += ((((float)rand()-(float)rand())/RAND_MAX) * 10.0f) + 5.0f;

		// Load the instance array with data.
		D3DXVECTOR3 posVector = D3DXVECTOR3(positionX, positionY, positionZ);

		object->SetPosition(posVector);
	}

	// Create cube

	object = mObjectFactory->CreateOrdinaryModel(mD3D->GetDevice(), hwnd, "cube", "Engine/data/models/cube.txt");
	object->SetPosition(D3DXVECTOR3(130.0f, 1.0f, 130.0f));
	object->SetMaterial(mMaterialFactory->GetMaterialByName("NormalWithSpec"));

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteToLogFile(funcTime, "	GraphicsClass::InitObjects time: ");
	}
}


void GraphicsClass::Shutdown()
{
	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the render to texture object.
	if(m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the bitmap object.
	if(m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

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

	// Release the model list object.
	if(mObjectFactory)
	{
		mObjectFactory->Shutdown();
		delete mObjectFactory;
		mObjectFactory = 0;
	}

	if(mMaterialFactory)
	{
		mMaterialFactory->Shutdown();
		delete mMaterialFactory;
		mMaterialFactory = 0;
	}

	// Release the quad tree object.
	if(mQuadTree)
	{
		mQuadTree->Shutdown();
		delete mQuadTree;
		mQuadTree = 0;
	}

	// Release the terrain object.
	if(mTerrain)
	{
		mTerrain->Shutdown();
		delete mTerrain;
		mTerrain = 0;
	}

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

	if(mD3D)
	{
		mD3D->Shutdown();
		delete mD3D;
		mD3D = 0;
	}
	return;
}

void GraphicsClass::ShutdownShaders()
{
	// Release the reflection shader object.
	if(m_ReflectionShader)
	{
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = 0;
	}

	// Release the fog shader object.
	if(m_FogShader)
	{
		m_FogShader->Shutdown();
		delete m_FogShader;
		m_FogShader = 0;
	}

	// Release the terrain shader object.
	if(mTerrainShader)
	{
		mTerrainShader->Shutdown();
		delete mTerrainShader;
		mTerrainShader = 0;
	}

	// Release the multitexture shader object.
	if(m_MultiTextureShader)
	{
		m_MultiTextureShader->Shutdown();
		delete m_MultiTextureShader;
		m_MultiTextureShader = 0;
	}

	// Release the light shader object.
	if(mDirSpecLightShader)
	{
		mDirSpecLightShader->Shutdown();
		delete mDirSpecLightShader;
		mDirSpecLightShader = 0;
	}

	if(mDirAmbLightShader)
	{
		mDirAmbLightShader->Shutdown();
		delete mDirAmbLightShader;
		mDirAmbLightShader = 0;
	}
	
	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the color shader object.
	if(mBasicShader)
	{
		mBasicShader->Shutdown();
		delete mBasicShader;
		mBasicShader = 0;
	}

	// Release the bump map shader object.
	if(m_BumpMapShader)
	{
		m_BumpMapShader->Shutdown();
		delete m_BumpMapShader;
		m_BumpMapShader = 0;
	}

	// Release the specular map shader object.
	if(m_SpecMapShader)
	{
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = 0;
	}
}


bool GraphicsClass::Frame()
{
	bool result;
	result = InputClass::GetInstance()->Frame();
	if(!result)
	{
		return false;
	}

	if (InputClass::GetInstance()->IsEscapePressed() == true)
	{
		return false;
	}

	mTimer->Frame();
	FpsClass::GetInstance()->Frame();
	CpuClass::GetInstance()->Frame();

	// Do the frame input processing.
	result = HandleInput(mTimer->GetTime());
	if(!result)
	{
		return false;
	}

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::HandleInput(float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

	if (InputClass::GetInstance()->IsWireframeModeOn())
	{
		SetFillMode(D3D11_FILL_WIREFRAME);
	}
	else
	{
		SetFillMode(D3D11_FILL_SOLID);
	}

	// Set the frame time for calculating the updated position.
	mCameraMovement->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = InputClass::GetInstance()->IsLeftPressed();
	mCameraMovement->TurnLeft(keyDown);

	keyDown = InputClass::GetInstance()->IsRightPressed();
	mCameraMovement->TurnRight(keyDown);

	keyDown = InputClass::GetInstance()->IsUpPressed();
	mCameraMovement->MoveForward(keyDown);

	keyDown = InputClass::GetInstance()->IsDownPressed();
	mCameraMovement->MoveBackward(keyDown);

	keyDown = InputClass::GetInstance()->IsAPressed();
	mCameraMovement->MoveUpward(keyDown);

	keyDown = InputClass::GetInstance()->IsZPressed();
	mCameraMovement->MoveDownward(keyDown);

	keyDown = InputClass::GetInstance()->IsPgUpPressed();
	mCameraMovement->LookUpward(keyDown);

	keyDown = InputClass::GetInstance()->IsPgDownPressed();
	mCameraMovement->LookDownward(keyDown);
	
	// Get the view point position/rotation.
	mCameraMovement->GetPosition(posX, posY, posZ);
	mCameraMovement->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	mCamera->SetPosition(posX, posY, posZ);
	mCamera->SetRotation(rotX, rotY, rotZ);

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
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	mD3D->GetDeviceContext()->RSSetState(m_rasterState);
	return true;
}


bool GraphicsClass::Render()
{
	bool result;
	float  fogColor, fogStart, fogEnd;

	// Set the color of the fog.
	fogColor = 0.5f;
	fogStart = 0.0f;
	fogEnd = 10.0f;

	// For camera positioning above terrain
	bool foundHeight = false;
	D3DXVECTOR3 position;
	float height;

	Timer::GetInstance()->SetTimeA();

	// Get the current position of the camera.
	position = mCamera->GetPosition();

	// Get the height of the triangle that is directly underneath the given camera position.
	foundHeight =  mQuadTree->GetHeightAtPosition(position.x, position.z, height);
	if(foundHeight)
	{
		// If there was a triangle under the camera then position the camera just above it by two units.
		mCamera->SetPosition(position.x, height + 2.0f, position.z);
	}

	// Render the entire scene to the texture first.
	// result = RenderToTextureFromCameraView();
	result = RenderToTextureFromReflectionView();
	if(!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	mD3D->BeginScene(fogColor, fogColor, fogColor, 1.0);

	// Render the scene as normal to the back buffer.
	result = RenderScene();
	if(!result)
	{
		return false;
	}

	Render2D();

	// Present the rendered scene to the screen.
	mD3D->EndScene();
	Timer::GetInstance()->SetTimeB();
	mDrawFuncTime = Timer::GetInstance()->GetDeltaTime();
	return true;
}

bool GraphicsClass::Render2D()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix, staticWorldMatrix;
	bool result;

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	staticWorldMatrix = worldMatrix;
	mD3D->GetProjectionMatrix(projectionMatrix);

	// We now also get the ortho matrix from the D3DClass for 2D rendering. We will pass this in instead of the projection matrix.
	mD3D->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Get the world, view, and ortho matrices from the camera and d3d objects.
	mD3D->GetWorldMatrix(worldMatrix);
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetOrthoMatrix(orthoMatrix);

	// Put the bitmap vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Bitmap->Render(mD3D->GetDeviceContext(), 0, 0);
	if(!result)
	{
		return false;
	}

	vector<ID3D11ShaderResourceView*> textureArray;
	textureArray.push_back(m_RenderTexture->GetShaderResourceView());

	// Render the bitmap with the texture shader.
	m_TextureShader->SetTextureArray(mD3D->GetDeviceContext(), textureArray);
	result = m_TextureShader->RenderOrdinary(mD3D->GetDeviceContext(),
		m_Bitmap->GetIndexCount(),
		worldMatrix,
		viewMatrix, 
		orthoMatrix);
	if(!result)
	{
		return false;
	}

	// Turn on the alpha blending before rendering the text.
	mD3D->TurnOnAlphaBlending();

	RenderText();

	// Turn off alpha blending after rendering the text.
	mD3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	return true;
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
	if(!result)
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	return true;
}

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

	RenderModel(worldMatrix, viewMatrix, projectionMatrix, fogStart, fogEnd);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	return true;
}
bool GraphicsClass::RenderScene()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix, staticWorldMatrix, reflectionMatrix;
	int modelCount;
	float fogColor, fogStart, fogEnd;
	D3DXVECTOR4 color;
	bool result;
	static float rotation = 0.0f;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();
	Material* material;

	// Set the color of the fog to grey.
	fogColor = 0.5f;

	// Set the start and end of the fog.
	fogStart = 0.0f;
	fogEnd = 10.0f;


	// Generate the view matrix based on the camera's position.
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(viewMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	staticWorldMatrix = worldMatrix;
	mD3D->GetProjectionMatrix(projectionMatrix);

	// We now also get the ortho matrix from the D3DClass for 2D rendering. We will pass this in instead of the projection matrix.
	mD3D->GetOrthoMatrix(orthoMatrix);

	//////////////////////////////////////////////////////////////////////////
	// 3D Rendering
	//////////////////////////////////////////////////////////////////////////
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

	RenderTerrain(worldMatrix, viewMatrix, projectionMatrix);
	RenderModel(worldMatrix, viewMatrix, projectionMatrix, fogStart, fogEnd);
	
	ModelObject* modelObj = mObjectFactory->GetObjectByName("floor");

	float height;
	// Set psition above terrain
	bool foundHeight =  mQuadTree->GetHeightAtPosition(modelObj->GetPosition().x, modelObj->GetPosition().z, height);
	if(foundHeight)
	{
		// If there was a triangle under the camera then position the camera just above it by two units.
		D3DXVECTOR3 newPosition = D3DXVECTOR3(modelObj->GetPosition().x, height + 0.1f, modelObj->GetPosition().z);
		modelObj->SetPosition(newPosition);
	}

	modelObj->GetModel()->RenderOrdinary(deviceContext);
	worldMatrix = modelObj->GetWorldMatrix();

	material = modelObj->GetMaterial();
	material->GetMaterialShader()->SetTextureArray(deviceContext, material->GetTextureVector());
	material->GetMaterialShader()->SetCameraPosition(deviceContext, mCamera->GetPosition(), LightClass::DIRECTIONAL_AMBIENT_LIGHT);
	material->GetMaterialShader()->SetLightSource(deviceContext, mDirAmbLight);
	result = material->GetMaterialShader()->RenderOrdinary(deviceContext,
		modelObj->GetModel()->GetIndexCount(),
		worldMatrix,
		viewMatrix, 
		projectionMatrix);
	/*
	result = m_ReflectionShader->RenderOrdinary(deviceContext, modelObj->GetModel()->GetIndexCount(), worldMatrix,
		viewMatrix,	projectionMatrix, modelObj->GetMaterial()->GetTextureVector(), 
		m_RenderTexture->GetShaderResourceView(), reflectionMatrix);
	*/
	if(!result)
	{
		return false;
	}
	return true;
}

bool GraphicsClass::RenderTerrain(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	bool result = true;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

	// Render the terrain buffers.
	vector<ID3D11ShaderResourceView*> texArr;
	texArr.push_back(mTerrain->GetTexture());

	// Render the terrain using the terrain shader.
	
	mTerrainShader->SetTextureArray(deviceContext, texArr);
	mTerrainShader->SetLightSource(deviceContext, mDirAmbLight);
	result = mTerrainShader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(!result)
	{
		return false;
	}

	// Render the terrain using the quad tree and terrain shader.
	mQuadTree->Render(m_Frustum, deviceContext, mTerrainShader);
	
	return result;
}

bool GraphicsClass::RenderModel(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, float fogStart, float fogEnd)
{
	bool result, renderModel;
	D3DXMATRIX reflectionMatrix, rotationMatrix;
	reflectionMatrix = mCamera->GetReflectionViewMatrix();
	static float rotation = 0.0f;
	static float dirDelta = 0.0f;
	mNumObjectsRendered = 0;

	ModelClass* model;
	ModelObject* modelObj;
	D3DXVECTOR3 posVector;

	vector<ModelObject*> listOfModels = mObjectFactory->GetVectorOfObjects();
	vector<ModelObject*>::iterator modelIt;
	ID3D11DeviceContext* deviceContext = mD3D->GetDeviceContext();

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
			float height;
			// Set psition above terrain
			bool foundHeight =  mQuadTree->GetHeightAtPosition(modelObj->GetPosition().x, modelObj->GetPosition().z, height);
			if(foundHeight)
			{
				// If there was a triangle under the camera then position the camera just above it by two units.
				D3DXVECTOR3 newPosition = D3DXVECTOR3(modelObj->GetPosition().x, height + 1, modelObj->GetPosition().z);
				modelObj->SetPosition(newPosition);
			}

			model->RenderInstanced(deviceContext);
			worldMatrix = modelObj->GetWorldMatrix();
			//  m_SpecMapShader // material->GetMaterialShader()
			material->GetMaterialShader()->SetTextureArray(deviceContext, textureVector);
			material->GetMaterialShader()->SetCameraPosition(deviceContext, mCamera->GetPosition(), LightClass::DIRECTIONAL_SPECULAR_LIGHT);
			material->GetMaterialShader()->SetLightSource(deviceContext, mDirSpecLight);
			result = material->GetMaterialShader()->RenderInstanced(deviceContext,
				model->GetVertexCount(),
				model->GetInstanceCount(),
				worldMatrix,
				viewMatrix, 
				projectionMatrix);

			if(!result)
			{
				return false;
			}
		}
		// Render non-instanced objects
		else if (!modelObj->IsInstanced())
		{			
			if (strcmp(modelObj->GetModelName(), "cube") == 0)
			{
				// Update the rotation variable each frame.
				rotation += (float)D3DX_PI * 0.00099f;
				if(rotation > 360.0f)
				{
					rotation -= 360.0f;
				}

				//modelObj->SetRotation(rotation);
				float height;
				// Set psition above terrain
				bool foundHeight =  mQuadTree->GetHeightAtPosition(modelObj->GetPosition().x, modelObj->GetPosition().z, height);
				if(foundHeight)
				{
					// If there was a triangle under the camera then position the camera just above it by two units.
					D3DXVECTOR3 newPosition = D3DXVECTOR3(modelObj->GetPosition().x, height + 1, modelObj->GetPosition().z);
					modelObj->SetPosition(newPosition);
				}

				model->RenderOrdinary(deviceContext);
				worldMatrix = modelObj->GetWorldMatrix();

				// material->GetMaterialShader() // m_SpecMapShaderNonInstanced
				material->GetMaterialShader()->SetTextureArray(deviceContext, textureVector);
				material->GetMaterialShader()->SetCameraPosition(deviceContext, mCamera->GetPosition(), LightClass::DIRECTIONAL_AMBIENT_LIGHT);
				material->GetMaterialShader()->SetLightSource(deviceContext, mDirAmbLight);
				result = material->GetMaterialShader()->RenderOrdinary(deviceContext,
					model->GetIndexCount(),
					worldMatrix,
					viewMatrix,
					projectionMatrix);
				if(!result)
				{
					return false;
				}
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
						dirDelta += 0.00019f;
						mDirSpecLight->SetDirection(0.0f, dirDelta, 1.0f);

						float height;
						// Set psition above terrain
						bool foundHeight =  mQuadTree->GetHeightAtPosition(modelObj->GetPosition().x, modelObj->GetPosition().z, height);
						if(foundHeight)
						{
							// If there was a triangle under the camera then position the camera just above it by two units.
							D3DXVECTOR3 newPosition = D3DXVECTOR3(modelObj->GetPosition().x, height + 1, modelObj->GetPosition().z);
							modelObj->SetPosition(newPosition);
						}
						worldMatrix = modelObj->GetWorldMatrix();
						
						model->RenderOrdinary(mD3D->GetDeviceContext());
						// material->GetMaterialShader() // m_SpecMapShaderNonInstanced
						material->GetMaterialShader()->SetTextureArray(deviceContext, textureVector);
						material->GetMaterialShader()->SetCameraPosition(deviceContext, mCamera->GetPosition(), LightClass::DIRECTIONAL_SPECULAR_LIGHT);
						material->GetMaterialShader()->SetLightSource(deviceContext, mDirSpecLight);
						result = material->GetMaterialShader()->RenderOrdinary(
																				deviceContext,
																				model->GetIndexCount(),
																				worldMatrix,
																				viewMatrix, 
																				projectionMatrix);
						if(!result)
						{
							return false;
						}
					}
					else if (strcmp(lightObjPrefix.c_str(), "light_") == 0)
					{
						modelObj->SetScale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
						worldMatrix = modelObj->GetWorldMatrix();
						model->RenderOrdinary(mD3D->GetDeviceContext());

						// material->GetMaterialShader() // mDirSpecLightShader
						material->GetMaterialShader()->SetTextureArray(deviceContext, textureVector);
						material->GetMaterialShader()->SetCameraPosition(deviceContext, mCamera->GetPosition(), LightClass::DIRECTIONAL_SPECULAR_LIGHT);
						material->GetMaterialShader()->SetLightSource(deviceContext, mDirSpecLight);
						result = material->GetMaterialShader()->RenderOrdinary(
																	deviceContext,
																	model->GetIndexCount(),
																	worldMatrix,
																	viewMatrix, 
																	projectionMatrix);
					}

					// Since this model was rendered then increase the count for this frame.
					mNumObjectsRendered++;
				}
			}
		}
	}
	return true;
}

bool GraphicsClass::RenderText()
{
	D3DXMATRIX orthoMatrix, worldMatrix, staticWorldMatrix;
	int mouseX, mouseY;
	int MAX_STRING_LENGTH = 30;
	char tempString[10];
	char fpsString[10];
	char countString[20];
	char cpuString[20];
	char mouseString[20];
	char darwCountString[35];
	char drawTimeString[25];
	bool result;

	SetFillMode(D3D11_FILL_SOLID);

	mD3D->GetOrthoMatrix(orthoMatrix);
	mD3D->GetWorldMatrix(worldMatrix);
	staticWorldMatrix = worldMatrix;

	// Convert the fps integer to string format.
	_itoa_s(FpsClass::GetInstance()->GetFps(), tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), fpsString, 800, 100, 0.0f, 0.0f, 1.0f, 0);

	// Convert the cpu integer to string format.
	_itoa_s(CpuClass::GetInstance()->GetCpuPercentage(), tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), cpuString, 800, 100 + 20, 1.0f, 0.0f, 1.0f, 1);

	// Convert the count integer to string format.
	_itoa_s(mNumObjectsRendered, tempString, 10);

	// Setup the render count string.
	strcpy_s(countString, "Render Count: ");
	strcat_s(countString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), countString, 800, 100 + 20 * 5, 1.0f, 0.0f, 0.0f, 2);
	if(!result)
	{
		return false;
	}

	// Convert the mouseX integer to string format.
	InputClass::GetInstance()->GetMouseLocation(mouseX, mouseY);
	_itoa_s(mouseX, tempString, 10);

	// Setup the mouseX string.
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), mouseString, mouseX, mouseY, 1.0f, 0.0f, 0.0f, 3);
	if(!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	_itoa_s(mouseY, tempString, 10);

	// Setup the mouseX string.
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), mouseString, mouseX, mouseY + 20, 1.0f, 0.0f, 0.0f, 4);
	if(!result)
	{
		return false;
	}

	char dataString[16];

	float posX, posY, posZ, rotX, rotY, rotZ;
	int terrinDrawCount;

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

	// Setup the X position string.
	_itoa_s((int)posX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), dataString, 800, 240, 1.0f, 0.0f, 1.0f, 5);
	if(!result)
	{
		return false;
	}

	// Setup the Y position string.
	_itoa_s((int)posY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), dataString, 800, 260, 1.0f, 0.0f, 1.0f, 6);
	if(!result)
	{
		return false;
	}

	// Setup the Z position string.
	_itoa_s((int)posZ, tempString, 10);
	strcpy_s(dataString, "Z: ");
	strcat_s(dataString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), dataString, 800, 280, 1.0f, 0.0f, 1.0f, 7);
	if(!result)
	{
		return false;
	}

	// Update the rotation values in the text object.
	// Setup the X rotation string.
	_itoa_s((int)rotX, tempString, 10);
	strcpy_s(dataString, "rX: ");
	strcat_s(dataString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), dataString, 800, 320, 0.0f, 1.0f, 1.0f, 8);
	if(!result)
	{
		return false;
	}

	_itoa_s((int)rotY, tempString, 10);
	strcpy_s(dataString, "rY: ");
	strcat_s(dataString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), dataString, 800, 340, 0.0f, 1.0f, 1.0f, 9);
	if(!result)
	{
		return false;
	}

	_itoa_s((int)rotZ, tempString, 10);
	strcpy_s(dataString, "rZ: ");
	strcat_s(dataString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), dataString, 800, 360, 0.0f, 1.0f, 1.0f, 10);
	if(!result)
	{
		return false;
	}
	
	_itoa_s((int)terrinDrawCount, tempString, 10);
	strcpy_s(darwCountString, "terrain draw count: ");
	strcat_s(darwCountString, tempString);

	result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), darwCountString, 800, 400, 0.0f, 1.0f, 1.0f, 11);
	if(!result)
	{
		return false;
	}
	

	if (mDrawFuncTime != -1)
	{
		_itoa_s(mDrawFuncTime, tempString, 10);
		strcpy_s(drawTimeString, "draw time: ");
		strcat_s(drawTimeString, tempString);

		result = m_Text->AddSentence(mD3D->GetDevice(), mD3D->GetDeviceContext(), drawTimeString, 800, 440, 0.0f, 1.0f, 1.0f, 12);
		if(!result)
		{
			return false;
		}
	}

	// Render the text strings.
	result = m_Text->Render(mD3D->GetDeviceContext(), staticWorldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}
	return true;
}