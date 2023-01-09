#pragma once

#include <GL/glew.h>

#include "base/Base.h"

#include "types/Font.h"
#include "types/Color.h"

#include "shader_program/FontOpenGLShaderProgram.h"

namespace renderer {
namespace opengl {

MAJOR_CLASS( Font, base::Base )

	Font( types::Font *font, shader_program::FontOpenGLShaderProgram *shader_program );
	~Font();

	void Render( const char *text, float x, float y, float z_index, types::Color color ) const;
protected:
	types::Font *m_font;
	shader_program::FontOpenGLShaderProgram *m_shader_program;

	float m_tx[128]; // ascii only for now
	float m_ty[128]; // ascii only for now

	GLuint m_vbo;
	GLuint m_texture;
};

} /* namespace opengl */
} /* namespace renderer */
