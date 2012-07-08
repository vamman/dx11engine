// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Terrain.h"
#include "graphicsclass.h"
#include "ResourceMgr.h"
#include "FileSystemHelper.h"

Terrain::Terrain()
{
	m_heightMap = 0;
	m_Texture = 0;
	m_DetailTexture = 0;
	m_vertices = 0;
	m_Textures = 0;
	m_Materials = 0;
}

Terrain::Terrain(const Terrain& other)
{
}

Terrain::~Terrain()
{
}

bool Terrain::InitializeWithQuadTree(ID3D11Device* device, char* heightMapFileName, char* textureFilename,
									 char* colorMapFilename)
{
	bool result;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Load in the height map for the terrain.
	ASSERT(LoadHeightMap(device, heightMapFileName), L"Terrain::LoadHeightMap FAILED");

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	ASSERT(CalculateNormals(), L"Terrain::CalculateNormals FAILED");

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Load the texture.
	m_Texture = reinterpret_cast<Texture* >(ResourceMgr::GetInstance()->GetResourceByName(textureFilename, ResourceMgr::ResourceTypeTexture));

	// ASSERT(LoadTexture(device, textureFilename), L"Terrain::LoadTexture FAILED");

	// Load in the color map for the terrain.
	ASSERT(LoadColorMap(colorMapFilename), L"Terrain::LoadColorMap FAILED");

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	ASSERT(InitializeBuffers(device), L"Terrain::InitializeBuffers FAILED");

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "	Terrain::InitializeWithQuadTree time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "	Terrain::InitializeWithQuadTree time: ");
	}

	return true;
}

bool Terrain::InitializeWithMaterials(ID3D11Device* device, char* heightMapFileName, char* materialsFilename,
									  char* materialMapFilename, char* colorMapFilename, char* detailMapFilename)
{
	bool result;
	DWORD funcTime = -1;

	Timer::GetInstance()->SetTimeA();

	// Load in the height map for the terrain.
	ASSERT(LoadHeightMap(device, heightMapFileName), L"Terrain::LoadHeightMap FAILED");

	// Load detail map
	m_DetailTexture = new Texture;
	m_DetailTexture = reinterpret_cast<Texture* >(ResourceMgr::GetInstance()->GetResourceByName(detailMapFilename, ResourceMgr::ResourceTypeTexture));
	
	//ASSERT(m_DetailTexture->Initialize(device, detailMapFilename), L"m_DetailTexture initialization FAILED");

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	ASSERT(CalculateNormals(), L"Terrain::CalculateNormals FAILED");

	// Load in the color map for the terrain.
	ASSERT(LoadColorMap(colorMapFilename), L"Terrain::LoadColorMap FAILED");

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	ASSERT(InitializeBuffers(device), L"Terrain::InitializeBuffers FAILED");

	// Initialize the material groups for the terrain.
	ASSERT(LoadMaterialFile(materialsFilename, materialMapFilename, device), L"Terrain::LoadMaterialFile FAILED");

	Timer::GetInstance()->SetTimeB();
	funcTime = Timer::GetInstance()->GetDeltaTime();

	if (funcTime != -1)
	{
		Log::GetInstance()->WriteTimedMessageToFile(funcTime, "	Terrain::InitializeWithMaterials time: ");
		Log::GetInstance()->WriteTimedMessageToOutput(funcTime, "	Terrain::InitializeWithMaterials time: ");
	}

	return true;
}

void Terrain::Shutdown()
{
	// Release the texture.
	ReleaseTexture();

	// Release the height map data.
	ShutdownHeightMap();

	// Release the vertex and index buffer.
	ShutdownBuffers();

	// Release the materials for the terrain.
	ReleaseMaterials();

	return;
}

ID3D11ShaderResourceView* Terrain::GetTexture()
{
	return m_Texture->GetShaderResourceView();
}

