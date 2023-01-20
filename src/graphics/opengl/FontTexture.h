#pragma once

#include <string>
#include <GL/glew.h>

#include "base/Base.h"

#include "types/Font.h"
#include "types/Color.h"

#include "shader_program/FontOpenGLShaderProgram.h"

using namespace std;

namespace graphics {
namespace opengl {

MAJOR_CLASS( FontTexture, base::Base )

	FontTexture( types::Font *font );
	~FontTexture();

	// ascii only for now
	float m_tx[128];
	float m_ty[128];
	
	GLuint m_texture;
};

} /* namespace opengl */
} /* namespace graphics */
