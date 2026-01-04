#include "LazyTexture.h"

#include "Texture.h"
#include "loader/texture/TextureLoader.h"

namespace types {
namespace texture {

LazyTexture::LazyTexture( loader::texture::TextureLoader* const texture_loader, const std::string& filename, const types::texture::texture_flag_t flags )
	: m_texture_loader( texture_loader )
	, m_filename( filename )
	, m_flags( flags ) {}

LazyTexture::~LazyTexture() {
	if ( m_texture ) {
		delete m_texture;
	}
}

const std::string& LazyTexture::GetFilename() const {
	return m_filename;
}

Texture* const LazyTexture::Get() {
	if ( !m_texture && !m_tried_and_failed ) {
		m_texture = m_texture_loader->TryLoadCustomTexture( m_filename, m_flags );
		if ( !m_texture ) {
			m_tried_and_failed = true;
		}
	}
	return m_texture;
}

}
}
