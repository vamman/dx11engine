#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(void) :
	m_PositionX(0)
	, m_PositionY(0)
	, m_PositionZ(0)
	, m_Texture(0)
	, m_particleList(0)
	, m_vertices(0)
	, m_vertexBuffer(0)	
	, m_indexBuffer(0)
	, m_ParticleLifeTime(0)
	, m_VerticleAngle(0)
{
}

ParticleSystem::ParticleSystem(const ParticleSystem& other)
{
}

ParticleSystem::~ParticleSystem(void)
{
}

bool ParticleSystem::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	m_Texture = (Texture* )ResourceMgr::GetInstance()->GetResourceByName(textureFilename, ResourceMgr::ResourceType::ResourceTypeTexture);

	// Initialize the particle system.
	bool result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Shutdown()
{
	// Release the particle system.
	ShutdownParticleSystem();

	return;
}

bool ParticleSystem::Frame(float frameTime, ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition)
{
	bool result;

	// Release old particles.
	KillParticles(frameTime);

	// Emit new particles.
	EmitParticles(frameTime);

	// Update the position of the particles.
	UpdateParticles(frameTime);

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext, cameraPosition);
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	return;
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture()
{
	return m_Texture->GetShaderResourceView();
}

int ParticleSystem::GetIndexCount()
{
	return m_indexCount;
}