ID3D11ShaderResourceView* Terrain::GetDetailMapTexture()
{
	return m_DetailTexture->GetShaderResourceView();
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::LoadHeightMap(ID3D11Device* device, char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	ID3D11ShaderResourceView* heightMapResourceView;
	Texture* heighMapTexture = new Texture;
	heighMapTexture = reinterpret_cast<Texture* >(ResourceMgr::GetInstance()->GetResourceByName(GetFilenameWithoutExtension(filename), ResourceMgr::ResourceTypeTexture));
	heightMapResourceView = heighMapTexture->GetShaderResourceView();


	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	/*
	if(count != imageSize)
	{
		return false;
	}
	*/
	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the height map.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}

void Terrain::NormalizeHeightMap()
{
	int i, j;


	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}

	return;
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(m_terrainHeight-1) * (m_terrainWidth-1)];
	if(!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i+1);
			index3 = ((j+1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (m_terrainWidth-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (m_terrainWidth-1)) && (j < (m_terrainHeight-1)))
			{
				index = (j * (m_terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

	return true;
}

void Terrain::ShutdownHeightMap()
{
	if(m_heightMap)
	{
		delete [] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

// TODO: Each method more than 30 lines has be checked for result by assert
void Terrain::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)m_terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = m_terrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for(j=0; j<m_terrainHeight; j++)
	{
		for(i=0; i<m_terrainWidth; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(m_terrainHeight * j) + i].tu = tuCoordinate;
			m_heightMap[(m_terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if(tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if(tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool Terrain::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new Texture;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

// TODO: Each method more than 30 lines has be checked for result by assert
HRESULT Terrain::LoadColorMap(char* filename)
{
	int error, imageSize, i, j, k, index, colorMapWidth, colorMapHeight;
	FILE* filePtr;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Open the color map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return E_FAIL;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return E_FAIL;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return E_FAIL;
	}

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	colorMapWidth = bitmapInfoHeader.biWidth;
	colorMapHeight = bitmapInfoHeader.biHeight;

	if((colorMapWidth != m_terrainWidth) || (colorMapHeight != m_terrainHeight))
	{
		return E_FAIL;
	}

	// Calculate the size of the bitmap image data.
	imageSize = colorMapWidth * colorMapHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return E_FAIL;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return E_FAIL;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return E_FAIL;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the color map portion of the height map structure.
	for(j=0; j<colorMapHeight; j++)
	{
		for(i=0; i<colorMapWidth; i++)
		{
			index = (colorMapHeight * j) + i;

			m_heightMap[index].b = (float)bitmapImage[k]   / 255.0f;
			m_heightMap[index].g = (float)bitmapImage[k+1] / 255.0f;
			m_heightMap[index].r = (float)bitmapImage[k+2] / 255.0f;

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return S_OK;
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::LoadMaterialFile(char* filename, char* materialMapFilename, ID3D11Device* device)
{
	ifstream fin;
	char input;
	int i, error;
	char inputFilename[128];
	unsigned int stringLength;
	WCHAR textureFilename[128];
	bool result;

	// Open the materials information text file.
	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	// Read up to the value of texture count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the texture count.
	fin >> m_textureCount;

	// Create the texture object array.
	m_Textures = new Texture[m_textureCount];
	if(!m_Textures)
	{
		return false;
	}

	// Load each of the textures in.
	for(i=0; i<m_textureCount; i++)
	{
		fin.get(input);
		while(input != ':')
		{
			fin.get(input);
		}

		fin >> inputFilename;

		// Convert the character filename to WCHAR.
		error = mbstowcs_s(&stringLength, textureFilename, 128, inputFilename, 128);
		if(error != 0)
		{
			return false;
		}

		// Load the texture or alpha map.
		result = m_Textures[i].Initialize(device, textureFilename);
		if(!result)
		{
			return false;
		}
	}

	// Read up to the value of the material count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the material count.
	fin >> m_materialCount;

	// Create the material group array.
	m_Materials = new MaterialGroupType[m_materialCount];
	if(!m_Materials)
	{
		return false;
	}

	// Initialize the material group array.

	// Load each of the material group indexes in.
	for(i=0; i<m_materialCount; i++)
	{
		fin.get(input);
		while(input != ':')
		{
			fin.get(input);
		}

		fin >> m_Materials[i].textureIndex1 >> m_Materials[i].textureIndex2 >> m_Materials[i].alphaIndex;
		fin >> m_Materials[i].red >> m_Materials[i].green >> m_Materials[i].blue;
	}

	// Close the materials information text file.
	fin.close();

	// Now load the material index map.
	result = LoadMaterialMap(materialMapFilename);
	if(!result)
	{
		return false;
	}

	// Load the vertex buffer for each material group with the terrain data.
	result = LoadMaterialBuffers(device);
	if(!result)
	{
		return false;
	}
	return true;
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::LoadMaterialMap(char* filename)
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Open the material map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Make sure the material index map dimensions are the same as the terrain dimensions for 1 to 1 mapping.
	if((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.
	imageSize = bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer so each vertex has an material index associated with it.
	k=0;

	// Read the material index data into the height map structure.
	for(j = 0; j < m_terrainHeight; j++)
	{
		for(i = 0; i < m_terrainWidth; i++)
		{
			index = (m_terrainHeight * j) + i;

			// TODO: Fix bug: In m_heightMap rIndex, gIndex and bIndex are written incorrect
			m_heightMap[index].rIndex = (int)bitmapImage[k+2];
			m_heightMap[index].gIndex = (int)bitmapImage[k+1];
			m_heightMap[index].bIndex = (int)bitmapImage[k];

			k+=3;
		}
	}

	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::LoadMaterialBuffers(ID3D11Device* device)
{
	int maxVertexCount, maxIndexCount, i, j, index1, index2, index3, index4, redIndex, greenIndex, blueIndex, index, vIndex;
	bool found;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the value for the maximum number of vertices a material group could possibly have.
	maxVertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the maximum vertex count.
	maxIndexCount = maxVertexCount;

	// Initialize vertex and index arrays for each material group to the maximum size.
	for(i=0; i<m_materialCount; i++)
	{
		// Create the temporary vertex array for this material group.
		m_Materials[i].vertices = new VertexType[maxVertexCount];
		if(!m_Materials[i].vertices)
		{
			return false;
		}

		// Create the temporary index array for this material group.
		m_Materials[i].indices = new unsigned long[maxIndexCount];
		if(!m_Materials[i].indices)
		{
			return false;
		}

		// Initialize the counts to zero.
		m_Materials[i].vertexCount = 0;
		m_Materials[i].indexCount = 0;
	}

	// Now loop through the terrain and build the vertex arrays for each material group.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i+1);      // Bottom right.
			index3 = (m_terrainHeight * (j+1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j+1)) + (i+1);  // Upper right.

			// Query the upper left corner vertex for the material index.
			redIndex   = m_heightMap[index3].rIndex;
			greenIndex = m_heightMap[index3].gIndex;
			blueIndex  = m_heightMap[index3].bIndex;

			// Find which material group this vertex belongs to.
			index = 0;
			found = false;
			while(!found)
			{
				if((redIndex == m_Materials[index].red) && (greenIndex == m_Materials[index].green) && (blueIndex == m_Materials[index].blue))
				{
					found = true;
				}
				else
				{
					index++;
				}
			}

			// Set the index position in the vertex and index array to the count.
			vIndex = m_Materials[index].vertexCount;

			// Upper left.
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index3].r, m_heightMap[index3].g, m_heightMap[index3].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;

			// Fill vertices buffers here
			/*
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index3].r, m_heightMap[index3].g, m_heightMap[index3].b, 1.0f);
			*/

			vIndex++;

			// Upper right.
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 0.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// Fill vertices buffers here
			/*
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			*/

			// Bottom left.
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// Fill vertices buffers here
			/*
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			*/

			// Bottom left.
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// Fill vertices buffers here
			/*
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			*/

			// Upper right.
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 0.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// Fill vertices buffers here
			/*
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			*/

			// Bottom right.
			m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_Materials[index].vertices[vIndex].texture = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index2].r, m_heightMap[index2].g, m_heightMap[index2].b, 1.0f);
			m_Materials[index].indices[vIndex] = vIndex;
			vIndex++;

			// Fill vertices buffers here
			/*
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			*/

			// Increment the vertex and index array counts.
			m_Materials[index].vertexCount += 6;
			m_Materials[index].indexCount += 6;
		}
	}

	// Now create the vertex and index buffers from the vertex and index arrays for each material group.
	for(i=0; i<m_materialCount; i++)
	{
		result = BufferManager::GetInstance()->CreateVertexBuffer(device, sizeof(VertexType) * m_Materials[i].vertexCount, m_Materials[i].vertices, &m_Materials[i].vertexBuffer);
		if(FAILED(result))
		{
			return false;
		}

		result = BufferManager::GetInstance()->CreateIndexBuffer(device, sizeof(unsigned long) * m_Materials[i].indexCount, m_Materials[i].indices, &m_Materials[i].indexBuffer);
		if(FAILED(result))
		{
			return false;
		}

		delete [] m_Materials[i].vertices;
		m_Materials[i].vertices = 0;

		delete [] m_Materials[i].indices;
		m_Materials[i].indices = 0;
	}

	return true;
}


void Terrain::ReleaseMaterials()
{
	int i;

	// Release the material groups.
	if(m_Materials)
	{
		for(i=0; i<m_materialCount; i++)
		{
			if(m_Materials[i].vertexBuffer)
			{
				m_Materials[i].vertexBuffer->Release();
				m_Materials[i].vertexBuffer = 0;
			}

			if(m_Materials[i].indexBuffer)
			{
				m_Materials[i].indexBuffer->Release();
				m_Materials[i].indexBuffer = 0;
			}

			if(m_Materials[i].vertices)
			{
				delete [] m_Materials[i].vertices;
				m_Materials[i].vertices = 0;
			}

			if(m_Materials[i].indices)
			{
				delete [] m_Materials[i].indices;
				m_Materials[i].indices = 0;
			}
		}

		delete [] m_Materials;
		m_Materials = 0;
	}

	// Release the terrain textures and alpha maps.
	if(m_Textures)
	{
		for(i=0; i<m_textureCount; i++)
		{
			m_Textures[i].Shutdown();
		}

		delete [] m_Textures;
		m_Textures = 0;
	}

	return;
}

void Terrain::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	// Release the detail map texture object.
	if(m_DetailTexture)
	{
		m_DetailTexture->Shutdown();
		delete m_DetailTexture;
		m_DetailTexture = 0;
	}

	return;
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float tu, tv;

	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Create the vertex array.
	m_vertices = new VertexType[m_vertexCount];
	if(!m_vertices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for(j=0; j<(m_terrainHeight-1); j++)
	{
		for(i=0; i<(m_terrainWidth-1); i++)
		{
			index1 = (m_terrainHeight * j) + i;          // Bottom left.
			index2 = (m_terrainHeight * j) + (i+1);      // Bottom right.
			index3 = (m_terrainHeight * (j+1)) + i;      // Upper left.
			index4 = (m_terrainHeight * (j+1)) + (i+1);  // Upper right.

			// Upper left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if(tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_vertices[index].texture = D3DXVECTOR4(m_heightMap[index3].tu, tv, 0.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index3].r, m_heightMap[index3].g, m_heightMap[index3].b, 1.0f);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if(tu == 0.0f) { tu = 1.0f; }
			if(tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = D3DXVECTOR4(tu, tv, 1.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			index++;

			// Bottom left.
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR4(m_heightMap[index1].tu, m_heightMap[index1].tv, 0.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			index++;

			// Bottom right.
			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = D3DXVECTOR4(m_heightMap[index1].tu, m_heightMap[index1].tv, 0.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if(tu == 0.0f) { tu = 1.0f; }
			if(tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = D3DXVECTOR4(tu, tv, 1.0f, 0.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if(tu == 0.0f) { tu = 1.0f; }

			m_vertices[index].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_vertices[index].texture = D3DXVECTOR4(tu, m_heightMap[index2].tv, 1.0f, 1.0f);
			m_vertices[index].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			m_vertices[index].color = D3DXVECTOR4(m_heightMap[index2].r, m_heightMap[index2].g, m_heightMap[index2].b, 1.0f);
			index++;
		}
	}

	return true;
}

// TODO: Each method more than 30 lines has be checked for result by assert
bool Terrain::Render(ID3D11DeviceContext* deviceContext, TerrainShader* shader, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	unsigned int stride;
	unsigned int offset;
	int i;
	HRESULT result;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the shader parameters that it will use for rendering.
	result = shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, false);
	if(FAILED(result))
	{
		return false;
	}

	// Set the type of primitive that should be rendered from the vertex buffers, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render each material group.
	for(i=0; i<m_materialCount; i++)
	{
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &m_Materials[i].vertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(m_Materials[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// If the material group has a valid second texture index then this is a blended terrain polygon.
		vector<ID3D11ShaderResourceView*> textures;
		if(m_Materials[i].textureIndex2 != -1)
		{
			textures.push_back(m_Textures[m_Materials[i].textureIndex1].GetShaderResourceView());
			textures.push_back(m_Textures[m_Materials[i].textureIndex2].GetShaderResourceView());
			textures.push_back(m_Textures[m_Materials[i].alphaIndex].GetShaderResourceView());
			textures.push_back(m_DetailTexture->GetShaderResourceView());
			shader->SetTextureArray(deviceContext, textures, true);
		}

		// If not then it is just a single textured polygon.
		else
		{
			textures.push_back(m_Textures[m_Materials[i].textureIndex1].GetShaderResourceView());
			textures.push_back(m_DetailTexture->GetShaderResourceView());
			shader->SetTextureArray(deviceContext, textures, false);
		}

		// Check if the textures were set or not.
		if(FAILED(result))
		{
			return false;
		}

		// Now render the prepared buffers with the shader.
		shader->RenderShader(deviceContext, m_Materials[i].indexCount);
	}

	return true;
}

void Terrain::ShutdownBuffers()
{
	// Release the vertex array.
	if(m_vertices)
	{
		delete [] m_vertices;
		m_vertices = 0;
	}

	return;
}

int Terrain::GetVertexCount()
{
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	return m_vertexCount;
}

void Terrain::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);
	return;
}

void Terrain::GetTerrainSize(int& width, int& height)
{
	// Return the width and height of the terrain.
	width = m_terrainWidth;
	height = m_terrainHeight;
	return;
}