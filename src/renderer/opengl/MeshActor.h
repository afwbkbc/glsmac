#pragma once

#include <GL/glew.h>

#include "Actor.h"

#include "scene/actor/MeshActor.h"

namespace renderer {
namespace opengl {

MINOR_CLASS(Mesh, Actor)
	MeshActor( scene::actor::MeshActor *actor );
	~MeshActor();
	bool ReloadNeeded();
	void Load();
	void Unload();
	void Draw( shader_program::OpenGLShaderProgram *shader_program );
protected:
	size_t m_update_timestamp = 0;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
};

} /* namespace opengl */
} /* namespace renderer */
