#pragma once

#include "OpenGLRoutine.h"

#include "../shader_program/SkyboxOpenGLShaderProgram.h"

namespace renderer {
namespace opengl {
namespace routine {

MINOR_CLASS( Skybox, OpenGLRoutine )
	SkyboxOpenGLRoutine( shader_program::SkyboxOpenGLShaderProgram *shader_program ) : m_shader_program( shader_program ) {};

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

	void OnSceneRemove( Scene *scene );
protected:
	shader_program::SkyboxOpenGLShaderProgram *m_shader_program;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
	//GLuint m_texture;

private:
	void RemoveTexture( base::ObjectLink *link );
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
