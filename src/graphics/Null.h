#pragma once

#include <SDL.h>
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

	void LoadTexture( types::Texture* texture ) override {};
	void UnloadTexture( const types::Texture* texture ) override {};
	void EnableTexture( const types::Texture* texture ) override {};
	void DisableTexture() override {};

	const bool IsFullscreen() const override { return false; }
	void SetFullscreen() override {}
	void SetWindowed() override {}

	void RedrawOverlay() override {}

	const bool IsMouseLocked() const override { return false; }

	void ResizeWindow( const size_t width, const size_t height ) override {}

};

}
