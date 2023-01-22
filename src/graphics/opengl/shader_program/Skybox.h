#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

namespace routine {
	class Skybox;
}

namespace shader_program {

CLASS( Skybox, ShaderProgram )
	Skybox() : ShaderProgram( TYPE_SKYBOX ) {};
protected:
	friend class routine::Skybox;

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
} /* namespace graphics */

#include "../routine/Skybox.h"
