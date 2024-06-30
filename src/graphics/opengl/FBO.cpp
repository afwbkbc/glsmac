#include "FBO.h"

#include "types/mesh/Simple.h"
#include "types/texture/Texture.h"
#include "graphics/opengl/shader_program/Simple2D.h"

namespace graphics {
namespace opengl {

FBO::FBO( const size_t width, const size_t height ) {

	// texture render buffers
	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );

	NEW( m_mesh, types::mesh::Simple, 4, 2 ); // TODO: quad mesh
	auto top_left = m_mesh->AddVertex(
		{
			-1,
			-1,
			-1
		}, {
			0.0f,
			0.0f
		}
	);
	auto top_right = m_mesh->AddVertex(
		{
			1,
			-1,
			-1
		}, {
			1.0f,
			0.0f
		}
	);
	auto bottom_right = m_mesh->AddVertex(
		{
			1,
			1,
			-1
		}, {
			1.0f,
			1.0f
		}
	);
	auto bottom_left = m_mesh->AddVertex(
		{
			-1,
			1,
			-1
		}, {
			0.0f,
			1.0f
		}
	);

	m_mesh->AddSurface(
		{
			top_left,
			top_right,
			bottom_right
		}
	);
	m_mesh->AddSurface(
		{
			top_left,
			bottom_left,
			bottom_right
		}
	);

	m_mesh->Finalize();

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, m_mesh->GetVertexDataSize(), (GLvoid*)ptr( m_mesh->GetVertexData(), 0, m_mesh->GetVertexDataSize() ), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_mesh->GetIndexDataSize(), (GLvoid*)ptr( m_mesh->GetIndexData(), 0, m_mesh->GetIndexDataSize() ), GL_STATIC_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	m_ibo_size = m_mesh->GetIndexCount();

	// framebuffer
	glGenFramebuffers( 1, &m_fbo );
	glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );

#ifdef DEBUG
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	ASSERT( GL_FRAMEBUFFER_COMPLETE, "FB error, status: " + std::to_string( status ) );
#endif

	glGenTextures( 1, &m_textures.render );
	glGenTextures( 1, &m_textures.depth );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	m_width = m_height = 0;
	Resize( width, height );
}

FBO::~FBO() {

	glDeleteBuffers( 1, &m_vbo );
	glDeleteBuffers( 1, &m_ibo );

	glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );
	glDeleteTextures( 1, &m_textures.render );
	glDeleteTextures( 1, &m_textures.depth );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers( 1, &m_fbo );

	DELETE( m_mesh );
}

void FBO::Resize( size_t width, size_t height ) {

	ASSERT( width > 0, "fbo width zero" );
	ASSERT( height > 0, "fbo height zero" );

	// upscale
	width *= INTERNAL_RESOLUTION_MULTIPLIER;
	height *= INTERNAL_RESOLUTION_MULTIPLIER;

	if ( width != m_width || height != m_height ) {

		m_width = width;
		m_height = height;

		glBindFramebuffer( GL_FRAMEBUFFER, m_fbo );

		glBindTexture( GL_TEXTURE_2D, m_textures.render );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textures.render, 0 );

		glBindTexture( GL_TEXTURE_2D, m_textures.depth );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures.depth, 0 );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	}
}

void FBO::WriteBegin() {
	ASSERT( !m_is_enabled, "fbo already enabled" );
	ASSERT( m_width > 0, "fbo width is zero" );
	ASSERT( m_height > 0, "fbo height is zero" );

	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
	glDrawBuffer( GL_COLOR_ATTACHMENT0 );

	if ( INTERNAL_RESOLUTION_MULTIPLIER != 1 ) {
		// upscale
		glViewport( 0, 0, m_width, m_height );
	}

	// start with clean state
	// TODO: partial redraws?
	//glClearColor( 0.5f, 0.5f, 0.5f, 0.5f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// blending fix
	glBlendFuncSeparate( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

	m_is_enabled = true;
}

void FBO::WriteEnd() {
	ASSERT( m_is_enabled, "fbo already disabled" );

	// restore default
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDrawBuffer( GL_NONE );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

	if ( INTERNAL_RESOLUTION_MULTIPLIER != 1 ) {
		// restore
		glViewport( 0, 0, m_width / INTERNAL_RESOLUTION_MULTIPLIER, m_height / INTERNAL_RESOLUTION_MULTIPLIER );
	}

	m_is_enabled = false;
}

void FBO::Draw( shader_program::Simple2D* sp ) {
	ASSERT( !m_is_enabled, "can't draw fbo that is being written to" );
	ASSERT( m_width > 0, "fbo width is zero" );
	ASSERT( m_height > 0, "fbo height is zero" );

	// blending fix
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );

	sp->Enable();

	glUniform1ui( sp->uniforms.flags, 0 );

	glBindTexture( GL_TEXTURE_2D, m_textures.render );
	glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void*)( 0 ) );
	glBindTexture( GL_TEXTURE_2D, 0 );

	sp->Disable();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// restore default
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

types::texture::Texture* FBO::CaptureToTexture() {
	ASSERT( !m_is_enabled, "can't read fbo that is being written to" );
	ASSERT( m_width > 0, "fbo width is zero" );
	ASSERT( m_height > 0, "fbo height is zero" );

	// downscale
	const auto width = m_width / INTERNAL_RESOLUTION_MULTIPLIER;
	const auto height = m_height / INTERNAL_RESOLUTION_MULTIPLIER;

	NEWV( texture, types::texture::Texture, "FBOCapture", width, height );

	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_fbo );
	glReadBuffer( GL_COLOR_ATTACHMENT0 );

	glReadPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture->m_bitmap );

	glReadBuffer( GL_NONE );
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

	return texture;
}

}
}
