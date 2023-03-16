#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include "base/Module.h"

#include "scene/Scene.h"
#include "types/Texture.h"

typedef std::function<void( const float aspect_ratio )> on_resize_handler_t;

#define RH(...) [ __VA_ARGS__ ] ( const float aspect_ratio ) -> void

namespace graphics {

CLASS( Graphics, base::Module )

	static constexpr size_t MAX_WORLD_INSTANCES = 9; // not needed more than 3 in most cases but no harm from supporting extra
	static constexpr size_t MAX_WORLD_LIGHTS = 2;

	virtual ~Graphics();

	virtual void Iterate();
	
	virtual void AddScene( scene::Scene *scene ) = 0;
	virtual void RemoveScene( scene::Scene *scene ) = 0;
	virtual const unsigned short GetViewportWidth() const = 0;
	virtual const unsigned short GetViewportHeight() const = 0;
	
	virtual void LoadTexture( const types::Texture* texture ) = 0;
	virtual void UnloadTexture( const types::Texture* texture ) = 0;
	virtual void EnableTexture( const types::Texture* texture ) = 0;
	virtual void DisableTexture() = 0;

	virtual const bool IsFullscreen() const = 0;
	virtual void SetFullscreen() = 0;
	virtual void SetWindowed() = 0;
	
	virtual void RedrawOverlay() = 0;
	
	virtual const bool IsMouseLocked() const = 0;
	
	virtual void ResizeViewport( const size_t width, const size_t height ) = 0;
	
	const float GetAspectRatio() const;
	
	void AddOnWindowResizeHandler( void* object, const on_resize_handler_t& handler );
	void RemoveOnWindowResizeHandler( void* object );
	
	void ToggleFullscreen();
	
	const size_t GetFramesCountAndReset();
	
protected:
	
	// make sure to call this at initialization and after every resize
	virtual void OnWindowResize();
	
	size_t m_frames_count = 0;
	
private:
	float m_aspect_ratio = 0;
	std::unordered_map< void*, on_resize_handler_t > m_on_resize_handlers = {};
	std::vector< void* > m_on_resize_handlers_order = {};
};

} /* namespace graphics */
