#pragma once

#include "OpenGLShaderProgram.h"

namespace renderer {
namespace opengl {

class MeshActor;

namespace shader_program {

MINOR_CLASS( Orthographic, OpenGLShaderProgram )
	OrthographicOpenGLShaderProgram() : OpenGLShaderProgram( TYPE_ORTHO ) {};
protected:
	friend class opengl::MeshActor;

	struct {
		GLint sampler;
		GLint tint;
		GLint z_index;
	} m_gl_uniforms;

	struct {
		GLint coord;
		GLint tex_coord;
	} m_gl_attributes;

	void AddShaders();
	void Initialize();
	void EnableAttributes() const;
	void DisableAttributes() const;
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */

#include "../MeshActor.h"

