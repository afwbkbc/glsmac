#pragma once

#include <GL/glew.h>

#include "Graphics.h"

namespace graphics {

CLASS( Null, Graphics )

	void AddScene( scene::Scene* scene ) override {}
	void RemoveScene( scene::Scene* scene ) override {}
	const unsigned short GetWindowWidth() const override { return 0; }
	const unsigned short GetWindowHeight() const override { return 0; }
	const unsigned short GetViewportWidth() const override { return 0; }
	const unsigned short GetViewportHeight() const override { return 0; };

	void LoadTexture( types::texture::Texture* texture, const bool smoothen = true, const bool mipmaps = true ) override {};
	void UnloadTexture( const types::texture::Texture* texture ) override {};
	void WithTexture( const types::texture::Texture* texture, const f_t& f ) override {};

	const bool IsFullscreen() const override { return false; }
	void SetFullscreen() override {}
	void SetWindowed() override {}

	void RedrawOverlay() override {}

	const bool IsMouseLocked() const override { return false; }

	void ResizeWindow( const size_t width, const size_t height ) override {}

};

}
