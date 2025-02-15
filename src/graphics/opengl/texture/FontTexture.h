#pragma once

#include <string>
#include <GL/glew.h>

#include "common/Common.h"

#include "types/Font.h"
#include "types/Color.h"

namespace graphics {
namespace opengl {

class OpenGL;

CLASS( FontTexture, common::Class )

	FontTexture( OpenGL* opengl, types::Font* font );
	~FontTexture();

	// ascii only for now
	struct rgba_t {
		float r;
		float g;
		float b;
		float a;
	};
	float m_tx[128];
	float m_ty[128];

	GLuint m_texture = 0;
};

}
}
