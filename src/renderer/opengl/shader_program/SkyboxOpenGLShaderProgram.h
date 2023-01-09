#pragma once

#include "OpenGLShaderProgram.h"

namespace renderer {
namespace opengl {

namespace routine {
	class SkyboxOpenGLRoutine;
}

namespace shader_program {

MINOR_CLASS( Skybox, OpenGLShaderProgram )
	SkyboxOpenGLShaderProgram() : OpenGLShaderProgram( TYPE_SKYBOX ) {};
protected:
	friend class routine::SkyboxOpenGLRoutine;

	struct {
		GLint pvm;
		GLint cubemap;
	} m_gl_uniforms;

	struct {
		GLint vertex;
	} m_gl_attributes;

	void AddShaders();
	void Initialize();
	void EnableAttributes() const;
	void DisableAttributes() const;
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */

#include "../routine/SkyboxOpenGLRoutine.h"
