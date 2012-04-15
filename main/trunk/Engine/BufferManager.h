#ifndef BUFFER_MANAGER_H_
#define BUFFER_MANAGER_H_

#include <d3d11.h>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "Log.h"

using namespace std;

class BufferManager
{

	public:
		static BufferManager* GetInstance();
		~BufferManager(void);
		HRESULT CreateVertexBuffer(ID3D11Device* device, int bufferSize, void * vertices, ID3D11Buffer** vertexBuffer);
		HRESULT CreateIndexBuffer(ID3D11Device* device, int bufferSize, void * indices, ID3D11Buffer** indexBuffer);
		HRESULT CreateInstanceBuffer(ID3D11Device* device, int bufferSize, void * instances, ID3D11Buffer** instanceBuffer);

	private:
		BufferManager();  // Private so that it can  not be called
		BufferManager(BufferManager const&){};             // copy constructor is private
		BufferManager& operator=(BufferManager const&){};  // assignment operator is private
		static BufferManager* m_pInstance;
};

#endif