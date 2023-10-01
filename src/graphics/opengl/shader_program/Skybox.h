#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

namespace routine {
class Skybox;
}

namespace shader_program {

CLASS( Skybox, ShaderProgram )
	Skybox()
		: ShaderProgram( TYPE_SKYBOX ) {};
protected:
	friend class routine::Skybox;

	struct {
		GLint pvm;
		GLint cubemap;
	} uniforms;

	struct {
		GLint vertex;
	} attributes;

	void AddShaders() override;
	void Initialize() override;
	void EnableAttributes() const override;
	void DisableAttributes() const override;
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */

#include "../routine/Skybox.h"
