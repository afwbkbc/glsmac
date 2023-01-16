#pragma once

#include "OpenGLShaderProgram.h"

namespace renderer {
namespace opengl {

class TextActor;

namespace shader_program {

MINOR_CLASS( Font, OpenGLShaderProgram )
	FontOpenGLShaderProgram() : OpenGLShaderProgram( TYPE_FONT ) {};
protected:
	friend class opengl::TextActor;

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

#include "../TextActor.h"
