#pragma once

#include "OpenGLShaderProgram.h"

#include "types/Matrix44.h"

namespace renderer {
namespace opengl {

namespace routine {
	class WorldOpenGLRoutine;
}

class MeshActor;

namespace shader_program {

MINOR_CLASS( World, OpenGLShaderProgram )
	WorldOpenGLShaderProgram() : OpenGLShaderProgram( TYPE_PERSP ) {};

	types::Matrix44 GetWorldMatrix();
protected:
	friend class opengl::MeshActor;
	friend class routine::WorldOpenGLRoutine;

	struct {
		GLint world;
		GLint light_color;
		GLint light_intensity;
		GLint campos;
	} m_gl_uniforms;

	struct {
		GLint position;
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
#include "../routine/WorldOpenGLRoutine.h"
