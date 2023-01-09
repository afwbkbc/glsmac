#pragma once

#include "OpenGLShaderProgram.h"

namespace renderer {
namespace opengl {

class Font;

namespace shader_program {

MINOR_CLASS( Font, OpenGLShaderProgram )
	FontOpenGLShaderProgram() : OpenGLShaderProgram( TYPE_FONT ) {};
	const float GetAspectRatio() const {
		return 1;
	}
	const int GetWindowWidth() const {
		return 640;
	}
	const int GetWindowHeight() const {
		return 480;
	}
protected:
	friend class opengl::Font;

	struct {
		GLint texture;
		GLint color;
		GLint z_index;
	} m_gl_uniforms;

	struct {
		GLint coord;
	} m_gl_attributes;

	void AddShaders();
	void Initialize();
	void EnableAttributes() const;
	void DisableAttributes() const;

};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */

#include "../Font.h"
