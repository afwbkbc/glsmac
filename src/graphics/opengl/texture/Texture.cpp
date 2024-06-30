#include "Texture.h"

#include "types/texture/Texture.h"

namespace graphics {
namespace opengl {

Texture::Texture( types::texture::Texture* texture )
	: m_texture( texture ) {
	m_name = texture->m_name;

	THROW( "deprecated" );
}

Texture::~Texture() {
}

void Texture::Load() {
	//Log( "Creating OpenGL texture" );

	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_texture_obj );
	glBindTexture( m_target, m_texture_obj );
	ASSERT( m_texture->m_bpp == 3, "Unsupported texture format ( invalid BPP )" );
	glTexImage2D( m_target, 0, GL_RGBA8, m_texture->m_width, m_texture->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_texture->m_bitmap );
	glTexParameterf( m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( m_target, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( m_target, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glBindTexture( m_target, 0 );
}

void Texture::Unload() {
	//Log( "Destroying OpenGL texture" );

	//glDeleteTextures( 1, &m_texture_obj );
}

void Texture::Enable() {
	glBindTexture( m_target, m_texture_obj );
	glActiveTexture( GL_TEXTURE0 );

}

void Texture::Disable() {
	//glActiveTexture( 0 );
	glBindTexture( m_target, 0 );
}

}
}
