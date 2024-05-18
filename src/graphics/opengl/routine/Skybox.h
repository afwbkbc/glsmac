#pragma once

#include <GL/glew.h>

#include "Routine.h"

namespace graphics {
namespace opengl {

namespace shader_program {
class Skybox;
}

namespace routine {

CLASS( Skybox, Routine )

	Skybox( OpenGL* opengl, shader_program::Skybox* shader_program );

	bool SceneBelongs( const scene::Scene* scene ) const override;

	void Start() override;
	void Stop() override;
	void Iterate() override;

	void OnSceneRemove( Scene* scene ) override;

protected:
	shader_program::Skybox* m_shader_program;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
	//GLuint m_texture;

private:
	void RemoveTexture( base::ObjectLink* link );
};

}
}
}
