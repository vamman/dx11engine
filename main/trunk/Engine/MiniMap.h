////////////////////////////////////////////////////////////////////////////////
// Filename: minimapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MINIMAPCLASS_H_
#define _MINIMAPCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "bitmapclass.h"
#include "shaders/textureshader.h"
#include "shaders/FontShader.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: MiniMapClass
////////////////////////////////////////////////////////////////////////////////
class MiniMap
{
	public:
		MiniMap();
		MiniMap(const MiniMap&);
		~MiniMap();

		HRESULT Initialize(ID3D11Device*, HWND, int, int, D3DXMATRIX, float, float);
		void Shutdown();
		bool Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix, TextureShader* textureShader, float cameraRotY);
		void PositionUpdate(float, float);

	private:
		int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY, mPlayerViewImageWidth, mPlayerViewImageHeight;
		float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
		D3DXMATRIX m_viewMatrix;
		BitmapClass *m_MiniMapBitmap, *m_Border, *mPlayerView;
		FontShader *mPlayerViewShader;
};

#endif