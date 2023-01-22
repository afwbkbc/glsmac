#pragma once

#include <GL/glew.h>

#include "Actor.h"

#include "scene/actor/Mesh.h"

namespace graphics {
namespace opengl {

CLASS(Mesh, Actor)
	Mesh( scene::actor::Mesh *actor );
	~Mesh();
	bool ReloadNeeded();
	void Load();
	void Unload();
	void Draw( shader_program::ShaderProgram *shader_program );
protected:
	size_t m_update_counter = 0;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
};

} /* namespace opengl */
} /* namespace graphics */
