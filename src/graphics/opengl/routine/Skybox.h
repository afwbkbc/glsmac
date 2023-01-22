#pragma once

#include "Routine.h"

#include "../shader_program/Skybox.h"

namespace graphics {
namespace opengl {
namespace routine {

CLASS( Skybox, Routine )
	Skybox( shader_program::Skybox *shader_program ) : m_shader_program( shader_program ) {};

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

	void OnSceneRemove( Scene *scene );
protected:
	shader_program::Skybox *m_shader_program;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
	//GLuint m_texture;

private:
	void RemoveTexture( base::ObjectLink *link );
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
