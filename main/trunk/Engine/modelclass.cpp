////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass() : mInstanceCount(-1), mVertexCount(-1), mIndexCount(-1), meshSubsets(0)
{
	mVertexBuffer = 0;
	mIndexBuffer = 0;
	mInstanceBuffer = 0;
	mModel = 0;
}


ModelClass::ModelClass(const ModelClass& existingModel) : mInstanceCount(-1), mVertexCount(-1), mIndexCount(-1), meshSubsets(0)
{
	mVertexBuffer = existingModel.GetVertexBuffer();
	mIndexBuffer = existingModel.GetIndexBuffer();
	mInstanceBuffer = existingModel.GetInstanceBuffer();
	mIndexCount = existingModel.GetIndexCount();
	mInstanceCount = existingModel.GetInstanceCount();
	mVertexCount = existingModel.GetVertexCount();
	mModel = existingModel.GetModelTypeData();

	strcpy_s(mModelFileName, existingModel.GetModelFileName());
}

ModelClass::~ModelClass()
{
}

bool ModelClass::InitializeInstanced(ID3D11Device* device, wstring modelFilename, InstanceType* instances, int numModels)
{
	bool result;
	mFileFormat = MODEL_FILE_TXT;

	result = LoadTXTModel(modelFilename.c_str());

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffersInstanced(device, instances, numModels);
	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::InitializeOrdinary(ID3D11Device* device, wstring modelFilename, ModelFileFormat fileFormat)
{
	bool result;
	D3D11_BLEND_DESC blendDesc;
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;

	ZeroMemory( &blendDesc, sizeof(blendDesc) );
	ZeroMemory( &rtbd, sizeof(rtbd) );

	mFileFormat = fileFormat;

	switch (mFileFormat)
	{
		case MODEL_FILE_TXT:
		{
			result = LoadTXTModel(modelFilename.c_str());
			// Initialize the vertex and index buffer that hold the geometry for the triangle.
			result = InitializeBuffersOrdinary(device);
			if(!result) { return false; }
			break;
		}
		case MODEL_FILE_OBJ:
		{
			result = LoadModelFromOBJ(device, modelFilename);
			if(!result) { return false; }

			// New blend state for mlt material
			ZeroMemory( &rtbd, sizeof(rtbd) );
			rtbd.BlendEnable			 = true;
			rtbd.SrcBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.DestBlend				 = D3D11_BLEND_SRC_ALPHA;
			rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha			 = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.DestBlendAlpha			 = D3D11_BLEND_SRC_ALPHA;
			rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
			rtbd.RenderTargetWriteMask	 = D3D10_COLOR_WRITE_ENABLE_ALL;
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0] = rtbd;
			device->CreateBlendState(&blendDesc, &Transparency);

			break;
		}
	}
	return true;
}

bool ModelClass::LoadTXTModel(wstring modelFilename)
{
	bool result;
	//char* stringPath = "";
	char* stringPath = (char *)malloc( 100 );

	wcstombs(stringPath, modelFilename.c_str(), 100);
	strcpy_s(mModelFileName, stringPath);

	// Load in the model data,
	result = LoadModelFromTXT(modelFilename);
	if(!result) { return false; }

	// Calculate the normal, tangent, and binormal vectors for the model.
	CalculateModelVectors();
	return true;
}

void ModelClass::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();
	// Release the model data.
	ReleaseModel();
	return;
}

int ModelClass::GetVertexCount() const
{
	return mVertexCount;
}

int ModelClass::GetInstanceCount() const
{
	return mInstanceCount;
}

int ModelClass::GetIndexCount() const
{
	int indexCount;
	switch (mFileFormat)
	{
		case MODEL_FILE_TXT:
		{
			indexCount = mIndexCount;
			break;
		}
		case MODEL_FILE_OBJ:
		{
			for(int i = 0; i < meshSubsets; ++i)
			{
				indexCount = meshSubsetIndexStart[i+1] - meshSubsetIndexStart[i];
			}
			break;
		}
		default:
		{
			indexCount = mIndexCount;
			break;
		}
	}
	return indexCount;
}

char* ModelClass::GetModelFileName() const
{
	return const_cast<char*>(mModelFileName);
}

ModelType* ModelClass::GetModelTypeData() const
{
	return mModel;
}

ID3D11Buffer* ModelClass::GetVertexBuffer() const
{
	return mVertexBuffer;
}

ID3D11Buffer* ModelClass::GetIndexBuffer() const
{
	return mIndexBuffer;
}

ID3D11Buffer* ModelClass::GetInstanceBuffer() const
{
	return mInstanceBuffer;
}

