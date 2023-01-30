#pragma once

#include <vector>
#include <functional>

#include "base/Module.h"

#include "scene/Scene.h"
#include "types/Texture.h"

typedef function<void( const float aspect_ratio )> on_resize_handler_t;

#define RH(...) [ __VA_ARGS__ ] ( const float aspect_ratio ) -> void

namespace graphics {

CLASS( Graphics, base::Module )

	virtual ~Graphics();

	virtual void AddScene( scene::Scene *scene ) = 0;
	virtual void RemoveScene( scene::Scene *scene ) = 0;
	virtual const unsigned short GetWindowWidth() const = 0;
	virtual const unsigned short GetWindowHeight() const = 0;
	
	virtual void LoadTexture( const types::Texture* texture ) = 0;
	virtual void UnloadTexture( const types::Texture* texture ) = 0;
	virtual void EnableTexture( const types::Texture* texture ) = 0;
	virtual void DisableTexture() = 0;

	virtual void ResizeWindow( const size_t width, const size_t height ) = 0;
	
	const float GetAspectRatio() const;
	
	void AddOnResizeHandler( void* object, const on_resize_handler_t& handler );
	void RemoveOnResizeHandler( void* object );
	
protected:
	
	// make sure to call this at initialization and after every resize
	void OnResize();
	
private:
	float m_aspect_ratio = 0;
	unordered_map< void*, on_resize_handler_t > m_on_resize_handlers;
};

} /* namespace graphics */
