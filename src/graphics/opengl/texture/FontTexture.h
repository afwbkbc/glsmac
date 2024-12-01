#pragma once

#include <string>
#include <GL/glew.h>

#include "common/Common.h"

#include "types/Font.h"
#include "types/Color.h"

#include "graphics/opengl/shader_program/Font.h"

namespace graphics {
namespace opengl {

class OpenGL;

CLASS( FontTexture, common::Class )

	FontTexture( OpenGL* opengl, types::Font* font );
	~FontTexture();

	// ascii only for now
	float m_tx[128];
	float m_ty[128];

	GLuint m_texture = 0;
};

}
}
