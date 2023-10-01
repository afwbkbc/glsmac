#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Mesh;

namespace shader_program {

CLASS( OrthographicData, ShaderProgram )

	OrthographicData()
		: ShaderProgram( TYPE_ORTHO_DATA ) {};

protected:
	friend class opengl::Mesh;

	struct {
		GLuint world;
		GLuint instances;
	} uniforms;

	struct {
		GLuint coord;
		GLuint data;
	} attributes;

	void AddShaders() override;
	void Initialize() override;
	void EnableAttributes() const override;
	void DisableAttributes() const override;

};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */

#include "../actor/Mesh.h"

