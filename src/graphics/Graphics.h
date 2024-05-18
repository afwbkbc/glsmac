#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>

#include "base/Module.h"

typedef std::function< void( const float aspect_ratio ) > on_resize_handler_t;

#define RH( ... ) [ __VA_ARGS__ ] ( const float aspect_ratio ) -> void

namespace types::texture {
class Texture;
}

namespace scene {
class Scene;
}

namespace graphics {

CLASS( Graphics, base::Module )

	static constexpr size_t MAX_WORLD_LIGHTS = 2;

	virtual ~Graphics();

	virtual void Iterate() override;

	virtual void AddScene( scene::Scene* scene ) = 0;
	virtual void RemoveScene( scene::Scene* scene ) = 0;
	virtual const unsigned short GetWindowWidth() const = 0;
	virtual const unsigned short GetWindowHeight() const = 0;
	virtual const unsigned short GetViewportWidth() const = 0;
	virtual const unsigned short GetViewportHeight() const = 0;

	virtual void LoadTexture( types::texture::Texture* texture ) = 0;
	virtual void UnloadTexture( const types::texture::Texture* texture ) = 0;
	virtual void EnableTexture( const types::texture::Texture* texture ) = 0;
	virtual void DisableTexture() = 0;

	virtual const bool IsFullscreen() const = 0;
	virtual void SetFullscreen() = 0;
	virtual void SetWindowed() = 0;

	virtual void RedrawOverlay() = 0;

	virtual const bool IsMouseLocked() const = 0;

	virtual void ResizeWindow( const size_t width, const size_t height ) = 0;

	const float GetAspectRatio() const;

	void AddOnWindowResizeHandler( void* object, const on_resize_handler_t& handler );
	void RemoveOnWindowResizeHandler( void* object );

	void ToggleFullscreen();

	const size_t GetFramesCountAndReset();

	void Lock();
	void Unlock();

protected:

	// make sure to call this at initialization and after every resize
	virtual void OnWindowResize();

	size_t m_frames_count = 0;

private:
	std::mutex m_render_lock;

	float m_aspect_ratio = 0;
	std::unordered_map< void*, on_resize_handler_t > m_on_resize_handlers = {};
	std::vector< void* > m_on_resize_handlers_order = {};
};

}
