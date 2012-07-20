////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11tex.h>
#include <stdint.h>
#include "BasicResource.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class Texture : public BasicResource
{
	public:
		Texture();
		Texture(const Texture&);
		~Texture();

		bool						Initialize(ID3D11Device*, WCHAR*);
		virtual void				Shutdown();

		ID3D11ShaderResourceView*	GetShaderResourceView();
		ID3D11ShaderResourceView**	GetShaderView();
		void						SetShaderResourceView(ID3D11ShaderResourceView* srv);

		ID3D11Resource*				GetResource();
		void						SetResource(ID3D11Resource* resource);

		UINT						GetWidth();
		UINT						GetHeight();
		void						SetWidth(UINT width);
		void						SetHeight(UINT height);

	private:
		ID3D11ShaderResourceView*	m_ShaderResourceView;
		ID3D11Resource*				m_Resource;
		UINT						m_Width;
		UINT						m_Height;
};

inline ID3D11ShaderResourceView** Texture::GetShaderView()
{
	return &m_ShaderResourceView;
}

inline ID3D11ShaderResourceView* Texture::GetShaderResourceView()
{
	return m_ShaderResourceView;
}

inline void	Texture::SetShaderResourceView(ID3D11ShaderResourceView* srv)
{
	m_ShaderResourceView = srv;
}

inline ID3D11Resource* Texture::GetResource()
{
	return m_Resource;
}

inline void	Texture::SetResource(ID3D11Resource* srv)
{
	m_Resource = srv;
}

inline UINT Texture::GetWidth()
{
	return m_Width;
}

inline UINT Texture::GetHeight()
{
	return m_Height;
}

inline void Texture::SetWidth(UINT width)
{
	m_Width = width;
}

inline void Texture::SetHeight(UINT height)
{
	m_Height = height;
}


#endif
