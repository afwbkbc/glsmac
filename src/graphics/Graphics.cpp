#include "Graphics.h"

namespace graphics {

Graphics::~Graphics() {
#ifdef DEBUG
	if ( !m_on_resize_handlers.empty() ) {
		Log( "WARNING: some resize handlers still set" );
	}
	if ( !m_on_resize_handlers_order.empty() ) {
		Log( "WARNING: some resize handlers still set in order vector" );
	}
#endif
}

void Graphics::Iterate() {
	m_frames_count++;
}

const float Graphics::GetAspectRatio() const {
	ASSERT( m_aspect_ratio, "aspect ratio not set" );
	return m_aspect_ratio;
}

void Graphics::OnWindowResize() {
	m_aspect_ratio = (float) GetViewportHeight() / GetViewportWidth();
	for ( auto& object : m_on_resize_handlers_order ) {
		ASSERT( m_on_resize_handlers.find( object ) != m_on_resize_handlers.end(), "resize handler not found but exists in order vector" );
		m_on_resize_handlers.at( object )( m_aspect_ratio );
	}
}

void Graphics::AddOnWindowResizeHandler( void* object, const on_resize_handler_t& handler ) {
	ASSERT( m_on_resize_handlers.find( object ) == m_on_resize_handlers.end(), "duplicate resize handler" );
	m_on_resize_handlers[ object ] = handler;
	m_on_resize_handlers_order.push_back( object );
}

void Graphics::RemoveOnWindowResizeHandler( void* object ) {
	{
		auto it = m_on_resize_handlers.find( object );
		ASSERT( it != m_on_resize_handlers.end(), "resize handler not found" );
		m_on_resize_handlers.erase( it );
	}
	{
		auto it = std::find( m_on_resize_handlers_order.begin(), m_on_resize_handlers_order.end(), object );
		ASSERT( it != m_on_resize_handlers_order.end(), "resize handler not found in order vector" );
		m_on_resize_handlers_order.erase( it );
	}
}

void Graphics::ToggleFullscreen() {
	if ( IsFullscreen() ) {
		SetWindowed();
	}
	else {
		SetFullscreen();
	}
}

const size_t Graphics::GetFramesCountAndReset() {
	const size_t frames_count = m_frames_count;
	m_frames_count = 0;
	return frames_count;
}

}
