#include "FBO.h"

namespace graphics {
namespace opengl {

FBO::FBO( const size_t width, const size_t height ) {

	// texture render buffers
	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	
	NEW( m_mesh, mesh::Simple, 4, 2 ); // TODO: quad mesh
	auto top_left = m_mesh->AddVertex( { -1, -1 }, { 0, 0 } );
	auto top_right = m_mesh->AddVertex( { 1, -1 }, { 1, 0 } );
	auto bottom_right = m_mesh->AddVertex( { 1, 1 }, { 1, 1 } );
	auto bottom_left = m_mesh->AddVertex( { -1, 1 }, { 0, 1 } );
	
	m_mesh->AddSurface( { top_left, top_right, bottom_right } );
	m_mesh->AddSurface( { top_left, bottom_left, bottom_right } );
	
	m_mesh->Finalize();
	
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, m_mesh->GetVertexDataSize(), (GLvoid *)ptr( m_mesh->GetVertexData(), 0, m_mesh->GetVertexDataSize() ), GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_mesh->GetIndexDataSize(), (GLvoid *)ptr( m_mesh->GetIndexData(), 0, m_mesh->GetIndexDataSize() ), GL_STATIC_DRAW );
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
	
void FBO::Resize( const size_t width, const size_t height ) {
	
	ASSERT( width > 0, "fbo width zero" );
	ASSERT( height > 0, "fbo height zero" );
	
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
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textures.depth , 0 );
		
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		
	}
}

void FBO::WriteBegin() {
	ASSERT( !m_is_enabled, "fbo already enabled" );
	
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_fbo );
	glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	
	// start with clean state
	// TODO: partial redraws?
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	
	m_is_enabled = true;
}

void FBO::WriteEnd() {
	ASSERT( m_is_enabled, "fbo already disabled" );
	
	glDrawBuffer( GL_NONE );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	
	m_is_enabled = false;
}

void FBO::Draw( shader_program::Simple2D* sp ) {
	ASSERT( !m_is_enabled, "can't draw fbo that is being written to" );
	
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	
	sp->Enable();
	
	glBindTexture( GL_TEXTURE_2D, m_textures.render );
	glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	sp->Disable();
	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

}
}
