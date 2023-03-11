#include "Graphics.h"

namespace graphics {

Graphics::~Graphics() {
#ifdef DEBUG
	if ( !m_on_resize_handlers.empty() ) {
		Log( "some resize handlers still set" );
	}
#endif
}

const float Graphics::GetAspectRatio() const {
	ASSERT( m_aspect_ratio, "aspect ratio not set" );
	return m_aspect_ratio;
}

void Graphics::OnWindowResize() {
	m_aspect_ratio = (float) GetViewportWidth() / GetViewportHeight();
	for ( auto& it : m_on_resize_handlers ) {
		it.second( m_aspect_ratio );
	}
}

void Graphics::AddOnWindowResizeHandler( void* object, const on_resize_handler_t& handler ) {
	ASSERT( m_on_resize_handlers.find( object ) == m_on_resize_handlers.end(), "duplicate resize handler" );
	m_on_resize_handlers[ object ] = handler;
}

void Graphics::RemoveOnWindowResizeHandler( void* object ) {
	auto it = m_on_resize_handlers.find( object );
	ASSERT( it != m_on_resize_handlers.end(), "resize handler not found" );
	m_on_resize_handlers.erase( it );
}

void Graphics::ToggleFullscreen() {
	if ( IsFullscreen() ) {
		SetWindowed();
	}
	else {
		SetFullscreen();
	}
}

}