bool ParticleSystem::InitializeParticleSystem()
{
	int i;

	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.2f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 5000;

	m_ParticleLifeTime = 4000.0f;

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles];
	if(!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for(i=0; i<m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}

void ParticleSystem::SetPosition(D3DXVECTOR3 position)
{
	m_PositionX = position.x;
	m_PositionY = position.y;
	m_PositionZ = position.z;
}

D3DXVECTOR3 ParticleSystem::GetPosition()
{
	return D3DXVECTOR3(m_PositionX, m_PositionY, m_PositionZ);
}

void ParticleSystem::ShutdownParticleSystem()
{
	// Release the particle list.
	if(m_particleList)
	{
		delete [] m_particleList;
		m_particleList = 0;
	}

	return;
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if(!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for(i=0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	result = BufferManager::GetInstance()->CreateVertexBuffer(device, sizeof(VertexType) * m_vertexCount,
		m_vertices, &m_vertexBuffer, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	result = BufferManager::GetInstance()->CreateIndexBuffer(device, sizeof(unsigned long) * m_indexCount, indices, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

void ParticleSystem::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue, lifeTime;
	int index, i, j;

	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if(m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationX + m_PositionX;
		positionY = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationY + m_PositionY;
		positionZ = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationZ + m_PositionZ;

		velocity = m_particleVelocity + (((float)rand()-(float)rand())/RAND_MAX) * m_particleVelocityVariation;

		red   = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		green = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
		blue  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;

		lifeTime = 0.0f;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while(!found)
		{
			if((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over
		// by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while(i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red       = m_particleList[j].red;
			m_particleList[i].green     = m_particleList[j].green;
			m_particleList[i].blue      = m_particleList[j].blue;
			m_particleList[i].velocity  = m_particleList[j].velocity;
			m_particleList[i].lifeTime  = m_particleList[j].lifeTime;
			m_particleList[i].active    = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red       = red;
		m_particleList[index].green     = green;
		m_particleList[index].blue      = blue;
		m_particleList[index].velocity  = velocity;
		m_particleList[index].lifeTime  = lifeTime;
		m_particleList[index].active    = true;
	}

	return;
}

void ParticleSystem::UpdateParticles(float frameTime)
{
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for(i=0; i<m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
		m_particleList[i].lifeTime += frameTime;
		
	}

	return;
}

void ParticleSystem::KillParticles(float frameTime)
{
	int i, j;

	// Kill all the particles that have gone below a certain height range.
	for(i=0; i<m_maxParticles; i++)
	{
		if( m_particleList[i].active == true && 
			m_particleList[i].lifeTime >= m_ParticleLifeTime)
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array 
			// sorted correctly.
			for(j=i; j<m_maxParticles-1; j++)
			{
				m_particleList[j].positionX = m_particleList[j+1].positionX;
				m_particleList[j].positionY = m_particleList[j+1].positionY;
				m_particleList[j].positionZ = m_particleList[j+1].positionZ;
				m_particleList[j].red       = m_particleList[j+1].red;
				m_particleList[j].green     = m_particleList[j+1].green;
				m_particleList[j].blue      = m_particleList[j+1].blue;
				m_particleList[j].velocity  = m_particleList[j+1].velocity;
				m_particleList[j].lifeTime  = m_particleList[j+1].lifeTime;
				m_particleList[j].active    = m_particleList[j+1].active;
			}
		}
	}
	return;
}

bool ParticleSystem::UpdateBuffers(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 cameraPosition)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	double horizontalAngle, verticalAngle;
	float horizontalRotation, verticalRotation, cosHorizontalAlpha, cosVerticalAlpha, sinHorizontalAlpha, sinVerticalAlpha;

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for(i=0; i<m_currentParticleCount; i++)
	{
		// Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
		horizontalAngle = atan2(m_particleList[i].positionX - cameraPosition.x, m_particleList[i].positionZ - cameraPosition.z) * (180.0 / D3DX_PI);
		// verticalAngle	= atan2(m_particleList[i].positionY - cameraPosition.y, m_particleList[i].positionZ - cameraPosition.z) * (180.0 / D3DX_PI);
		m_VerticleAngle += 0.00001f * (180.0 / D3DX_PI);
		verticalAngle	=  m_VerticleAngle;
		//horizontalAngle	=  m_VerticleAngle;


		// Convert rotation into radians.
		horizontalRotation = (float)horizontalAngle * 0.0174532925f;
		verticalRotation   = (float)verticalAngle * 0.0174532925f;

		cosHorizontalAlpha = cos(horizontalRotation);
		sinHorizontalAlpha = sin(horizontalRotation);

		cosVerticalAlpha = cos(verticalRotation);
		sinVerticalAlpha = sin(verticalRotation);

		// Coordinates without rotations :
		// Bottom left
		float bottomLeftX = m_particleList[i].positionX - m_particleSize;
		float bottomLeftY = m_particleList[i].positionY - m_particleSize;
		float bottomLeftZ = m_particleList[i].positionZ + m_particleSize;

		// Top left
		float topLeftX = m_particleList[i].positionX - m_particleSize;
		float topLeftY = m_particleList[i].positionY + m_particleSize;
		float topLeftZ = m_particleList[i].positionZ + m_particleSize;

		// Bottom right
		float bottomRightX = m_particleList[i].positionX + m_particleSize;
		float bottomRightY = m_particleList[i].positionY - m_particleSize;
		float bottomRightZ = m_particleList[i].positionZ - m_particleSize;

		// Top right
		float topRightX = m_particleList[i].positionX + m_particleSize;
		float topRightY = m_particleList[i].positionY + m_particleSize;
		float topRightZ = m_particleList[i].positionZ - m_particleSize;

		// For horizontal rotation we need to multiply m_particleSize in z component on - sinHorizontalAlpha

		// Bottom left.
		m_vertices[index].position = /*RotatePointByYAxis(D3DXVECTOR3(bottomLeftX, bottomLeftY, bottomLeftZ), sinHorizontalAlpha, cosHorizontalAlpha);*/ D3DXVECTOR3(m_particleList[i].positionX - /*cosHorizontalAlpha */ m_particleSize, m_particleList[i].positionY - sinVerticalAlpha *  m_particleSize, m_particleList[i].positionZ - /*sinHorizontalAlpha*/ cosVerticalAlpha * m_particleSize);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = /*RotatePointByYAxis(D3DXVECTOR3(topLeftX, topLeftY, topLeftZ), sinHorizontalAlpha, cosHorizontalAlpha);*/ D3DXVECTOR3(m_particleList[i].positionX - /*cosHorizontalAlpha */ m_particleSize, m_particleList[i].positionY + sinVerticalAlpha * m_particleSize, m_particleList[i].positionZ - /*sinHorizontalAlpha*/ cosVerticalAlpha * m_particleSize);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = /*RotatePointByYAxis(D3DXVECTOR3(bottomRightX, bottomRightY, bottomRightZ), sinHorizontalAlpha, cosHorizontalAlpha);*/ D3DXVECTOR3(m_particleList[i].positionX + /*cosHorizontalAlpha */ m_particleSize, m_particleList[i].positionY - sinVerticalAlpha * m_particleSize, m_particleList[i].positionZ + /*sinHorizontalAlpha*/ cosVerticalAlpha * m_particleSize);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = /*RotatePointByYAxis(D3DXVECTOR3(bottomRightX, bottomRightY, bottomRightZ), sinHorizontalAlpha, cosHorizontalAlpha);*/ D3DXVECTOR3(m_particleList[i].positionX + /*cosHorizontalAlpha */ m_particleSize, m_particleList[i].positionY - sinVerticalAlpha * m_particleSize, m_particleList[i].positionZ + /*sinHorizontalAlpha */ cosVerticalAlpha * m_particleSize);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = /*RotatePointByYAxis(D3DXVECTOR3(topLeftX, topLeftY, topLeftZ), sinHorizontalAlpha, cosHorizontalAlpha);*/ D3DXVECTOR3(m_particleList[i].positionX - /*cosHorizontalAlpha */ m_particleSize, m_particleList[i].positionY + sinVerticalAlpha * m_particleSize, m_particleList[i].positionZ - /*sinHorizontalAlpha*/ cosVerticalAlpha * m_particleSize);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top right.
		m_vertices[index].position = /*RotatePointByYAxis(D3DXVECTOR3(topRightX, topRightY, topRightZ), sinHorizontalAlpha, cosHorizontalAlpha);*/ D3DXVECTOR3(m_particleList[i].positionX + /*cosHorizontalAlpha */ m_particleSize, m_particleList[i].positionY + sinVerticalAlpha * m_particleSize, m_particleList[i].positionZ + /*sinHorizontalAlpha*/ cosVerticalAlpha * m_particleSize);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

D3DXVECTOR3 ParticleSystem::RotatePointByXAxis(D3DXVECTOR3 vec, float sinAlpha, float cosAlpha)
{
	float xNew = vec.x;
	float yNew = vec.y * cosAlpha - vec.z * sinAlpha;
	float zNew = vec.y * sinAlpha + vec.z * cosAlpha;
	return D3DXVECTOR3(xNew, yNew, zNew);
}

D3DXVECTOR3 ParticleSystem::RotatePointByYAxis(D3DXVECTOR3 vec, float sinAlpha, float cosAlpha)
{
	float xNew = vec.x * cosAlpha - vec.z * sinAlpha;
	float yNew = vec.y;
	float zNew = (-1 * vec.x) * sinAlpha + vec.z * cosAlpha;
	return D3DXVECTOR3(xNew, yNew, zNew);
}

D3DXVECTOR3 ParticleSystem::RotatePointByZAxis(D3DXVECTOR3 vec, float sinAlpha, float cosAlpha)
{
	float xNew = vec.x * cosAlpha - vec.y * sinAlpha;
	float yNew = vec.x * sinAlpha + vec.y * cosAlpha;
	float zNew = vec.z;
	return D3DXVECTOR3(xNew, yNew, zNew);
}

void ParticleSystem::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


