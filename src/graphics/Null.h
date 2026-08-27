#pragma once

#include <GL/glew.h>

#include "Graphics.h"

namespace graphics {

CLASS( Null, Graphics )
	Null( const unsigned short width = 1, const unsigned short height = 1 )
		: m_width( width )
		, m_height( height ) {}

	void Start() override { OnWindowResize(); }

	void AddScene( scene::Scene* scene ) override {}
	void RemoveScene( scene::Scene* scene ) override {}
	const unsigned short GetWindowWidth() const override { return m_width; }
	const unsigned short GetWindowHeight() const override { return m_height; }
	const unsigned short GetViewportWidth() const override { return m_width; }
	const unsigned short GetViewportHeight() const override { return m_height; };

	void LoadTexture( types::texture::Texture* texture, const bool smoothen = true ) override {};
	void UnloadTexture( const types::texture::Texture* texture ) override {};
	void WithTexture( const types::texture::Texture* texture, const f_t& f ) override {};

	const bool IsFullscreen() const override { return false; }
	void SetFullscreen() override {}
	void SetWindowed() override {}

	const bool IsMouseLocked() const override { return false; }

	void ResizeWindow( const size_t width, const size_t height ) override {
		m_width = static_cast< unsigned short >( width );
		m_height = static_cast< unsigned short >( height );
		OnWindowResize();
	}

private:
	unsigned short m_width;
	unsigned short m_height;

};

}