bool ModelClass::InitializeBuffersInstanced(ID3D11Device* device, InstanceType* instances /* vector<InstanceType*> instancesVector */, int numModels)
{
	VertexTypeNormalMap* vertices;
	// InstanceType* instances;

	D3D11_SUBRESOURCE_DATA vertexData, instanceData;

	//unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc; // indexBufferDesc;
	//D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	vertices = new VertexTypeNormalMap[mVertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array and index array with data.	
	for(int i=0; i < mVertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(mModel[i].x, mModel[i].y, mModel[i].z);
		vertices[i].texture = D3DXVECTOR2(mModel[i].tu, mModel[i].tv);
		vertices[i].normal = D3DXVECTOR3(mModel[i].nx, mModel[i].ny, mModel[i].nz);
		vertices[i].tangent = D3DXVECTOR3(mModel[i].tx, mModel[i].ty, mModel[i].tz);
		vertices[i].binormal = D3DXVECTOR3(mModel[i].bx, mModel[i].by, mModel[i].bz);
		// indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeNormalMap) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;
	
	mInstanceCount = numModels; // instancesVector.size()

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * mInstanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances; // instances // &instancesVector[0]
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &mInstanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	
	delete [] instances;
	instances = 0;
	
	// instancesVector.clear();

	return true;
}

bool ModelClass::InitializeBuffersOrdinary(ID3D11Device* device)
{
	VertexTypeNormalMap* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	// vertices = new VertexTypeLight[m_vertexCount]; 
	vertices = new VertexTypeNormalMap[mVertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[mIndexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.	
	for(int i=0; i < mVertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(mModel[i].x, mModel[i].y, mModel[i].z);
		vertices[i].texture = D3DXVECTOR2(mModel[i].tu, mModel[i].tv);
		vertices[i].normal = D3DXVECTOR3(mModel[i].nx, mModel[i].ny, mModel[i].nz);
		vertices[i].tangent = D3DXVECTOR3(mModel[i].tx, mModel[i].ty, mModel[i].tz);
		vertices[i].binormal = D3DXVECTOR3(mModel[i].bx, mModel[i].by, mModel[i].bz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeNormalMap) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete [] indices;
	indices = 0;

	return true;
}

void ModelClass::SetPosition(D3DXVECTOR3 posVector)
{
	
}

D3DXVECTOR3* ModelClass::GetPosition()
{
	D3DXVECTOR3 * posVector = new D3DXVECTOR3();
	return posVector;
}

bool ModelClass::LoadModelFromOBJ(ID3D11Device* device, wstring filename)
{
	HRESULT hr = 0;

	wifstream fileIn(filename.c_str());	//Open file
	wstring meshMatLib;					//String to hold our obj material library filename

	//Arrays to store our model's information
	vector<DWORD> indices;
	vector<D3DXVECTOR3> vertPos;
	vector<D3DXVECTOR3> vertNorm;
	vector<D3DXVECTOR2> vertTexCoord;
	vector<wstring> meshMaterials;

	//Vertex definition indices
	vector<int> vertPosIndex;
	vector<int> vertNormIndex;
	vector<int> vertTCIndex;

	//Make sure we have a default if no tex coords or normals are defined
	bool hasTexCoord = false;
	bool hasNorm = false;

	//Temp variables to store into vectors
	std::wstring meshMaterialsTemp;
	int vertPosIndexTemp;
	int vertNormIndexTemp;
	int vertTCIndexTemp;

	wchar_t checkChar;		//The variable we will use to store one char from file at a time
	wstring face;		//Holds the string containing our face vertices
	int vIndex = 0;			//Keep track of our vertex index count
	int triangleCount = 0;	//Total Triangles
	int totalVerts = 0;
	int meshTriangles = 0;

	bool isRHCoordSys = true;
	bool computeNormals = true;

	//Check to see if the file was opened
	if (fileIn)
	{
		while(fileIn)
		{			
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{		
			case '#':	// Comment chars
				checkChar = fileIn.get();
				while(checkChar != '\n')
					checkChar = fileIn.get();
				break;
			case 'v':	//Get Vertex Descriptions
				checkChar = fileIn.get();
				if(checkChar == ' ')	//v - vert position
				{
					float vz, vy, vx;
					fileIn >> vx >> vy >> vz;	//Store the next three types

					if(isRHCoordSys)	//If model is from an RH Coord System
						vertPos.push_back(D3DXVECTOR3( vx, vy, vz * -1.0f));	//Invert the Z axis
					else
						vertPos.push_back(D3DXVECTOR3( vx, vy, vz));
				}
				if(checkChar == 't')	//vt - vert tex coords
				{	
					float vtcu, vtcv;
					fileIn >> vtcu >> vtcv;		//Store next two types

					if(isRHCoordSys)	//If model is from an RH Coord System
						vertTexCoord.push_back(D3DXVECTOR2(vtcu, 1.0f-vtcv));	//Reverse the "v" axis
					else
						vertTexCoord.push_back(D3DXVECTOR2(vtcu, vtcv));	

					hasTexCoord = true;	//We know the model uses texture coords
				}
				//Since we compute the normals later, we don't need to check for normals
				//In the file, but i'll do it here anyway
				if(checkChar == 'n')	//vn - vert normal
				{
					float vnx, vny, vnz;
					fileIn >> vnx >> vny >> vnz;	//Store next three types

					if(isRHCoordSys)	//If model is from an RH Coord System
						vertNorm.push_back(D3DXVECTOR3( vnx, vny, vnz * -1.0f ));	//Invert the Z axis
					else
						vertNorm.push_back(D3DXVECTOR3( vnx, vny, vnz ));	

					hasNorm = true;	//We know the model defines normals
				}
				break;

				//New group (Subset)
			case 'g':	//g - defines a group
				checkChar = fileIn.get();
				if(checkChar == ' ')
				{
					meshSubsetIndexStart.push_back(vIndex);		//Start index for this subset
					meshSubsets++;
				}
				break;

				//Get Face Index
			case 'f':	//f - defines the faces
				checkChar = fileIn.get();
				if(checkChar == ' ')
				{
					face = L"";
					wstring VertDef;	//Holds one vertex definition at a time
					triangleCount = 0;

					checkChar = fileIn.get();
					while(checkChar != '\n')
					{
						face += checkChar;			//Add the char to our face string
						checkChar = fileIn.get();	//Get the next Character
						if(checkChar == ' ')		//If its a space...
							triangleCount++;		//Increase our triangle count
					}

					//Check for space at the end of our face string
					if(face[face.length()-1] == ' ')
						triangleCount--;	//Each space adds to our triangle count

					triangleCount -= 1;		//Ever vertex in the face AFTER the first two are new faces

					wstringstream ss(face);

					if(face.length() > 0)
					{
						int firstVIndex, lastVIndex;	//Holds the first and last vertice's index

						for(int i = 0; i < 3; ++i)		//First three vertices (first triangle)
						{
							ss >> VertDef;	//Get vertex definition (vPos/vTexCoord/vNorm)

							wstring vertPart;
							int whichPart = 0;		//(vPos, vTexCoord, or vNorm)

							//Parse this string
							for(int j = 0; j < VertDef.length(); ++j)
							{
								if(VertDef[j] != '/')	//If there is no divider "/", add a char to our vertPart
									vertPart += VertDef[j];

								//If the current char is a divider "/", or its the last character in the string
								if(VertDef[j] == '/' || j ==  VertDef.length()-1)
								{
									wistringstream wstringToInt(vertPart);	//Used to convert wstring to int

									if(whichPart == 0)	//If vPos
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1

										//Check to see if the vert pos was the only thing specified
										if(j == VertDef.length()-1)
										{
											vertNormIndexTemp = 0;
											vertTCIndexTemp = 0;
										}
									}

									else if(whichPart == 1)	//If vTexCoord
									{
										if(vertPart != L"")	//Check to see if there even is a tex coord
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;	//subtract one since c++ arrays start with 0, and obj start with 1
										}
										else	//If there is no tex coord, make a default
											vertTCIndexTemp = 0;

										//If the cur. char is the second to last in the string, then
										//there must be no normal, so set a default normal
										if(j == VertDef.length()-1)
											vertNormIndexTemp = 0;

									}								
									else if(whichPart == 2)	//If vNorm
									{
										wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;		//subtract one since c++ arrays start with 0, and obj start with 1
									}

									vertPart = L"";	//Get ready for next vertex part
									whichPart++;	//Move on to next vertex part					
								}
							}

							//Check to make sure there is at least one subset
							if(meshSubsets == 0)
							{
								
								meshSubsetIndexStart.push_back(vIndex);		//Start index for this subset
								meshSubsets++;
							}

							//Avoid duplicate vertices
							bool vertAlreadyExists = false;
							if(totalVerts >= 3)	//Make sure we at least have one triangle to check
							{
								//Loop through all the vertices
								for(int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									//If the vertex position and texture coordinate in memory are the same
									//As the vertex position and texture coordinate we just now got out
									//of the obj file, we will set this faces vertex index to the vertex's
									//index value in memory. This makes sure we don't create duplicate vertices
									if(vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if(vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);		//Set index for this vertex
											vertAlreadyExists = true;		//If we've made it here, the vertex already exists
										}
									}
								}
							}

							//If this vertex is not already in our vertex arrays, put it there
							if(!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;	//We created a new vertex
								indices.push_back(totalVerts-1);	//Set index for this vertex
							}							

							//If this is the very first vertex in the face, we need to
							//make sure the rest of the triangles use this vertex
							if(i == 0)
							{
								firstVIndex = indices[vIndex];	//The first vertex index of this FACE

							}

							//If this was the last vertex in the first triangle, we will make sure
							//the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
							if(i == 2)
							{								
								lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE
							}
							vIndex++;	//Increment index count
						}

						meshTriangles++;	//One triangle down

						//If there are more than three vertices in the face definition, we need to make sure
						//we convert the face to triangles. We created our first triangle above, now we will
						//create a new triangle for every new vertex in the face, using the very first vertex
						//of the face, and the last vertex from the triangle before the current triangle
						for(int l = 0; l < triangleCount-1; ++l)	//Loop through the next vertices to create new triangles
						{
							//First vertex of this triangle (the very first vertex of the face too)
							indices.push_back(firstVIndex);			//Set index for this vertex
							vIndex++;

							//Second Vertex of this triangle (the last vertex used in the tri before this one)
							indices.push_back(lastVIndex);			//Set index for this vertex
							vIndex++;

							//Get the third vertex for this triangle
							ss >> VertDef;

							wstring vertPart;
							int whichPart = 0;

							//Parse this string (same as above)
							for(int j = 0; j < VertDef.length(); ++j)
							{
								if(VertDef[j] != '/')
									vertPart += VertDef[j];
								if(VertDef[j] == '/' || j ==  VertDef.length()-1)
								{
									wistringstream wstringToInt(vertPart);

									if(whichPart == 0)
									{
										wstringToInt >> vertPosIndexTemp;
										vertPosIndexTemp -= 1;

										//Check to see if the vert pos was the only thing specified
										if(j == VertDef.length()-1)
										{
											vertTCIndexTemp = 0;
											vertNormIndexTemp = 0;
										}
									}
									else if(whichPart == 1)
									{
										if(vertPart != L"")
										{
											wstringToInt >> vertTCIndexTemp;
											vertTCIndexTemp -= 1;
										}
										else
											vertTCIndexTemp = 0;
										if(j == VertDef.length()-1)
											vertNormIndexTemp = 0;

									}								
									else if(whichPart == 2)
									{
										wistringstream wstringToInt(vertPart);

										wstringToInt >> vertNormIndexTemp;
										vertNormIndexTemp -= 1;
									}

									vertPart = L"";
									whichPart++;							
								}
							}					

							//Check for duplicate vertices
							bool vertAlreadyExists = false;
							if(totalVerts >= 3)	//Make sure we at least have one triangle to check
							{
								for(int iCheck = 0; iCheck < totalVerts; ++iCheck)
								{
									if(vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
									{
										if(vertTCIndexTemp == vertTCIndex[iCheck])
										{
											indices.push_back(iCheck);			//Set index for this vertex
											vertAlreadyExists = true;		//If we've made it here, the vertex already exists
										}
									}
								}
							}

							if(!vertAlreadyExists)
							{
								vertPosIndex.push_back(vertPosIndexTemp);
								vertTCIndex.push_back(vertTCIndexTemp);
								vertNormIndex.push_back(vertNormIndexTemp);
								totalVerts++;					//New vertex created, add to total verts
								indices.push_back(totalVerts-1);		//Set index for this vertex
							}

							//Set the second vertex for the next triangle to the last vertex we got		
							lastVIndex = indices[vIndex];	//The last vertex index of this TRIANGLE

							meshTriangles++;	//New triangle defined
							vIndex++;		
						}
					}
				}
				break;

			case 'm':	//mtllib - material library filename
				checkChar = fileIn.get();
				if(checkChar == 't')
				{
					checkChar = fileIn.get();
					if(checkChar == 'l')
					{
						checkChar = fileIn.get();
						if(checkChar == 'l')
						{
							checkChar = fileIn.get();
							if(checkChar == 'i')
							{
								checkChar = fileIn.get();
								if(checkChar == 'b')
								{
									checkChar = fileIn.get();
									if(checkChar == ' ')
									{
										//Store the material libraries file name
										fileIn >> meshMatLib;
									}
								}
							}
						}
					}
				}

				break;

			case 'u':	//usemtl - which material to use
				checkChar = fileIn.get();
				if(checkChar == 's')
				{
					checkChar = fileIn.get();
					if(checkChar == 'e')
					{
						checkChar = fileIn.get();
						if(checkChar == 'm')
						{
							checkChar = fileIn.get();
							if(checkChar == 't')
							{
								checkChar = fileIn.get();
								if(checkChar == 'l')
								{
									checkChar = fileIn.get();
									if(checkChar == ' ')
									{
										meshMaterialsTemp = L"";	//Make sure this is cleared

										fileIn >> meshMaterialsTemp; //Get next type (string)

										meshMaterials.push_back(meshMaterialsTemp);
									}
								}
							}
						}
					}
				}
				break;

			default:				
				break;
			}
		}
		
	}
	else	//If we could not open the file
	{
		//SwapChain->SetFullscreenState(false, NULL);	//Make sure we are out of fullscreen

		//create message
		wstring message = L"Could not open: ";
		message += filename;

		MessageBox(0, message.c_str(), L"Error", MB_OK);

		return false;
	}
	
	meshSubsetIndexStart.push_back(vIndex); //There won't be another index start after our last subset, so set it here

	//sometimes "g" is defined at the very top of the file, then again before the first group of faces.
	//This makes sure the first subset does not conatain "0" indices.
	if(meshSubsetIndexStart[1] == 0)
	{
		meshSubsetIndexStart.erase(meshSubsetIndexStart.begin()+1);
		meshSubsets--;
	}

	//Make sure we have a default for the tex coord and normal
	//if one or both are not specified
	if(!hasNorm)
		vertNorm.push_back(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	if(!hasTexCoord)
		vertTexCoord.push_back(D3DXVECTOR2(0.0f, 0.0f));

	//Close the obj file, and open the mtl file
	fileIn.close();
	
	wstring mltPath = wstring(L"Engine/data/models/spaceCompound/");
	wstring mltFileName = mltPath.append(meshMatLib);
	fileIn.open(mltFileName.c_str()); // meshMatLib.c_str()

	std::wstring lastStringRead;
	int matCount = material.size();	//total materials

	//kdset - If our diffuse color was not set, we can use the ambient color (which is usually the same)
	//If the diffuse color WAS set, then we don't need to set our diffuse color to ambient
	bool kdset = false;
	
	if (fileIn)
	{
		while(fileIn)
		{
			checkChar = fileIn.get();	//Get next char

			switch (checkChar)
			{
				//Check for comment
			case '#':
				checkChar = fileIn.get();
				while(checkChar != '\n')
					checkChar = fileIn.get();
				break;

				//Set diffuse color
			case 'K':
				checkChar = fileIn.get();
				if(checkChar == 'd')	//Diffuse Color
				{
					checkChar = fileIn.get();	//remove space

					fileIn >> material[matCount-1].difColor.x;
					fileIn >> material[matCount-1].difColor.y;
					fileIn >> material[matCount-1].difColor.z;

					kdset = true;
				}

				//Ambient Color (We'll store it in diffuse if there isn't a diffuse already)
				if(checkChar == 'a')	
				{					
					checkChar = fileIn.get();	//remove space
					if(!kdset)
					{
						fileIn >> material[matCount-1].difColor.x;
						fileIn >> material[matCount-1].difColor.y;
						fileIn >> material[matCount-1].difColor.z;
					}
				}
				break;

				//Check for transparency
			case 'T':
				checkChar = fileIn.get();
				if(checkChar == 'r')
				{
					checkChar = fileIn.get();	//remove space
					float Transparency;
					fileIn >> Transparency;

					material[matCount-1].difColor.w = Transparency;

					if(Transparency > 0.0f)
						material[matCount-1].transparent = true;
				}
				break;

				//Some obj files specify d for transparency
			case 'd':
				checkChar = fileIn.get();
				if(checkChar == ' ')
				{
					float Transparency;
					fileIn >> Transparency;

					//'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
					Transparency = 1.0f - Transparency;

					material[matCount-1].difColor.w = Transparency;

					if(Transparency > 0.0f)
						material[matCount-1].transparent = true;					
				}
				break;

				//Get the diffuse map (texture)
			case 'm':
				checkChar = fileIn.get();
				if(checkChar == 'a')
				{
					checkChar = fileIn.get();
					if(checkChar == 'p')
					{
						checkChar = fileIn.get();
						if(checkChar == '_')
						{
							//map_Kd - Diffuse map
							checkChar = fileIn.get();
							if(checkChar == 'K')
							{
								checkChar = fileIn.get();
								if(checkChar == 'd')
								{
									std::wstring fileNamePath;

									fileIn.get();	//Remove whitespace between map_Kd and file

									//Get the file path - We read the pathname char by char since
									//pathnames can sometimes contain spaces, so we will read until
									//we find the file extension
									bool texFilePathEnd = false;
									while(!texFilePathEnd)
									{
										checkChar = fileIn.get();

										fileNamePath += checkChar;

										if(checkChar == '.')
										{
											for(int i = 0; i < 3; ++i)
												fileNamePath += fileIn.get();

											texFilePathEnd = true;
										}							
									}

									//check if this texture has already been loaded
									bool alreadyLoaded = false;
									for(int i = 0; i < textureNameArray.size(); ++i)
									{
										if(fileNamePath == textureNameArray[i])
										{
											alreadyLoaded = true;
											material[matCount-1].texArrayIndex = i;
											material[matCount-1].hasTexture = true;
										}
									}

									//if the texture is not already loaded, load it now
									if(!alreadyLoaded)
									{
										ID3D11ShaderResourceView* tempMeshSRV;
										hr = D3DX11CreateShaderResourceViewFromFile( device, fileNamePath.c_str(),
											NULL, NULL, &tempMeshSRV, NULL );
										if(SUCCEEDED(hr))
										{
											textureNameArray.push_back(fileNamePath.c_str());
											material[matCount-1].texArrayIndex = meshSRV.size();
											meshSRV.push_back(tempMeshSRV);
											material[matCount-1].hasTexture = true;
										}
									}	
								}
							}
							//map_d - alpha map
							else if(checkChar == 'd')
							{
								//Alpha maps are usually the same as the diffuse map
								//So we will assume that for now by only enabling
								//transparency for this material, as we will already
								//be using the alpha channel in the diffuse map
								material[matCount-1].transparent = true;
							}
						}
					}
				}
				break;

			case 'n':	//newmtl - Declare new material
				checkChar = fileIn.get();
				if(checkChar == 'e')
				{
					checkChar = fileIn.get();
					if(checkChar == 'w')
					{
						checkChar = fileIn.get();
						if(checkChar == 'm')
						{
							checkChar = fileIn.get();
							if(checkChar == 't')
							{
								checkChar = fileIn.get();
								if(checkChar == 'l')
								{
									checkChar = fileIn.get();
									if(checkChar == ' ')
									{
										//New material, set its defaults
										SurfaceMaterial tempMat;
										material.push_back(tempMat);
										fileIn >> material[matCount].matName;
										material[matCount].transparent = false;
										material[matCount].hasTexture = false;
										material[matCount].texArrayIndex = 0;
										matCount++;
										kdset = false;
									}
								}
							}
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}	
	else
	{
		//SwapChain->SetFullscreenState(false, NULL);	//Make sure we are out of fullscreen

		std::wstring message = L"Could not open: ";
		message += meshMatLib;

		MessageBox(0, message.c_str(),
			L"Error", MB_OK);

		return false;
	}

	//Set the subsets material to the index value
	//of the its material in our material array
	for(int i = 0; i < meshSubsets; ++i)
	{
		bool hasMat = false;
		for(int j = 0; j < material.size(); ++j)
		{
			if(meshMaterials[i] == material[j].matName)
			{
				meshSubsetTexture.push_back(j);
				hasMat = true;
			}
		}
		if(!hasMat)
			meshSubsetTexture.push_back(0); //Use first material in array
	}

	vector<VertexTypeLight> vertices;
	VertexTypeLight tempVert;

	//Create our vertices using the information we got 
	//from the file and store them in a vector
	for(int j = 0 ; j < totalVerts; ++j)
	{
		tempVert.position = vertPos[vertPosIndex[j]];
		tempVert.normal = vertNorm[vertNormIndex[j]];
		tempVert.texture = vertTexCoord[vertTCIndex[j]];

		vertices.push_back(tempVert);
	}

	//////////////////////Compute Normals///////////////////////////
	//If computeNormals was set to true then we will create our own
	//normals, if it was set to false we will use the obj files normals
	if(computeNormals)
	{
		vector<D3DXVECTOR3> tempNormal;

		//normalized and unnormalized normals
		D3DXVECTOR3 unnormalized = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		//Used to get vectors (sides) from the position of the verts
		float vecX, vecY, vecZ;

		//Two edges of our triangle
		D3DXVECTOR4 edge1 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		D3DXVECTOR4 edge2 = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

		//Compute face normals
		for(int i = 0; i < meshTriangles; ++i)
		{
			//Get the vector describing one edge of our triangle (edge 0,2)
			vecX = vertices[indices[(i*3)]].position.x - vertices[indices[(i*3)+2]].position.x;
			vecY = vertices[indices[(i*3)]].position.y - vertices[indices[(i*3)+2]].position.y;
			vecZ = vertices[indices[(i*3)]].position.z - vertices[indices[(i*3)+2]].position.z;		
			edge1 = D3DXVECTOR4(vecX, vecY, vecZ, 0.0f);	//Create our first edge

			//Get the vector describing another edge of our triangle (edge 2,1)
			vecX = vertices[indices[(i*3)+2]].position.x - vertices[indices[(i*3)+1]].position.x;
			vecY = vertices[indices[(i*3)+2]].position.y - vertices[indices[(i*3)+1]].position.y;
			vecZ = vertices[indices[(i*3)+2]].position.z - vertices[indices[(i*3)+1]].position.z;		
			edge2 = D3DXVECTOR4(vecX, vecY, vecZ, 0.0f);	//Create our second edge

			//Cross multiply the two edge vectors to get the un-normalized face normal
			//XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
			D3DXVec3Cross(&unnormalized, & D3DXVECTOR3(edge1.x, edge1.y, edge1.z), & D3DXVECTOR3(edge2.x, edge2.y, edge2.z));
			tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
		}

		//Compute vertex normals (normal Averaging)
		D3DXVECTOR3 normalSum = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		int facesUsing = 0;
		float tX;
		float tY;
		float tZ;

		//Go through each vertex
		for(int i = 0; i < totalVerts; ++i)
		{
			//Check which triangles use this vertex
			for(int j = 0; j < meshTriangles; ++j)
			{
				if(indices[j*3] == i ||
					indices[(j*3)+1] == i ||
					indices[(j*3)+2] == i)
				{
					tX = normalSum.x + tempNormal[j].x;
					tY = normalSum.y + tempNormal[j].y;
					tZ = normalSum.z + tempNormal[j].z;

					normalSum = D3DXVECTOR3(tX, tY, tZ);	//If a face is using the vertex, add the unormalized face normal to the normalSum
					facesUsing++;
				}
			}

			//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
			normalSum = normalSum / facesUsing;

			//Normalize the normalSum vector
			D3DXVec3Normalize(&normalSum, &normalSum);
			//normalSum = XMVector3Normalize(normalSum);

			//Store the normal in our current vertex
			vertices[i].normal.x = normalSum.x;
			vertices[i].normal.y = normalSum.y;
			vertices[i].normal.z = normalSum.z;

			//Clear normalSum and facesUsing for next vertex
			normalSum = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			facesUsing = 0;

		}
	}

	//Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * meshTriangles*3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	device->CreateBuffer(&indexBufferDesc, &iinitData, &mIndexBuffer);

	//Create Vertex Buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypeLight) * totalVerts;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
	vertexBufferData.pSysMem = &vertices[0];
	hr = device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &mVertexBuffer);
	
	return true;
}

bool ModelClass::LoadModelFromTXT(wstring filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename.c_str());

	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> mVertexCount;

	// Set the number of indices to be the same as the vertex count.
	mIndexCount = mVertexCount;

	// Create the model using the vertex count that was read in.
	mModel = new ModelType[mVertexCount];
	if(!mModel)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(i=0; i<mVertexCount; i++)
	{
		fin >> mModel[i].x >> mModel[i].y >> mModel[i].z;
		fin >> mModel[i].tu >> mModel[i].tv;
		fin >> mModel[i].nx >> mModel[i].ny >> mModel[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void ModelClass::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = mVertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for(i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.position.x = mModel[index].x;
		vertex1.position.y = mModel[index].y;
		vertex1.position.z = mModel[index].z;
		vertex1.texture.x = mModel[index].tu;
		vertex1.texture.y = mModel[index].tv;
		vertex1.normal.x = mModel[index].nx;
		vertex1.normal.y = mModel[index].ny;
		vertex1.normal.z = mModel[index].nz;
		index++;

		vertex2.position.x = mModel[index].x;
		vertex2.position.y = mModel[index].y;
		vertex2.position.z = mModel[index].z;
		vertex2.texture.x = mModel[index].tu;
		vertex2.texture.y = mModel[index].tv;
		vertex2.normal.x = mModel[index].nx;
		vertex2.normal.y = mModel[index].ny;
		vertex2.normal.z = mModel[index].nz;
		index++;

		vertex3.position.x = mModel[index].x;
		vertex3.position.y = mModel[index].y;
		vertex3.position.z = mModel[index].z;
		vertex3.texture.x = mModel[index].tu;
		vertex3.texture.y = mModel[index].tv;
		vertex3.normal.x = mModel[index].nx;
		vertex3.normal.y = mModel[index].ny;
		vertex3.normal.z = mModel[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		mModel[index-1].nx = normal.x;
		mModel[index-1].ny = normal.y;
		mModel[index-1].nz = normal.z;
		mModel[index-1].tx = tangent.x;
		mModel[index-1].ty = tangent.y;
		mModel[index-1].tz = tangent.z;
		mModel[index-1].bx = binormal.x;
		mModel[index-1].by = binormal.y;
		mModel[index-1].bz = binormal.z;

		mModel[index-2].nx = normal.x;
		mModel[index-2].ny = normal.y;
		mModel[index-2].nz = normal.z;
		mModel[index-2].tx = tangent.x;
		mModel[index-2].ty = tangent.y;
		mModel[index-2].tz = tangent.z;
		mModel[index-2].bx = binormal.x;
		mModel[index-2].by = binormal.y;
		mModel[index-2].bz = binormal.z;

		mModel[index-3].nx = normal.x;
		mModel[index-3].ny = normal.y;
		mModel[index-3].nz = normal.z;
		mModel[index-3].tx = tangent.x;
		mModel[index-3].ty = tangent.y;
		mModel[index-3].tz = tangent.z;
		mModel[index-3].bx = binormal.x;
		mModel[index-3].by = binormal.y;
		mModel[index-3].bz = binormal.z;
	}
	return;
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;

	// Calculate the two vectors for this face.
	vector1[0] = vertex2.position.x - vertex1.position.x;
	vector1[1] = vertex2.position.y - vertex1.position.y;
	vector1[2] = vertex2.position.z - vertex1.position.z;

	vector2[0] = vertex3.position.x - vertex1.position.x;
	vector2[1] = vertex3.position.y - vertex1.position.y;
	vector2[2] = vertex3.position.z - vertex1.position.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.texture.x - vertex1.texture.x;
	tvVector[0] = vertex2.texture.y - vertex1.texture.y;

	tuVector[1] = vertex3.texture.x - vertex1.texture.x;
	tvVector[1] = vertex3.texture.y - vertex1.texture.y;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;

	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}

void ModelClass::ReleaseModel()
{
	if(mModel)
	{
		delete [] mModel;
		mModel = 0;
	}
	return;
}

void ModelClass::ShutdownBuffers()
{
	// Release the instance buffer.
	if(mInstanceBuffer)
	{
		mInstanceBuffer->Release();
		mInstanceBuffer = 0;
	}
	
	// Release the vertex buffer.
	if(mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderInstanced(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffersInstanced(deviceContext);
	return;
}

void ModelClass::RenderOrdinary(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	switch (mFileFormat)
	{
		case MODEL_FILE_TXT:
		{
			RenderBuffersOrdinaryForTXTFile(deviceContext);
			break;
		}
		case MODEL_FILE_OBJ:
		{
			RenderBuffersOrdinaryForOBJFile(deviceContext);
			break;
		}
		default:
		{
			RenderBuffersOrdinaryForTXTFile(deviceContext);
			break;
		}
	}
	
	return;
}

void ModelClass::RenderBuffersInstanced(ID3D11DeviceContext* deviceContext )
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexTypeNormalMap); 
	strides[1] = sizeof(InstanceType); 

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = mVertexBuffer;	
	bufferPointers[1] = mInstanceBuffer; // instanceBuffer

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ModelClass::RenderBuffersOrdinaryForTXTFile(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexTypeNormalMap); // TODO: Probably need to pass Vertex type of the model which need to be rendered
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ModelClass::RenderBuffersOrdinaryForOBJFile(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexTypeNormalMap); // TODO: Probably need to pass Vertex type of the model which need to be rendered
	offset = 0;

	//Draw our model's NON-transparent subsets
	/*
	for(int i = 0; i < 1; ++i) // meshSubsets
	{
		//Set the grounds index buffer
		deviceContext->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		deviceContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );
	*/
		//Set the WVP matrix and send it to the constant buffer in effect file
		/*
		WVP = meshWorld * camView * camProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);	
		cbPerObj.World = XMMatrixTranspose(meshWorld);	
		cbPerObj.difColor = material[meshSubsetTexture[i]].difColor;
		cbPerObj.hasTexture = material[meshSubsetTexture[i]].hasTexture;
		deviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
		deviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
		deviceContext->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
		if(material[meshSubsetTexture[i]].hasTexture)
		{
			deviceContext->PSSetShaderResources( 0, 1, &meshSRV[material[meshSubsetTexture[i]].texArrayIndex] );
		}
		deviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );
		*/
	/*
		// deviceContext->RSSetState(RSCullNone);
		int indexStart = meshSubsetIndexStart[i];
		int indexDrawAmount =  meshSubsetIndexStart[i+1] - meshSubsetIndexStart[i];
		if(!material[meshSubsetTexture[i]].transparent)
		{
			deviceContext->DrawIndexed( indexDrawAmount, indexStart, 0 );
		}
	}
	*/
	//Draw our model's TRANSPARENT subsets now

	//Set our blend state
	deviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	for(int i = 0; i < 1; ++i) // meshSubsets
	{
		//Set the grounds index buffer
		deviceContext->IASetIndexBuffer( mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		deviceContext->IASetVertexBuffers( 0, 1, &mVertexBuffer, &stride, &offset );

		//Set the WVP matrix and send it to the constant buffer in effect file
		/*
		WVP = meshWorld * camView * camProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);	
		cbPerObj.World = XMMatrixTranspose(meshWorld);	
		cbPerObj.difColor = material[meshSubsetTexture[i]].difColor;
		cbPerObj.hasTexture = material[meshSubsetTexture[i]].hasTexture;
		deviceContext->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
		deviceContext->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
		deviceContext->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
		if(material[meshSubsetTexture[i]].hasTexture)
		{
			deviceContext->PSSetShaderResources( 0, 1, &meshSRV[material[meshSubsetTexture[i]].texArrayIndex] );
		}
		deviceContext->PSSetSamplers( 0, 1, &CubesTexSamplerState );
		*/
		// deviceContext->RSSetState(RSCullNone);
		int indexStart = meshSubsetIndexStart[i];
		int indexDrawAmount =  meshSubsetIndexStart[i+1] - meshSubsetIndexStart[i];
		if(material[meshSubsetTexture[i]].transparent)
		{
			deviceContext->DrawIndexed( indexDrawAmount, indexStart, 0 );
		}
	}
}
