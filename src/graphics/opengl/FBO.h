#pragma once

#include <GL/glew.h>

#include "base/Base.h"

#include "shader_program/Simple2D.h"
#include "types/mesh/Simple.h"

namespace graphics {
namespace opengl {

CLASS( FBO, base::Base )
	
	FBO( const size_t width, const size_t height );
	~FBO();
	
	void Resize( const size_t width, const size_t height );

	void WriteBegin();
	void WriteEnd();
	
	void Draw( shader_program::Simple2D* sp );
	
protected:
	
	bool m_is_enabled = false;
	
	size_t m_width = 0;
	size_t m_height = 0;
	
	GLuint m_fbo = 0;
	struct {
		GLuint render = 0;
		GLuint depth = 0;
	} m_textures;
	
	// for rendering texture to screen
	GLuint m_vbo;
	GLuint m_ibo;
	size_t m_ibo_size;
	types::mesh::Simple* m_mesh = nullptr;
};
	
}
}
