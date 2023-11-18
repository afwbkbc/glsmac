#pragma once

#include <SDL.h>
#include <GL/glew.h>

#include "../Graphics.h"

namespace graphics {
namespace null {

CLASS( Null, Graphics )

	void AddScene( scene::Scene* scene ) override;
	void RemoveScene( scene::Scene* scene ) override;
	const unsigned short GetWindowWidth() const override;
	const unsigned short GetWindowHeight() const override;
	const unsigned short GetViewportWidth() const override;
	const unsigned short GetViewportHeight() const override;

	void LoadTexture( types::Texture* texture ) override;
	void UnloadTexture( const types::Texture* texture ) override;
	void EnableTexture( const types::Texture* texture ) override;
	void DisableTexture() override;

	const bool IsFullscreen() const override;
	void SetFullscreen() override;
	void SetWindowed() override;

	void RedrawOverlay() override;

	const bool IsMouseLocked() const override;

	void ResizeWindow( const size_t width, const size_t height ) override;

};

}
}
