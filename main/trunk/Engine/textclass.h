////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include <vector>
#include <algorithm>
#include "fontclass.h"
#include "Shaders/FontShader.h"
#include "timerclass.h"
#include "Log.h"
#include "d3dclass.h"

const int MAX_LENGTH = 35;

using namespace std;
////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);
	bool AddSentence(D3DClass* d3d, char *text, int posX, int posY, float colorR, float colorG, float colorB, int numerator);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderText(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

private:
	FontClass* m_Font;
//	FontShaderClass* m_FontShader;
	FontShader* m_FontShader;

	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;

	vector<SentenceType*> mSentenceVector;
	vector<int> mSentenceIDs;
	
	SentenceType* mSentence;

};

#endif