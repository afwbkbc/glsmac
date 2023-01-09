#pragma once

#include "OpenGLRoutine.h"

#include "../shader_program/WorldOpenGLShaderProgram.h"

namespace renderer {
namespace opengl {
namespace routine {

MINOR_CLASS( World, OpenGLRoutine )
	WorldOpenGLRoutine( shader_program::WorldOpenGLShaderProgram *shader_program ) : m_shader_program( shader_program ) {};

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

protected:
	shader_program::WorldOpenGLShaderProgram *m_shader_program;
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
